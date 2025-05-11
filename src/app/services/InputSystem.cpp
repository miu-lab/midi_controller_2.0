#include "app/services/InputSystem.hpp"

#include "app/services/ServiceLocator.hpp"

InputSystem::InputSystem()
    : encoderManager_({}),
      processEncoders_(encoderManager_.getEncoders()),
      buttonManager_({}),
      processButtons_(buttonManager_.getButtons()),
      inputController_(nullptr) {
#ifndef DISABLE_CONTROLLERS
    // Créer et initialiser l'InputController avec un shared pointer
    inputController_ = std::make_shared<InputController>(ServiceLocator::getNavigationConfigService());

    // Enregistrer le contrôleur dans le ServiceLocator
    ServiceLocator::registerInputController(inputController_);

    // Configurer les processeurs pour utiliser l'InputController
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
    static InputController nullController(ServiceLocator::getNavigationConfigService());
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
