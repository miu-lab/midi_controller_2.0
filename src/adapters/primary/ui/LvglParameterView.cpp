#include "LvglParameterView.hpp"
#include "config/DisplayConfig.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include <Arduino.h>


// Note: Mapping CC→Widget et Button→Widget maintenant géré par WidgetMappingManager (Phase 5.3)

LvglParameterView::LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge,
                                     std::shared_ptr<UnifiedConfiguration> config,
                                     std::shared_ptr<EventBus> eventBus)
    : bridge_(bridge),
      config_(config),
      eventBus_(eventBus),
      initialized_(false),
      active_(false),
      event_subscription_id_(0) {
    // Créer le parser de configuration MIDI (Phase 5.2 refactoring)
    MidiConfigurationParser::ParserConfig parserConfig;
    parserConfig.enableLogging = false; // Peut être activé pour debug
    configParser_ = std::make_unique<MidiConfigurationParser>(parserConfig);
    
    // Créer le gestionnaire de mappings (Phase 5.3 refactoring)
    WidgetMappingManager::MappingConfig mappingConfig;
    mappingConfig.maxWidgets = 8;        // 8 widgets dans la grille 4x2
    mappingConfig.enableLogging = false; // Peut être activé pour debug
    mappingConfig.enableButtonMapping = true;
    mappingManager_ = std::make_unique<WidgetMappingManager>(mappingConfig);
    
    // Le ParameterEventHandler et LvglSceneManager seront créés après l'initialisation des mappings
    eventHandler_ = nullptr;
    sceneManager_ = nullptr;
}

LvglParameterView::~LvglParameterView() {
    // TODO DEBUG MSG
    setActive(false);
    unsubscribeFromEvents();
    // Le nettoyage des objets LVGL est maintenant géré par LvglSceneManager (Phase 5.5 refactoring)
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
    
    // Initialiser tous les mappings depuis la configuration (Phase 5.3 refactoring)
    initializeMappingsFromConfig();
    
    // Créer le gestionnaire de scène LVGL maintenant que les mappings sont prêts (Phase 5.5 refactoring)
    createSceneManager();
    
    // Créer le gestionnaire d'événements maintenant que la scène est prête (Phase 5.4 refactoring)
    createEventHandler();

    // S'abonner aux événements MIDI
    subscribeToEvents();

    initialized_ = true;
    
    // TODO DEBUG MSG
    return true;
}

void LvglParameterView::render() {
    if (!active_ || !initialized_ || !sceneManager_) {
        return;
    }
    
    // Charger l'écran LVGL via le gestionnaire de scène (Phase 5.5 refactoring)
    lv_obj_t* mainScreen = sceneManager_->getMainScreen();
    if (mainScreen) {
        lv_screen_load(mainScreen);
    }

    // Forcer le refresh du bridge
    // Le refresh est géré centralement par MidiControllerApp
}

