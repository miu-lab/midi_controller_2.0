#include "LvglParameterView.hpp"
#include "config/DisplayConfig.hpp"
#include <Arduino.h>
#include "config/debug/DebugMacros.hpp"

LvglParameterView::LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge)
    : bridge_(bridge),
      parameter_widget_(nullptr),
      main_screen_(nullptr),
      initialized_(false),
      active_(false),
      event_subscription_id_(0),
      last_cc_number_(0),
      last_channel_(1),
      last_value_(0),
      last_parameter_name_("NO PARAM") {
    DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Constructor (MIDI→UI flow)");
}

LvglParameterView::~LvglParameterView() {
    DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Destructor");
    setActive(false);
    unsubscribeFromEvents();
    cleanupLvglObjects();
}

bool LvglParameterView::init() {
    if (initialized_) {
        DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Already initialized");
        return true;
    }
    
    if (!bridge_) {
        DEBUG_LOG(DEBUG_LEVEL_ERROR, "LvglParameterView: Bridge LVGL non disponible");
        return false;
    }
    
    DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Initialisation...");
    
    // Créer l'écran principal
    setupMainScreen();

    // Créer le widget de paramètre
    createParameterWidget();

    // S'abonner aux événements MIDI
    subscribeToEvents();

    initialized_ = true;
    DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Initialisé avec succès");
    return true;
}

void LvglParameterView::render() {
    if (!active_ || !initialized_) {
        return;
    }
    
    // Charger l'écran LVGL
    if (main_screen_) {
        lv_screen_load(main_screen_);
    }

    // Forcer le refresh du bridge
    // Le refresh est géré centralement par MidiControllerApp
}

void LvglParameterView::update() {
    if (!active_ || !initialized_) {
        return;
    }

    // Traiter les mises à jour différées du ParameterWidget
    if (parameter_widget_) {
        parameter_widget_->processPendingUpdates();
    }
}

bool LvglParameterView::isActive() const {
    return active_;
}

void LvglParameterView::setActive(bool active) {
    if (active && !active_) {
        // Activation
        active_ = true;

        // S'assurer que le widget est visible
        if (parameter_widget_) {
            parameter_widget_->setVisible(true);
        }

        DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Activé");
    } else if (!active && active_) {
        // Désactivation
        active_ = false;

        // Masquer le widget
        if (parameter_widget_) {
            parameter_widget_->setVisible(false);
        }

        DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Désactivé");
    }
}

//=============================================================================
// Interface paramètre MIDI
//=============================================================================

void LvglParameterView::setParameter(uint8_t cc_number, uint8_t channel, uint8_t value,
                                     const String& parameter_name, bool animate) {
    if (parameter_widget_) {
        parameter_widget_->setParameter(cc_number, channel, value, parameter_name, animate);
        DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Parameter set - CC%d CH%d Value:%d Name:%s", cc_number, channel, value, parameter_name.c_str());
    } else {
        DEBUG_LOG(DEBUG_LEVEL_WARNING, "LvglParameterView: WARNING - No parameter widget available");
    }
}

void LvglParameterView::setValue(uint8_t value, bool animate) {
    if (parameter_widget_) {
        parameter_widget_->setValue(value, animate);
        DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Value set to %d", value);
    } else {
        DEBUG_LOG(DEBUG_LEVEL_WARNING, "LvglParameterView: WARNING - No parameter widget available");
    }
}

//=============================================================================
// Interface Event Listener
//=============================================================================

bool LvglParameterView::onEvent(const Event& event) {
    // Filtrer uniquement les événements UI batchés
    if (event.getType() == UIDisplayEvents::UIParameterUpdate) {
        const UIParameterUpdateEvent& uiEvent = static_cast<const UIParameterUpdateEvent&>(event);
        return handleUIParameterUpdateEvent(uiEvent);
    }

    return false;  // Événement non traité
}

//=============================================================================
// Méthodes privées
//=============================================================================

void LvglParameterView::setupMainScreen() {
    // Créer l'écran principal
    main_screen_ = lv_obj_create(nullptr);

    // Style de l'écran - fond sombre
    lv_obj_set_style_bg_color(main_screen_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(main_screen_, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(main_screen_, 0, 0);

    DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Écran principal créé");
}

void LvglParameterView::createParameterWidget() {
    if (!main_screen_) {
        DEBUG_LOG(DEBUG_LEVEL_WARNING, "LvglParameterView: Cannot create widget - no main screen");
        return;
    }

    // Créer le widget avec dimensions adaptées à l'écran ILI9341 (320x240)
    // Le constructeur legacy est utilisé temporairement (read-only par défaut)
    parameter_widget_ = std::make_unique<ParameterWidget>(main_screen_);

    // Centrer le widget sur l'écran
    parameter_widget_->center();

    // Définir un paramètre initial vide
    parameter_widget_->setParameter(last_cc_number_,
                                    last_channel_,
                                    last_value_,
                                    last_parameter_name_,
                                    false);

    DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: ParameterWidget créé (read-only)");
}

void LvglParameterView::cleanupLvglObjects() {
    // Nettoyer le widget en premier
    if (parameter_widget_) {
        parameter_widget_.reset();
        DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: ParameterWidget détruit");
    }

    // Nettoyer l'écran principal
    if (main_screen_) {
        lv_obj_delete(main_screen_);
        main_screen_ = nullptr;
        DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Écran principal détruit");
    }
}

//=============================================================================
// Event handling
//=============================================================================

void LvglParameterView::subscribeToEvents() {
    EventBus& eventBus = EventBus::getInstance();
    // S'abonner aux événements UI batchés au lieu des MIDI bruts
    event_subscription_id_ = eventBus.subscribe(this, 100);  // Haute priorité pour l'UI

    if (event_subscription_id_ > 0) {
        DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Abonné aux événements UI batchés (ID: %d)", event_subscription_id_);
    } else {
        DEBUG_LOG(DEBUG_LEVEL_ERROR, "LvglParameterView: ERREUR - Échec abonnement événements");
    }
}

void LvglParameterView::unsubscribeFromEvents() {
    if (event_subscription_id_ > 0) {
        EventBus& eventBus = EventBus::getInstance();
        if (eventBus.unsubscribe(event_subscription_id_)) {
            DEBUG_LOG(DEBUG_LEVEL_INFO, "LvglParameterView: Désabonné des événements (ID: %d)", event_subscription_id_);
        } else {
            DEBUG_LOG(DEBUG_LEVEL_ERROR, "LvglParameterView: ERREUR - Échec désabonnement (ID: %d)", event_subscription_id_);
        }
        event_subscription_id_ = 0;
    }
}

bool LvglParameterView::handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event) {
    // Stocker les dernières valeurs reçues (déjà optimisées par le batcher)
    last_cc_number_ = event.controller;
    last_channel_ = event.channel + 1;  // Convertir 0-15 vers 1-16
    last_value_ = event.value;
    last_parameter_name_ = event.parameter_name.length() == 0 ? 
                          ("CC" + String(event.controller)) : event.parameter_name;

    // Mettre à jour l'affichage directement (pas de throttling, déjà fait par le batcher)
    if (parameter_widget_ && active_ && initialized_) {
        parameter_widget_->setParameter(last_cc_number_,
                                        last_channel_,
                                        last_value_,
                                        last_parameter_name_,
                                        true);
    }

    return true;  // Événement traité
}