#include "ParameterViewController.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include <Arduino.h>

ParameterViewController::ParameterViewController(const ControllerConfig& config,
                                               std::shared_ptr<Ili9341LvglBridge> bridge,
                                               std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                                               std::shared_ptr<EventBus> eventBus)
    : config_(config)
    , bridge_(bridge)
    , unifiedConfig_(unifiedConfig)
    , eventBus_(eventBus)
    , initialized_(false)
    , active_(false)
    , event_subscription_id_(0)
    , configParser_(nullptr)
    , mappingManager_(nullptr)
    , eventHandler_(nullptr)
    , sceneManager_(nullptr) {
    
    logInfo("ParameterViewController created with " + String(config_.maxWidgets) + " max widgets");
}

ParameterViewController::~ParameterViewController() {
    setActive(false);
    unsubscribeFromEvents();
    
    // Nettoyage dans l'ordre inverse de création
    eventHandler_.reset();
    sceneManager_.reset();
    mappingManager_.reset();
    configParser_.reset();
    
    logInfo("ParameterViewController destroyed");
}

bool ParameterViewController::initialize() {
    if (initialized_) {
        logDebug("Controller already initialized");
        return true;
    }
    
    if (!bridge_ || !unifiedConfig_ || !eventBus_) {
        logError("Missing required dependencies for initialization");
        return false;
    }
    
    logInfo("Initializing ParameterViewController...");
    
    // Initialiser les composants dans l'ordre des dépendances
    if (!initializeConfigParser()) {
        logError("Failed to initialize config parser");
        return false;
    }
    
    if (!initializeMappingManager()) {
        logError("Failed to initialize mapping manager");
        return false;
    }
    
    if (!initializeMappingsFromConfig()) {
        logError("Failed to initialize mappings from config");
        return false;
    }
    
    if (!initializeSceneManager()) {
        logError("Failed to initialize scene manager");
        return false;
    }
    
    if (!initializeEventHandler()) {
        logError("Failed to initialize event handler");
        return false;
    }
    
    // S'abonner aux événements
    subscribeToEvents();
    
    initialized_ = true;
    logInfo("ParameterViewController initialized successfully");
    return true;
}

void ParameterViewController::update() {
    if (!active_ || !initialized_) {
        return;
    }
    
    // Traiter les mises à jour différées des widgets via le gestionnaire de scène
    if (sceneManager_) {
        const auto& widgets = sceneManager_->getWidgets();
        for (const auto& widget : widgets) {
            if (widget) {
                widget->processPendingUpdates();
            }
        }
    }
}

void ParameterViewController::setActive(bool active) {
    if (active == active_) {
        return; // Pas de changement
    }
    
    active_ = active;
    
    if (active) {
        logDebug("Activating controller");
        
        // Activer la visibilité des widgets
        if (sceneManager_) {
            sceneManager_->setWidgetsVisible(true);
        }
        
        // Activer le gestionnaire d'événements
        if (eventHandler_) {
            eventHandler_->setActive(true);
        }
    } else {
        logDebug("Deactivating controller");
        
        // Désactiver le gestionnaire d'événements
        if (eventHandler_) {
            eventHandler_->setActive(false);
        }
        
        // Masquer les widgets
        if (sceneManager_) {
            sceneManager_->setWidgetsVisible(false);
        }
    }
}

bool ParameterViewController::isActive() const {
    return active_;
}

lv_obj_t* ParameterViewController::getMainScreen() const {
    if (sceneManager_) {
        return sceneManager_->getMainScreen();
    }
    return nullptr;
}

void ParameterViewController::setParameter(uint8_t cc_number, uint8_t channel, uint8_t value,
                                         const String& parameter_name, bool animate) {
    ParameterWidget* widget = getWidgetForCC(cc_number);
    if (widget) {
        widget->setParameter(cc_number, channel, value, parameter_name, animate);
        logDebug("Set parameter CC" + String(cc_number) + " = " + String(value));
    } else {
        logDebug("No widget found for CC" + String(cc_number));
    }
}

void ParameterViewController::setButtonState(uint16_t button_id, bool pressed, bool animate) {
    ParameterWidget* widget = getWidgetForButton(button_id);
    if (widget && widget->hasButtonIndicator()) {
        widget->setButtonState(pressed, animate);
        logDebug("Set button " + String(button_id) + " state: " + String(pressed ? "pressed" : "released"));
    } else {
        logDebug("No widget with button indicator found for button " + String(button_id));
    }
}

