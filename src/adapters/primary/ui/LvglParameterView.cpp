#include "LvglParameterView.hpp"
#include "config/DisplayConfig.hpp"
#include <Arduino.h>


// Initialisation du mapping CC vers index de widget
// CC 1-8 mappent vers widgets 0-7, autres CC non mappés (-1)
static uint8_t cc_to_widget_mapping[128];
static bool mapping_initialized = false;

LvglParameterView::LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge,
                                     std::shared_ptr<UnifiedConfiguration> config)
    : bridge_(bridge),
      config_(config),
      grid_container_(nullptr),
      main_screen_(nullptr),
      initialized_(false),
      active_(false),
      event_subscription_id_(0) {
    // DEBUG MSG TO IMPLEMENT
}

LvglParameterView::~LvglParameterView() {
    // DEBUG MSG TO IMPLEMENT
    setActive(false);
    unsubscribeFromEvents();
    cleanupLvglObjects();
}

bool LvglParameterView::init() {
    if (initialized_) {
        // DEBUG MSG TO IMPLEMENT
        return true;
    }
    
    if (!bridge_) {
        // DEBUG MSG TO IMPLEMENT
        return false;
    }
    
    // DEBUG MSG TO IMPLEMENT
    
    // Créer l'écran principal
    setupMainScreen();

    // Initialiser le mapping CC->Widget depuis la configuration
    initializeCCMappingFromConfig();
    
    // Créer le container grille
    createGridContainer();
    
    // Créer les widgets de paramètres avec configuration
    createParameterWidgets();
    
    // Initialiser les configurations des widgets depuis la configuration
    initializeWidgetConfigurationsFromConfig();

    // S'abonner aux événements MIDI
    subscribeToEvents();

    initialized_ = true;
    // DEBUG MSG TO IMPLEMENT
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

    // Traiter les mises à jour différées de tous les ParameterWidget
    for (auto& widget : parameter_widgets_) {
        if (widget) {
            widget->processPendingUpdates();
        }
    }
}

bool LvglParameterView::isActive() const {
    return active_;
}

void LvglParameterView::setActive(bool active) {
    if (active && !active_) {
        // Activation
        active_ = true;

        // S'assurer que tous les widgets sont visibles
        for (auto& widget : parameter_widgets_) {
            if (widget) {
                widget->setVisible(true);
            }
        }

        // DEBUG MSG TO IMPLEMENT
    } else if (!active && active_) {
        // Désactivation
        active_ = false;

        // Masquer tous les widgets
        for (auto& widget : parameter_widgets_) {
            if (widget) {
                widget->setVisible(false);
            }
        }

        // DEBUG MSG TO IMPLEMENT
    }
}

//=============================================================================
// Interface paramètre MIDI
//=============================================================================

