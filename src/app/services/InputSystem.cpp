#include "app/services/InputSystem.hpp"

#include "app/services/ServiceLocator.hpp"

InputSystem::InputSystem()
    : encoderManager_({}),
      processEncoders_(encoderManager_.getEncoders()),
      buttonManager_({}),
      processButtons_(buttonManager_.getButtons()),
      inputController_(nullptr) {
#ifndef DISABLE_CONTROLLERS
    // Créer et initialiser l'InputController
    inputController_ = new InputController(ServiceLocator::getNavigationConfigService());

    // Enregistrer le contrôleur dans le ServiceLocator
    ServiceLocator::registerInputController(inputController_);

    // Configurer les processeurs pour utiliser l'InputController
    processEncoders_.setInputController(inputController_);
    processButtons_.setInputController(inputController_);
#endif
}

void InputSystem::init() {
    // Initialiser l'état initial des encodeurs et boutons
    encoderManager_.updateAll();
    buttonManager_.updateAll();
    processButtons_.initStates();  // Initialiser les états sans déclencher d'événements

#ifndef DISABLE_CONTROLLERS
    // Configurer le contrôleur d'entrée avec les callbacks par défaut
    // Ces callbacks utilisent l'EventBus traditionnel pour maintenir la compatibilité
    inputController_->setNavigationEncoderCallback(
        [](EncoderId id, int32_t absPos, int8_t relChange) {
            // Publier l'événement pour les encodeurs de navigation
            EventBus<EventTypes::EncoderTurned>::publish({.id = id, .absolutePosition = absPos});
        });

    inputController_->setMidiEncoderCallback([](EncoderId id, int32_t absPos, int8_t relChange) {
        // Publier l'événement pour les encodeurs MIDI
        // MidiMapper se chargera de gérer les cas où absPos est hors limites (0-127)
        // et de détecter les changements de direction
        EventBus<EventTypes::EncoderTurned>::publish({.id = id, .absolutePosition = absPos});
    });

    // Callbacks pour les boutons d'encodeurs (navigation et MIDI)
    auto encoderButtonCallback = [](EncoderId id, bool pressed) {
        EventBus<EventTypes::EncoderButton>::publish({id, pressed});
    };
    inputController_->setNavigationEncoderButtonCallback(encoderButtonCallback);
    inputController_->setMidiEncoderButtonCallback(encoderButtonCallback);

    // Callbacks pour les boutons (navigation et MIDI)
    auto buttonCallback = [](ButtonId id, bool pressed) {
        if (pressed) {
            EventBus<EventTypes::ButtonPressed>::publish({id});
        } else {
            EventBus<EventTypes::ButtonReleased>::publish({id});
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
    processEncoders_.setInputController(inputController_);
    processButtons_.setInputController(inputController_);
#endif

    // Initialiser l'état initial
    init();
}

InputController& InputSystem::getInputController() {
#ifndef DISABLE_CONTROLLERS
    return *inputController_;
#else
    // Ceci ne devrait jamais être appelé en mode DISABLE_CONTROLLERS
    static InputController* nullController = nullptr;
    return *nullController; // Provoquerait un crash si appelé
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