ParameterWidget* ParameterViewController::getWidgetForCC(uint8_t cc_number) {
    if (!mappingManager_ || !sceneManager_) {
        return nullptr;
    }
    
    int8_t index = mappingManager_->getWidgetIndexForCC(cc_number);
    if (index >= 0 && index < config_.maxWidgets) {
        return sceneManager_->getWidget(static_cast<uint8_t>(index));
    }
    return nullptr;
}

ParameterWidget* ParameterViewController::getWidgetForButton(uint16_t button_id) {
    if (!mappingManager_ || !sceneManager_) {
        return nullptr;
    }
    
    int8_t index = mappingManager_->getWidgetIndexForButton(button_id);
    if (index >= 0 && index < config_.maxWidgets) {
        return sceneManager_->getWidget(static_cast<uint8_t>(index));
    }
    return nullptr;
}

bool ParameterViewController::onEvent(const Event& event) {
    if (!active_ || !initialized_) {
        return false;
    }
    
    // Déléguer au gestionnaire d'événements en priorité
    if (eventHandler_ && eventHandler_->isActive()) {
        bool handled = eventHandler_->onEvent(event);
        if (handled) {
            return true;
        }
    }
    
    // Traitement direct pour compatibilité
    if (event.getType() == UIDisplayEvents::UIParameterUpdate) {
        const UIParameterUpdateEvent& uiEvent = static_cast<const UIParameterUpdateEvent&>(event);
        return handleUIParameterUpdateEvent(uiEvent);
    }
    
    if (event.getType() == EventTypes::HighPriorityButtonPress) {
        return handleButtonEvent(event);
    }
    
    return false;
}

void ParameterViewController::finalizePositioning() {
    if (sceneManager_) {
        sceneManager_->finalizePositioning();
        logDebug("Finalized positioning");
    }
}

void ParameterViewController::setWidgetsVisible(bool visible) {
    if (sceneManager_) {
        sceneManager_->setWidgetsVisible(visible);
        logDebug("Set widgets visible: " + String(visible ? "true" : "false"));
    }
}

bool ParameterViewController::isInitialized() const {
    return initialized_;
}

//=============================================================================
// Méthodes privées d'initialisation
//=============================================================================

bool ParameterViewController::initializeConfigParser() {
    ConfigurationMidiExtractor::ParserConfig parserConfig;
    parserConfig.enableLogging = config_.enableLogging;

    configParser_ = std::make_unique<ConfigurationMidiExtractor>(parserConfig);

    logDebug("Config parser initialized");
    return configParser_ != nullptr;
}

bool ParameterViewController::initializeMappingManager() {
    ParameterWidgetMappingManager::MappingConfig mappingConfig;
    mappingConfig.maxWidgets = config_.maxWidgets;
    mappingConfig.enableLogging = config_.enableLogging;
    mappingConfig.enableButtonMapping = true;

    mappingManager_ = std::make_unique<ParameterWidgetMappingManager>(mappingConfig);

    logDebug("Mapping manager initialized");
    return mappingManager_ != nullptr;
}

bool ParameterViewController::initializeMappingsFromConfig() {
    if (!configParser_ || !mappingManager_ || !unifiedConfig_) {
        return false;
    }
    
    // Extraire les données de configuration
    auto midiControls = configParser_->extractMidiControls(*unifiedConfig_);
    auto buttonInfos = configParser_->extractButtonInfo(*unifiedConfig_);
    
    // Initialiser tous les mappings
    mappingManager_->initializeMappings(midiControls, buttonInfos);
    
    logDebug("Mappings initialized from config (" + String(midiControls.size()) + " MIDI controls, " + 
             String(buttonInfos.size()) + " buttons)");
    return true;
}

