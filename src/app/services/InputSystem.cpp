#include "app/services/InputSystem.hpp"

#include "app/services/ServiceLocator.hpp"

InputSystem::InputSystem()
    : encoderManager_({}),
      processEncoders_(encoderManager_.getEncoders()),
      buttonManager_({}),
      processButtons_(buttonManager_.getButtons()),
      inputController_(nullptr) {
    // Créer et initialiser l'InputController
    inputController_ = new InputController(ServiceLocator::getNavigationConfigService());

    // Enregistrer le contrôleur dans le ServiceLocator
    ServiceLocator::registerInputController(inputController_);

    // Configurer les processeurs pour utiliser l'InputController
    processEncoders_.setInputController(inputController_);
    processButtons_.setInputController(inputController_);
}

void InputSystem::init() {
    // Initialiser l'état initial des encodeurs et boutons
    encoderManager_.updateAll();
    buttonManager_.updateAll();
    processButtons_.initStates();  // Initialiser les états sans déclencher d'événements

    // Configurer le contrôleur d'entrée avec les callbacks par défaut
    // Ces callbacks utilisent l'EventBus traditionnel pour maintenir la compatibilité
    inputController_->setNavigationEncoderCallback(
        [](EncoderId id, int32_t absPos, int8_t relChange) {
            Serial.print("NAVIGATION_ENCODER: ");
            Serial.print(id);
            Serial.print(" pos:");
            Serial.println(absPos);
            EventBus<EventTypes::EncoderTurned>::publish({.id = id, .absolutePosition = absPos});
        });

    inputController_->setMidiEncoderCallback([](EncoderId id, int32_t absPos, int8_t relChange) {
        Serial.print("MIDI_ENC ");
        Serial.print(id);
        Serial.print(" abs:");
        Serial.println(absPos);

        // Publier l'événement même si absPos est hors limites (0-127)
        // MidiMapper se chargera de gérer ces cas et de détecter les changements de direction
        EventBus<EventTypes::EncoderTurned>::publish({.id = id, .absolutePosition = absPos});
    });

    inputController_->setNavigationEncoderButtonCallback([](EncoderId id, bool pressed) {
        EventBus<EventTypes::EncoderButton>::publish({id, pressed});
    });

    inputController_->setMidiEncoderButtonCallback([](EncoderId id, bool pressed) {
        EventBus<EventTypes::EncoderButton>::publish({id, pressed});
    });

    inputController_->setNavigationButtonCallback([](ButtonId id, bool pressed) {
        if (pressed)
            EventBus<EventTypes::ButtonPressed>::publish({id});
        else
            EventBus<EventTypes::ButtonReleased>::publish({id});
    });

    inputController_->setMidiButtonCallback([](ButtonId id, bool pressed) {
        if (pressed)
            EventBus<EventTypes::ButtonPressed>::publish({id});
        else
            EventBus<EventTypes::ButtonReleased>::publish({id});
    });
}

void InputSystem::init(const std::vector<EncoderConfig>& encoderConfigs,
                       const std::vector<ButtonConfig>& buttonConfigs) {
    // Reconfigurer les gestionnaires
    encoderManager_ = EncoderManager(encoderConfigs);
    processEncoders_ = ProcessEncoders(encoderManager_.getEncoders());
    buttonManager_ = DigitalButtonManager(buttonConfigs);
    processButtons_ = ProcessButtons(buttonManager_.getButtons());

    // Reconfigurer les processeurs pour utiliser l'InputController
    processEncoders_.setInputController(inputController_);
    processButtons_.setInputController(inputController_);

    // Initialiser l'état initial
    init();
}

InputController& InputSystem::getInputController() {
    return *inputController_;
}

void InputSystem::update() {
    // Lecture et publication des mouvements d'encodeurs
    encoderManager_.updateAll();
    processEncoders_.update();

    // Lecture et publication des états de boutons
    buttonManager_.updateAll();
    processButtons_.update();
}
