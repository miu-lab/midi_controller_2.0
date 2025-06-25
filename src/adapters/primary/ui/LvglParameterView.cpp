#include "LvglParameterView.hpp"
#include "config/DisplayConfig.hpp"
#include <Arduino.h>

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
    Serial.println(F("LvglParameterView: Constructor (MIDI→UI flow)"));
}

LvglParameterView::~LvglParameterView() {
    Serial.println(F("LvglParameterView: Destructor"));
    setActive(false);
    unsubscribeFromEvents();
    cleanupLvglObjects();
}

bool LvglParameterView::init() {
    if (initialized_) {
        Serial.println(F("LvglParameterView: Already initialized"));
        return true;
    }
    
    if (!bridge_) {
        Serial.println(F("LvglParameterView: Bridge LVGL non disponible"));
        return false;
    }
    
    Serial.println(F("LvglParameterView: Initialisation..."));
    
    // Créer l'écran principal
    setupMainScreen();

    // Créer le widget de paramètre
    createParameterWidget();

    // S'abonner aux événements MIDI
    subscribeToEvents();

    initialized_ = true;
    Serial.println(F("LvglParameterView: Initialisé avec succès"));
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

        Serial.println(F("LvglParameterView: Activé"));
    } else if (!active && active_) {
        // Désactivation
        active_ = false;

        // Masquer le widget
        if (parameter_widget_) {
            parameter_widget_->setVisible(false);
        }

        Serial.println(F("LvglParameterView: Désactivé"));
    }
}

//=============================================================================
// Interface paramètre MIDI
//=============================================================================

void LvglParameterView::setParameter(uint8_t cc_number, uint8_t channel, uint8_t value,
                                     const String& parameter_name, bool animate) {
    if (parameter_widget_) {
        parameter_widget_->setParameter(cc_number, channel, value, parameter_name, animate);
        Serial.printf("LvglParameterView: Parameter set - CC%d CH%d Value:%d Name:%s\n",
                      cc_number,
                      channel,
                      value,
                      parameter_name.c_str());
    } else {
        Serial.println(F("LvglParameterView: WARNING - No parameter widget available"));
    }
}

void LvglParameterView::setValue(uint8_t value, bool animate) {
    if (parameter_widget_) {
        parameter_widget_->setValue(value, animate);
        Serial.printf("LvglParameterView: Value set to %d\n", value);
    } else {
        Serial.println(F("LvglParameterView: WARNING - No parameter widget available"));
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

    Serial.println(F("LvglParameterView: Écran principal créé"));
}

void LvglParameterView::createParameterWidget() {
    if (!main_screen_) {
        Serial.println(F("LvglParameterView: Cannot create widget - no main screen"));
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

    Serial.println(F("LvglParameterView: ParameterWidget créé (read-only)"));
}

void LvglParameterView::cleanupLvglObjects() {
    // Nettoyer le widget en premier
    if (parameter_widget_) {
        parameter_widget_.reset();
        Serial.println(F("LvglParameterView: ParameterWidget détruit"));
    }

    // Nettoyer l'écran principal
    if (main_screen_) {
        lv_obj_delete(main_screen_);
        main_screen_ = nullptr;
        Serial.println(F("LvglParameterView: Écran principal détruit"));
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
        Serial.printf("LvglParameterView: Abonné aux événements UI batchés (ID: %d)\\n",
                      event_subscription_id_);
    } else {
        Serial.println(F("LvglParameterView: ERREUR - Échec abonnement événements"));
    }
}

void LvglParameterView::unsubscribeFromEvents() {
    if (event_subscription_id_ > 0) {
        EventBus& eventBus = EventBus::getInstance();
        if (eventBus.unsubscribe(event_subscription_id_)) {
            Serial.printf("LvglParameterView: Désabonné des événements (ID: %d)\\n",
                          event_subscription_id_);
        } else {
            Serial.printf("LvglParameterView: ERREUR - Échec désabonnement (ID: %d)\\n",
                          event_subscription_id_);
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