void LvglParameterView::setParameter(uint8_t cc_number, uint8_t channel, uint8_t value,
                                     const String& parameter_name, bool animate) {
    ParameterWidget* widget = getWidgetForCC(cc_number);
    if (widget) {
        widget->setParameter(cc_number, channel, value, parameter_name, animate);
        // DEBUG MSG TO IMPLEMENT
    } else {
        // DEBUG MSG TO IMPLEMENT
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

    // DEBUG MSG TO IMPLEMENT
}

void LvglParameterView::createGridContainer() {
    if (!main_screen_) {
        // DEBUG MSG TO IMPLEMENT
        return;
    }

    // Créer le container principal pour la grille
    grid_container_ = lv_obj_create(main_screen_);
    
    // Configuration du container pour une grille 4x2
    lv_obj_set_size(grid_container_, 320, 240);  // Taille plein écran ILI9341
    lv_obj_set_pos(grid_container_, 0, 0);
    
    // // Style du container
    lv_obj_set_style_bg_opa(grid_container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid_container_, 0, 0);
    lv_obj_set_style_pad_all(grid_container_, 0, 0);
    lv_obj_set_style_pad_gap(grid_container_, 0, 0);
    
    // Configurer le layout en grille 4x2
    static lv_coord_t col_dsc[] = {80, 80, 80, 80, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {120, 120, LV_GRID_TEMPLATE_LAST};
    
    lv_obj_set_grid_dsc_array(grid_container_, col_dsc, row_dsc);
    lv_obj_set_layout(grid_container_, LV_LAYOUT_GRID);
    
    // DEBUG MSG TO IMPLEMENT
}

void LvglParameterView::createParameterWidgets() {
    if (!grid_container_) {
        // DEBUG MSG TO IMPLEMENT
        return;
    }

    // Créer seulement 4 widgets pour économiser la mémoire (grille 4x1)
    // Économie: ~8KB par rapport aux 8 widgets originaux
    for (uint8_t i = 0; i < 8; i++) {
        // Créer le widget avec dimensions adaptées pour la grille (constructeur legacy)
        parameter_widgets_[i] = std::make_unique<ParameterWidget>(grid_container_, 80, 120, 70);
        
        // Calculer position dans la grille (4 colonnes, 2 lignes)
        uint8_t col = i % 4;
        uint8_t row = i / 4;
        
        // Positionner dans la grille
        lv_obj_set_grid_cell(parameter_widgets_[i]->getContainer(), 
                             LV_GRID_ALIGN_CENTER, col, 1,
                             LV_GRID_ALIGN_CENTER, row, 1);
        
        // Les paramètres initiaux seront définis dans initializeWidgetConfigurations()
        // après que le mapping soit établi
    }

    // DEBUG MSG TO IMPLEMENT
}

void LvglParameterView::cleanupLvglObjects() {
    // Nettoyer tous les widgets en premier
    for (auto& widget : parameter_widgets_) {
        if (widget) {
            widget.reset();
        }
    }
    // DEBUG MSG TO IMPLEMENT
    
    // Nettoyer le container grille
    if (grid_container_) {
        lv_obj_delete(grid_container_);
        grid_container_ = nullptr;
        // DEBUG MSG TO IMPLEMENT
    }

    // Nettoyer l'écran principal
    if (main_screen_) {
        lv_obj_delete(main_screen_);
        main_screen_ = nullptr;
        // DEBUG MSG TO IMPLEMENT
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
        // DEBUG MSG TO IMPLEMENT
    } else {
        // DEBUG MSG TO IMPLEMENT
    }
}

void LvglParameterView::unsubscribeFromEvents() {
    if (event_subscription_id_ > 0) {
        EventBus& eventBus = EventBus::getInstance();
        if (eventBus.unsubscribe(event_subscription_id_)) {
            // DEBUG MSG TO IMPLEMENT
        } else {
            // DEBUG MSG TO IMPLEMENT
        }
        event_subscription_id_ = 0;
    }
}

bool LvglParameterView::handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event) {
    // Les valeurs sont traitées directement sans stockage global
    // car chaque widget gère son propre état

    // Mettre à jour le widget correspondant au CC reçu (pas de throttling, déjà fait par le batcher)
    if (active_ && initialized_) {
        ParameterWidget* widget = getWidgetForCC(event.controller);
        if (widget) {
            uint8_t channel = event.channel + 1;  // Convertir 0-15 vers 1-16
            String parameter_name = event.parameter_name.length() == 0 ? 
                                  ("CC" + String(event.controller)) : event.parameter_name;
            
            widget->setParameter(event.controller,
                               channel,
                               event.value,
                               parameter_name,
                               true);
        }
    }

    return true;  // Événement traité
}

//=============================================================================
// Méthodes de gestion du mapping CC->Widget
//=============================================================================

std::vector<LvglParameterView::MidiControlInfo> LvglParameterView::extractMidiControlsFromConfig() {
    std::vector<MidiControlInfo> midiControls;
    
    if (!config_) {
        // DEBUG MSG TO IMPLEMENT
        // Fallback vers la configuration par défaut
        for (uint8_t i = 0; i < 8; i++) {
            MidiControlInfo info;
            info.cc_number = i + 1;
            info.channel = 0;  // Canal 0 (MIDI canal 1)
            info.name = "ENC" + String(i + 1);
            info.control_id = 71 + i;  // IDs des encodeurs selon ConfigurationFactory
            midiControls.push_back(info);
        }
        return midiControls;
    }
    
    // Obtenir tous les contrôles depuis la configuration
    auto allControls = config_->getAllControls();
    
    for (const auto& control : allControls) {
        // Chercher les mappings MIDI pour ce contrôle
        auto midiMappings = control.getMappingsForRole(MappingRole::MIDI);
        
        for (const auto& mapping : midiMappings) {
            if (std::holds_alternative<ControlDefinition::MidiConfig>(mapping.config)) {
                auto midiConfig = std::get<ControlDefinition::MidiConfig>(mapping.config);
                
                // Ajouter tous les contrôles MIDI CC
                MidiControlInfo info;
                info.cc_number = midiConfig.control;
                info.channel = midiConfig.channel; 
                
                // Gérer la concaténation des strings correctement
                if (control.label.length() > 0) {
                    info.name = control.label.c_str();
                } else {
                    info.name = "CC" + String(midiConfig.control);
                }
                
                info.control_id = control.id;
                midiControls.push_back(info);
                
                // DEBUG MSG TO IMPLEMENT
            }
        }
    }
    
    // DEBUG MSG TO IMPLEMENT
    
    return midiControls;
}

void LvglParameterView::initializeCCMappingFromConfig() {
    if (mapping_initialized) {
        return;
    }
    
    // Initialiser tout le tableau à 255 (pas de mapping)
    for (int i = 0; i < 128; i++) {
        cc_to_widget_mapping[i] = 255;
    }
    
    // Extraire les contrôles MIDI depuis la configuration
    auto midiControls = extractMidiControlsFromConfig();
    
    // Mapper les 8 premiers contrôles aux widgets
    size_t widgetIndex = 0;
    for (const auto& control : midiControls) {
        if (widgetIndex >= 8) break;  // Maximum 8 widgets
        
        if (control.cc_number < 128) {
            cc_to_widget_mapping[control.cc_number] = widgetIndex;
            // DEBUG MSG TO IMPLEMENT
            widgetIndex++;
        }
    }
    
    mapping_initialized = true;
    // DEBUG MSG TO IMPLEMENT
}

void LvglParameterView::initializeWidgetConfigurationsFromConfig() {
    // Extraire les contrôles MIDI depuis la configuration
    auto midiControls = extractMidiControlsFromConfig();
    
    // Configurer chaque widget avec les paramètres réels
    for (size_t i = 0; i < std::min(midiControls.size(), static_cast<size_t>(8)); i++) {
        if (parameter_widgets_[i]) {
            const auto& control = midiControls[i];
            uint8_t channel = control.channel + 1;  // Convertir 0-based vers 1-based
            
            // Valeur initiale à 0 comme demandé
            parameter_widgets_[i]->setParameter(control.cc_number, channel, 0, control.name, false);
            // DEBUG MSG TO IMPLEMENT
        }
    }
    
    // Si moins de 8 contrôles configurés, masquer les widgets excédentaires
    for (size_t i = midiControls.size(); i < 8; i++) {
        if (parameter_widgets_[i]) {
            parameter_widgets_[i]->setVisible(false);
            // DEBUG MSG TO IMPLEMENT
        }
    }
}

int8_t LvglParameterView::getWidgetIndexForCC(uint8_t cc_number) const {
    if (cc_number >= 128) {
        return -1;
    }
    
    uint8_t index = cc_to_widget_mapping[cc_number];
    return (index == 255) ? -1 : static_cast<int8_t>(index);
}

ParameterWidget* LvglParameterView::getWidgetForCC(uint8_t cc_number) {
    int8_t index = getWidgetIndexForCC(cc_number);
    if (index >= 0 && index < 8 && parameter_widgets_[index]) {
        return parameter_widgets_[index].get();
    }
    return nullptr;
}