void LvglParameterView::update() {
    if (!active_ || !initialized_ || !sceneManager_) {
        return;
    }

    // Traiter les mises à jour différées via le gestionnaire de scène (Phase 5.5 refactoring)
    const auto& widgets = sceneManager_->getWidgets();
    for (const auto& widget : widgets) {
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

        // S'assurer que tous les widgets sont visibles via le gestionnaire de scène (Phase 5.5 refactoring)
        if (sceneManager_) {
            sceneManager_->setWidgetsVisible(true);
        }
        
        // Activer le gestionnaire d'événements (Phase 5.4 refactoring)
        if (eventHandler_) {
            eventHandler_->setActive(true);
        }

        // TODO DEBUG MSG
    } else if (!active && active_) {
        // Désactivation
        active_ = false;

        // Masquer tous les widgets via le gestionnaire de scène (Phase 5.5 refactoring)
        if (sceneManager_) {
            sceneManager_->setWidgetsVisible(false);
        }
        
        // Désactiver le gestionnaire d'événements (Phase 5.4 refactoring)
        if (eventHandler_) {
            eventHandler_->setActive(false);
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
    // Déléguer la gestion d'événements au ParameterEventHandler (Phase 5.4 refactoring)
    if (eventHandler_ && eventHandler_->isActive()) {
        bool handled = eventHandler_->onEvent(event);
        
        // Pour compatibilité avec l'ancien comportement, traiter les événements UI directement aussi
        if (!handled && event.getType() == UIDisplayEvents::UIParameterUpdate) {
            const UIParameterUpdateEvent& uiEvent = static_cast<const UIParameterUpdateEvent&>(event);
            return handleUIParameterUpdateEvent(uiEvent);
        }
        
        return handled;
    }
    
    // Fallback : traitement direct si pas de gestionnaire d'événements
    if (event.getType() == UIDisplayEvents::UIParameterUpdate) {
        const UIParameterUpdateEvent& uiEvent = static_cast<const UIParameterUpdateEvent&>(event);
        return handleUIParameterUpdateEvent(uiEvent);
    }
    
    if (event.getType() == EventTypes::HighPriorityButtonPress) {
        handleButtonEvent(event);
        return false;  // NE PAS marquer comme "handled" pour permettre à d'autres de le traiter
    }

    return false;  // Événement non traité
}

//=============================================================================
// Méthodes privées - Gestion des mappings (Phase 5.3 refactoring)
//=============================================================================

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


void LvglParameterView::initializeMappingsFromConfig() {
    // Utiliser le parser et le mapping manager pour initialiser tous les mappings (Phase 5.3)
    if (!config_ || !configParser_ || !mappingManager_) {
        return;
    }
    
    // Extraire les données de configuration
    auto midiControls = configParser_->extractMidiControls(*config_);
    auto buttonInfos = configParser_->extractButtonInfo(*config_);
    
    // Initialiser tous les mappings via le gestionnaire
    mappingManager_->initializeMappings(midiControls, buttonInfos);
}

// NOTE: initializeWidgetConfigurationsFromConfig() supprimée - maintenant gérée par LvglSceneManager (Phase 5.5)

int8_t LvglParameterView::getWidgetIndexForCC(uint8_t cc_number) const {
    // Déléguer au gestionnaire de mappings (Phase 5.3)
    if (mappingManager_) {
        return mappingManager_->getWidgetIndexForCC(cc_number);
    }
    return -1;
}

ParameterWidget* LvglParameterView::getWidgetForCC(uint8_t cc_number) {
    int8_t index = getWidgetIndexForCC(cc_number);
    if (index >= 0 && index < 8 && sceneManager_) {
        return sceneManager_->getWidget(static_cast<uint8_t>(index));
    }
    return nullptr;
}

//=============================================================================
// Gestion des boutons
//=============================================================================

int8_t LvglParameterView::getWidgetIndexForButton(uint16_t button_id) const {
    // Déléguer au gestionnaire de mappings (Phase 5.3)
    if (mappingManager_) {
        return mappingManager_->getWidgetIndexForButton(button_id);
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
    if (index >= 0 && index < 8 && sceneManager_) {
        return sceneManager_->getWidget(static_cast<uint8_t>(index));
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


// NOTE: setupButtonIndicators() et finalizeButtonIndicatorPositions() supprimées - 
// maintenant gérées par LvglSceneManager (Phase 5.5)

//=============================================================================
// Création du gestionnaire de scène LVGL (Phase 5.5 refactoring)
//=============================================================================

void LvglParameterView::createSceneManager() {
    if (!mappingManager_) {
        return;
    }
    
    // Configuration du gestionnaire de scène
    LvglSceneManager::SceneConfig sceneConfig;
    sceneConfig.maxWidgets = 8;           // 8 widgets dans la grille 4x2
    sceneConfig.screenWidth = 320;        // ILI9341 320x240
    sceneConfig.screenHeight = 240;
    sceneConfig.gridCols = 4;             // Grille 4x2
    sceneConfig.gridRows = 2;
    sceneConfig.widgetWidth = 80;         // Largeur d'un widget
    sceneConfig.widgetHeight = 120;       // Hauteur d'un widget
    sceneConfig.arcRadius = 70;           // Rayon de l'arc des widgets
    sceneConfig.buttonIndicatorSize = 12; // Taille des indicateurs de bouton
    sceneConfig.backgroundColor = 0x000000; // Fond noir
    sceneConfig.enableLogging = false;    // Peut être activé pour debug
    
    // Créer le gestionnaire de scène avec les mappings partagés
    sceneManager_ = std::make_unique<LvglSceneManager>(
        sceneConfig,
        std::shared_ptr<WidgetMappingManager>(mappingManager_.get(), [](WidgetMappingManager*){})
    );
    
    // Fonction d'accès à la configuration des widgets
    auto widgetConfigAccessor = [this](uint8_t index) -> LvglSceneManager::WidgetConfig* {
        static std::array<LvglSceneManager::WidgetConfig, 8> configs;
        
        if (!configParser_ || !config_) {
            return nullptr;
        }
        
        // Extraire les contrôles MIDI depuis la configuration
        auto midiControls = configParser_->extractMidiControls(*config_);
        
        if (index < midiControls.size()) {
            const auto& control = midiControls[index];
            configs[index].cc_number = control.cc_number;
            configs[index].channel = control.channel + 1;  // Convertir 0-based vers 1-based
            configs[index].value = 0;  // Valeur initiale à 0
            configs[index].name = control.name;
            configs[index].visible = true;
            return &configs[index];
        } else if (index < 8) {
            // Configuration par défaut pour les widgets sans mapping
            configs[index].cc_number = index + 1;
            configs[index].channel = 1;
            configs[index].value = 0;
            configs[index].name = "CC" + String(index + 1);
            configs[index].visible = false;  // Masquer les widgets non mappés
            return &configs[index];
        }
        
        return nullptr;
    };
    
    // Initialiser la scène LVGL
    bool success = sceneManager_->initializeScene(widgetConfigAccessor);
    if (success) {
        // Finaliser le positionnement après l'initialisation LVGL
        sceneManager_->finalizePositioning();
    }
}

//=============================================================================
// Création du gestionnaire d'événements (Phase 5.4 refactoring)
//=============================================================================

void LvglParameterView::createEventHandler() {
    if (!mappingManager_ || !sceneManager_) {
        return;
    }
    
    // Configuration du gestionnaire d'événements
    ParameterEventHandler::EventConfig eventConfig;
    eventConfig.enableMidiEvents = true;
    eventConfig.enableButtonEvents = true;
    eventConfig.enableLogging = false;  // Peut être activé pour debug
    eventConfig.enableAnimation = true;
    
    // Fonction d'accès aux widgets via le gestionnaire de scène (Phase 5.5 refactoring)
    auto widgetAccessor = [this](uint8_t index) -> ParameterWidget* {
        if (sceneManager_) {
            return sceneManager_->getWidget(index);
        }
        return nullptr;
    };
    
    // Créer le gestionnaire d'événements avec les mappings partagés
    eventHandler_ = std::make_unique<ParameterEventHandler>(
        eventConfig,
        widgetAccessor,
        std::shared_ptr<WidgetMappingManager>(mappingManager_.get(), [](WidgetMappingManager*){})
    );
}