bool ParameterViewController::initializeSceneManager() {
    if (!mappingManager_) {
        return false;
    }
    
    // Configuration du gestionnaire de scène
    ParameterSceneManager::SceneConfig sceneConfig;
    sceneConfig.maxWidgets = config_.maxWidgets;
    sceneConfig.screenWidth = config_.screenWidth;
    sceneConfig.screenHeight = config_.screenHeight;
    sceneConfig.gridCols = config_.gridCols;
    sceneConfig.gridRows = config_.gridRows;
    sceneConfig.widgetWidth = config_.widgetWidth;
    sceneConfig.widgetHeight = config_.widgetHeight;
    sceneConfig.arcRadius = config_.arcRadius;
    sceneConfig.buttonIndicatorSize = config_.buttonIndicatorSize;
    sceneConfig.backgroundColor = config_.backgroundColor;
    sceneConfig.enableLogging = config_.enableLogging;
    
    // Créer le gestionnaire avec les mappings partagés
    sceneManager_ = std::make_unique<ParameterSceneManager>(
        sceneConfig,
        std::shared_ptr<ParameterWidgetMappingManager>(mappingManager_.get(),
                                                       [](ParameterWidgetMappingManager*) {}));

    // Fonction d'accès à la configuration des widgets
    auto widgetConfigAccessor = [this](uint8_t index) -> ParameterSceneManager::WidgetConfig* {
        static std::array<ParameterSceneManager::WidgetConfig, 8> configs;

        if (!configParser_ || !unifiedConfig_) {
            return nullptr;
        }
        
        // Extraire les contrôles MIDI depuis la configuration
        auto midiControls = configParser_->extractMidiControls(*unifiedConfig_);
        
        if (index < midiControls.size()) {
            const auto& control = midiControls[index];
            configs[index].cc_number = control.cc_number;
            configs[index].channel = control.channel + 1;  // Convertir 0-based vers 1-based
            configs[index].value = 0;  // Valeur initiale
            configs[index].name = control.name;
            configs[index].visible = true;
            return &configs[index];
        } else if (index < config_.maxWidgets) {
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
        sceneManager_->finalizePositioning();
    }
    
    logDebug("Scene manager initialized: " + String(success ? "success" : "failed"));
    return success;
}

bool ParameterViewController::initializeEventHandler() {
    if (!mappingManager_ || !sceneManager_) {
        return false;
    }
    
    // Configuration du gestionnaire d'événements
    ParameterEventHandler::EventConfig eventConfig;
    eventConfig.enableMidiEvents = true;
    eventConfig.enableButtonEvents = true;
    eventConfig.enableLogging = config_.enableLogging;
    eventConfig.enableAnimation = config_.enableAnimation;
    
    // Fonction d'accès aux widgets via le gestionnaire de scène
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
        std::shared_ptr<ParameterWidgetMappingManager>(mappingManager_.get(),
                                                       [](ParameterWidgetMappingManager*) {}));

    logDebug("Event handler initialized");
    return eventHandler_ != nullptr;
}

//=============================================================================
// Gestion des événements
//=============================================================================

void ParameterViewController::subscribeToEvents() {
    if (!eventBus_) {
        return;
    }
    
    // S'abonner avec HAUTE PRIORITÉ pour recevoir les événements HighPriorityButtonPress
    event_subscription_id_ = eventBus_->subscribeHigh(this);
    logDebug("Subscribed to events with ID: " + String(event_subscription_id_));
}

void ParameterViewController::unsubscribeFromEvents() {
    if (event_subscription_id_ > 0 && eventBus_) {
        eventBus_->unsubscribe(event_subscription_id_);
        event_subscription_id_ = 0;
        logDebug("Unsubscribed from events");
    }
}

bool ParameterViewController::handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event) {
    if (!active_ || !initialized_) {
        return false;
    }
    
    ParameterWidget* widget = getWidgetForCC(event.controller);
    if (widget) {
        uint8_t channel = event.channel + 1;  // Convertir 0-15 vers 1-16
        String parameter_name = event.parameter_name.length() == 0 ? 
                              ("CC" + String(event.controller)) : event.parameter_name;
        
        widget->setParameter(event.controller,
                           channel,
                           event.value,
                           parameter_name,
                           config_.enableAnimation);
        return true;
    }
    
    return false;
}

bool ParameterViewController::handleButtonEvent(const Event& event) {
    if (!active_ || !initialized_) {
        return false;
    }
    
    if (event.getType() == EventTypes::HighPriorityButtonPress) {
        const auto& buttonEvent = static_cast<const HighPriorityButtonPressEvent&>(event);
        setButtonState(buttonEvent.buttonId, buttonEvent.pressed, config_.enableAnimation);
        return true;
    }
    
    return false;
}

//=============================================================================
// Utilitaires
//=============================================================================

void ParameterViewController::logInfo(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[ParameterViewController] " + message);
    }
}

void ParameterViewController::logDebug(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[ParameterViewController DEBUG] " + message);
    }
}

void ParameterViewController::logError(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[ParameterViewController ERROR] " + message);
    }
}