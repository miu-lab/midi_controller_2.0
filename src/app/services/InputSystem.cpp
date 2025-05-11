#include "app/services/InputSystem.hpp"

#include "app/di/DependencyContainer.hpp"
#include "app/di/ServiceLocatorAdapter.hpp"
#include "app/services/NavigationConfigService.hpp"

InputSystem::InputSystem()
    : encoderManager_({}),
      processEncoders_(encoderManager_.getEncoders()),
      buttonManager_({}),
      processButtons_(buttonManager_.getButtons()),
      inputController_(nullptr),
      container_(nullptr),
      usingContainer_(false) {
      
    initializeDependencies();
}

InputSystem::InputSystem(std::shared_ptr<DependencyContainer> container)
    : encoderManager_({}),
      processEncoders_(encoderManager_.getEncoders()),
      buttonManager_({}),
      processButtons_(buttonManager_.getButtons()),
      inputController_(nullptr),
      container_(container),
      usingContainer_(true) {
      
    initializeDependencies();
}

void InputSystem::initializeDependencies() {
#ifndef DISABLE_CONTROLLERS
    // Récupérer NavigationConfigService
    std::shared_ptr<NavigationConfigService> navConfigService;
    
    if (usingContainer_ && container_) {
        // Essayer de récupérer depuis le container
        navConfigService = container_->resolve<NavigationConfigService>();
        
        // Si le service n'est pas dans le container, créer une nouvelle instance
        if (!navConfigService) {
            auto& legacyService = ServiceLocatorAdapter::getNavigationConfigServiceStatic();
            // Création d'un shared_ptr avec un deleter personnalisé
            navConfigService = std::shared_ptr<NavigationConfigService>(&legacyService, [](NavigationConfigService*){});
        }
    } else {
        // Ancien comportement - utiliser ServiceLocatorAdapter
        auto& legacyService = ServiceLocatorAdapter::getNavigationConfigServiceStatic();
        navConfigService = std::shared_ptr<NavigationConfigService>(&legacyService, [](NavigationConfigService*){});
    }
    
    // Créer InputController
    inputController_ = std::make_shared<InputController>(*navConfigService);

    if (usingContainer_ && container_) {
        // Créer une instance de ServiceLocatorAdapter avec le container
        auto adapter = std::make_shared<ServiceLocatorAdapter>(container_);
        ServiceLocatorAdapter::setDefaultInstance(adapter);
        adapter->registerInputController(inputController_);
        
        // Enregistrer également dans le container
        container_->registerDependency<InputController>(inputController_);
    } else {
        // Si nous n'avons pas de container, essayer d'utiliser l'instance par défaut
        auto defaultAdapter = ServiceLocatorAdapter::getDefaultInstance();
        if (defaultAdapter) {
            defaultAdapter->registerInputController(inputController_);
        }
    }

    // Configurer les processeurs
    processEncoders_.setInputController(inputController_.get());
    processButtons_.setInputController(inputController_.get());
#endif
}

void InputSystem::init() {
    // Initialiser l'état initial des encodeurs et boutons
    encoderManager_.updateAll();
    buttonManager_.updateAll();
    processButtons_.initStates();  // Initialiser les états sans déclencher d'événements

    // Aucun adaptateur de compatibilité nécessaire avec le nouveau système
    // LegacyEventAdapter::initialize();

#ifndef DISABLE_CONTROLLERS
    // Configurer le contrôleur d'entrée avec les callbacks par défaut
    // Ces callbacks utilisent le nouveau système d'événements
    inputController_->setNavigationEncoderCallback(
        [](EncoderId id, int32_t absPos, int8_t relChange) {
            // Publier l'événement pour les encodeurs de navigation dans le nouveau système
            EncoderTurnedEvent event(id, absPos, relChange);
            EventBus::getInstance().publish(event);
        });

    inputController_->setMidiEncoderCallback([](EncoderId id, int32_t absPos, int8_t relChange) {
        // Publier l'événement pour les encodeurs MIDI dans le nouveau système
        // MidiMapper se chargera de gérer les cas où absPos est hors limites (0-127)
        // et de détecter les changements de direction
        EncoderTurnedEvent event(id, absPos, relChange);
        EventBus::getInstance().publish(event);
    });

    // Callbacks pour les boutons d'encodeurs (navigation et MIDI)
    auto encoderButtonCallback = [](EncoderId id, bool pressed) {
        EncoderButtonEvent event(id, pressed);
        EventBus::getInstance().publish(event);
    };
    inputController_->setNavigationEncoderButtonCallback(encoderButtonCallback);
    inputController_->setMidiEncoderButtonCallback(encoderButtonCallback);

    // Callbacks pour les boutons (navigation et MIDI)
    auto buttonCallback = [](ButtonId id, bool pressed) {
        if (pressed) {
            ButtonPressedEvent event(id);
            EventBus::getInstance().publish(event);
        } else {
            ButtonReleasedEvent event(id);
            EventBus::getInstance().publish(event);
        }
    };
    inputController_->setNavigationButtonCallback(buttonCallback);
    inputController_->setMidiButtonCallback(buttonCallback);
#endif
}

void InputSystem::init(const std::vector<EncoderConfig>& encoderConfigs,
                       const std::vector<ButtonConfig>& buttonConfigs) {
    // Reconfigurer les gestionnaires
    encoderManager_ = EncoderManager(encoderConfigs);
    processEncoders_ = ProcessEncoders(encoderManager_.getEncoders());
    buttonManager_ = DigitalButtonManager(buttonConfigs);
    processButtons_ = ProcessButtons(buttonManager_.getButtons());

#ifndef DISABLE_CONTROLLERS
    // Reconfigurer les processeurs pour utiliser l'InputController
    processEncoders_.setInputController(inputController_.get());
    processButtons_.setInputController(inputController_.get());
#endif

    // Initialiser l'état initial
    init();
}

InputController& InputSystem::getInputController() {
#ifndef DISABLE_CONTROLLERS
    return *inputController_;
#else
    // Ceci ne devrait jamais être appelé en mode DISABLE_CONTROLLERS
    if (usingContainer_ && container_) {
        auto navConfigService = container_->resolve<NavigationConfigService>();
        if (navConfigService) {
            static InputController nullController(*navConfigService);
            return nullController;
        }
    }
    
    // Fallback vers ServiceLocatorAdapter
    // Désactiver l'avertissement de dépréciation - c'est un fallback temporaire
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    static InputController nullController(ServiceLocatorAdapter::getNavigationConfigServiceStatic());
    #pragma GCC diagnostic pop
    return nullController; // Contrôleur par défaut en cas de besoin
#endif
}

void InputSystem::update() {
    // Lecture et publication des mouvements d'encodeurs
    encoderManager_.updateAll();
    processEncoders_.update();

    // Lecture et publication des états de boutons
    buttonManager_.updateAll();
    processButtons_.update();
}
