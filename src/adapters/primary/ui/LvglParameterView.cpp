#include "LvglParameterView.hpp"
#include "config/DisplayConfig.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include <Arduino.h>


// Initialisation du mapping CC vers index de widget
// CC 1-8 mappent vers widgets 0-7, autres CC non mappés (-1)
static uint8_t cc_to_widget_mapping[128];
static bool mapping_initialized = false;

LvglParameterView::LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge,
                                     std::shared_ptr<UnifiedConfiguration> config,
                                     std::shared_ptr<EventBus> eventBus)
    : bridge_(bridge),
      config_(config),
      eventBus_(eventBus),
      grid_container_(nullptr),
      main_screen_(nullptr),
      initialized_(false),
      active_(false),
      event_subscription_id_(0) {
    // TODO DEBUG MSG
}

LvglParameterView::~LvglParameterView() {
    // TODO DEBUG MSG
    setActive(false);
    unsubscribeFromEvents();
    cleanupLvglObjects();
}

bool LvglParameterView::init() {
    if (initialized_) {
        // TODO DEBUG MSG
        return true;
    }
    
    if (!bridge_) {
        // TODO DEBUG MSG
        return false;
    }
    
    // TODO DEBUG MSG
    
    // Créer l'écran principal
    setupMainScreen();

    // Initialiser le mapping CC->Widget depuis la configuration
    initializeCCMappingFromConfig();
    
    // Initialiser le mapping boutons depuis la configuration
    initializeButtonMappingFromConfig();
    
    // Créer le container grille
    createGridContainer();
    
    // Créer les widgets de paramètres avec configuration
    createParameterWidgets();
    
    // Initialiser les configurations des widgets depuis la configuration
    initializeWidgetConfigurationsFromConfig();
    
    // Configurer les indicateurs de boutons
    setupButtonIndicators();

    // S'abonner aux événements MIDI
    subscribeToEvents();

    initialized_ = true;
    
    // POST-TRAITEMENT : Finaliser le positionnement des LEDs après l'initialisation LVGL
    finalizeButtonIndicatorPositions();
    
    // TODO DEBUG MSG
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

        // TODO DEBUG MSG
    } else if (!active && active_) {
        // Désactivation
        active_ = false;

        // Masquer tous les widgets
        for (auto& widget : parameter_widgets_) {
            if (widget) {
                widget->setVisible(false);
            }
        }

        // TODO DEBUG MSG
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
        // TODO DEBUG MSG
    } else {
        // TODO DEBUG MSG
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
    
    // Gérer les événements de boutons (high priority)
    if (event.getType() == EventTypes::HighPriorityButtonPress) {
        handleButtonEvent(event);
        return false;  // NE PAS marquer comme "handled" pour permettre à d'autres de le traiter
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

    // TODO DEBUG MSG
}

void LvglParameterView::createGridContainer() {
    if (!main_screen_) {
        // TODO DEBUG MSG
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
    
    // TODO DEBUG MSG
}

void LvglParameterView::createParameterWidgets() {
    if (!grid_container_) {
        // TODO DEBUG MSG
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

    // TODO DEBUG MSG
}

void LvglParameterView::cleanupLvglObjects() {
    // Nettoyer tous les widgets en premier
    for (auto& widget : parameter_widgets_) {
        if (widget) {
            widget.reset();
        }
    }
    // TODO DEBUG MSG
    
    // Nettoyer le container grille
    if (grid_container_) {
        lv_obj_delete(grid_container_);
        grid_container_ = nullptr;
        // TODO DEBUG MSG
    }

    // Nettoyer l'écran principal
    if (main_screen_) {
        lv_obj_delete(main_screen_);
        main_screen_ = nullptr;
        // TODO DEBUG MSG
    }
}

//=============================================================================
// Event handling
//=============================================================================

void LvglParameterView::subscribeToEvents() {
    if (!eventBus_) {
        return;
    }
    
    // S'abonner avec HAUTE PRIORITÉ pour recevoir les événements HighPriorityButtonPress
    event_subscription_id_ = eventBus_->subscribeHigh(this);
}

void LvglParameterView::unsubscribeFromEvents() {
    if (event_subscription_id_ > 0 && eventBus_) {
        eventBus_->unsubscribe(event_subscription_id_);
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
        Serial.println("[DEBUG] No config, using fallback MIDI controls");
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
    Serial.printf("[DEBUG] Extracting MIDI controls from %d total controls\n", allControls.size());
    
    for (const auto& control : allControls) {
        // Chercher les mappings MIDI pour ce contrôle
        auto midiMappings = control.getMappingsForRole(MappingRole::MIDI);
        
        if (!midiMappings.empty()) {
            Serial.printf("[DEBUG] Control %d (%s) has %d MIDI mappings\n", control.id, control.label.c_str(), midiMappings.size());
        }
        
        for (const auto& mapping : midiMappings) {
            if (std::holds_alternative<ControlDefinition::MidiConfig>(mapping.config)) {
                auto midiConfig = std::get<ControlDefinition::MidiConfig>(mapping.config);
                
                // FILTRE CRITIQUE : Ne prendre que les encodeurs (pas les boutons)
                if (control.hardware.type == InputType::ENCODER) {
                    // Ajouter seulement les contrôles MIDI des encodeurs
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
                    
                    Serial.printf("[DEBUG] Added ENCODER MIDI control: ID=%d, CC=%d, CH=%d, Name=%s\n", 
                                 info.control_id, info.cc_number, info.channel, info.name.c_str());
                } else {
                    Serial.printf("[DEBUG] Skipped non-encoder MIDI control: ID=%d, Type=%d\n", 
                                 control.id, static_cast<int>(control.hardware.type));
                }
            }
        }
    }
    
    Serial.printf("[DEBUG] Extracted %d MIDI controls total\n", midiControls.size());
    
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
            // TODO DEBUG MSG
            widgetIndex++;
        }
    }
    
    mapping_initialized = true;
    // TODO DEBUG MSG
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
            // TODO DEBUG MSG
        }
    }
    
    // Si moins de 8 contrôles configurés, masquer les widgets excédentaires
    for (size_t i = midiControls.size(); i < 8; i++) {
        if (parameter_widgets_[i]) {
            parameter_widgets_[i]->setVisible(false);
            // TODO DEBUG MSG
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

//=============================================================================
// Gestion des boutons
//=============================================================================

int8_t LvglParameterView::getWidgetIndexForButton(uint16_t button_id) const {
    auto it = button_to_widget_mapping_.find(button_id);
    if (it != button_to_widget_mapping_.end()) {
        return static_cast<int8_t>(it->second);
    }
    return -1;
}

void LvglParameterView::setButtonState(uint16_t button_id, bool pressed, bool animate) {
    // Trouver le widget parent pour ce bouton
    ParameterWidget* widget = getWidgetForButton(button_id);
    if (widget && widget->hasButtonIndicator()) {
        widget->setButtonState(pressed, animate);
    }
}

ParameterWidget* LvglParameterView::getWidgetForButton(uint16_t button_id) {
    int8_t index = getWidgetIndexForButton(button_id);
    if (index >= 0 && index < 8 && parameter_widgets_[index]) {
        return parameter_widgets_[index].get();
    }
    return nullptr;
}

bool LvglParameterView::handleButtonEvent(const Event& event) {
    // Cast vers HighPriorityButtonPressEvent
    if (event.getType() == EventTypes::HighPriorityButtonPress) {
        const auto& buttonEvent = static_cast<const HighPriorityButtonPressEvent&>(event);
        
        // Mettre à jour l'état du bouton
        setButtonState(buttonEvent.buttonId, buttonEvent.pressed, true);
        
        return true;  // Événement traité
    }
    
    return false;  // Événement non traité
}

void LvglParameterView::initializeButtonMappingFromConfig() {
    // Nettoyer les mappings existants
    button_to_widget_mapping_.clear();
    standalone_buttons_.clear();
    
    // Extraire les informations sur tous les boutons
    auto buttonInfos = extractButtonInfoFromConfig();
    Serial.printf("[DEBUG] Found %d buttons in config\n", buttonInfos.size());
    
    // Créer les mappings pour les boutons avec parents (encodeurs)
    for (const auto& info : buttonInfos) {
        Serial.printf("[DEBUG] Button ID: %d, Parent: %d, Name: %s\n", info.button_id, info.parent_encoder_id, info.name.c_str());
        
        if (info.hasParent()) {
            // Trouver le widget index de l'encodeur parent
            uint16_t encoder_id = info.parent_encoder_id;
            
            // Chercher l'index du widget qui correspond à cet encodeur
            // (basé sur le mapping CC->Widget existant)
            auto midiControls = extractMidiControlsFromConfig();
            for (size_t i = 0; i < std::min(midiControls.size(), static_cast<size_t>(8)); i++) {
                if (midiControls[i].control_id == encoder_id) {
                    button_to_widget_mapping_[info.button_id] = i;
                    Serial.printf("[DEBUG] Mapped button %d to widget %d (encoder %d)\n", info.button_id, i, encoder_id);
                    break;
                }
            }
        } else {
            // Bouton indépendant - l'ajouter à la liste des standalone
            standalone_buttons_.push_back(info);
            Serial.printf("[DEBUG] Standalone button: %d\n", info.button_id);
        }
    }
    
    Serial.printf("[DEBUG] Total button mappings: %d\n", button_to_widget_mapping_.size());
}

void LvglParameterView::setupButtonIndicators() {
    // Ajouter des indicateurs aux widgets qui ont des boutons enfants
    for (const auto& mapping : button_to_widget_mapping_) {
        uint8_t widget_index = mapping.second;
        
        if (widget_index < 8 && parameter_widgets_[widget_index]) {
            // Ajouter un indicateur de bouton à ce widget
            parameter_widgets_[widget_index]->addButtonIndicator(12);  // Taille 12px
        }
    }
    
    // TODO: Créer des widgets séparés pour les boutons indépendants
    // Pour l'instant, on se concentre sur les boutons d'encodeurs
}

std::vector<LvglParameterView::ButtonInfo> LvglParameterView::extractButtonInfoFromConfig() {
    std::vector<ButtonInfo> buttonInfos;
    
    if (!config_) {
        Serial.println("[DEBUG] No config available, using fallback");
        return buttonInfos;
    }
    
    // Obtenir tous les contrôles depuis la configuration
    auto allControls = config_->getAllControls();
    Serial.printf("[DEBUG] Found %d total controls in config\n", allControls.size());
    
    for (const auto& control : allControls) {
        if (control.hardware.type == InputType::BUTTON && control.enabled) {
            ButtonInfo info;
            info.button_id = control.id;
            info.name = control.label.length() > 0 ? String(control.label.c_str()) : ("BTN" + String(control.id));
            
            // Déterminer si ce bouton a un parent encodeur
            if (control.parentId.has_value()) {
                info.parent_encoder_id = control.parentId.value();
                Serial.printf("[DEBUG] Found button %d with parent %d\n", info.button_id, info.parent_encoder_id);
            } else {
                info.parent_encoder_id = 0;  // Pas de parent
                Serial.printf("[DEBUG] Found standalone button %d\n", info.button_id);
            }
            
            buttonInfos.push_back(info);
        }
        
        // Vérifier aussi les boutons intégrés aux encodeurs
        if (control.hardware.type == InputType::ENCODER && control.enabled) {
            if (control.hardware.encoderButtonPin.has_value()) {
                ButtonInfo info;
                info.button_id = control.getEncoderButtonId();  // ID encodeur + 1000
                info.parent_encoder_id = control.id;  // L'encodeur est le parent
                info.name = String(control.label.c_str()) + " BTN";
                
                Serial.printf("[DEBUG] Found integrated encoder button %d for encoder %d\n", info.button_id, info.parent_encoder_id);
                buttonInfos.push_back(info);
            }
        }
    }
    
    Serial.printf("[DEBUG] Extracted %d button infos from config\n", buttonInfos.size());
    return buttonInfos;
}

void LvglParameterView::finalizeButtonIndicatorPositions() {
    // Ré-appliquer les positions et tailles après que LVGL ait fini l'initialisation
    for (const auto& mapping : button_to_widget_mapping_) {
        uint8_t widget_index = mapping.second;
        
        if (widget_index < 8 && parameter_widgets_[widget_index]) {
            ParameterWidget* widget = parameter_widgets_[widget_index].get();
            if (widget && widget->hasButtonIndicator()) {
                ButtonIndicator* indicator = widget->getButtonIndicator();
                if (indicator && indicator->getLedObject()) {
                    lv_obj_t* led = indicator->getLedObject();
                    
                    // Forcer la taille à nouveau
                    lv_obj_set_size(led, 12, 12);
                    
                    // Centrer la LED sur son parent (arc)
                    lv_obj_center(led);
                    
                    // Forcer au premier plan
                    lv_obj_move_foreground(led);
                    
                    // Invalider pour redraw
                    lv_obj_invalidate(led);
                }
            }
        }
    }
}