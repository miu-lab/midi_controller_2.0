#include "app/services/MidiSystem.hpp"

#include "app/services/ServiceLocator.hpp"
#include "core/domain/events/EventTypes.hpp"
#include "core/domain/strategies/MidiMappingFactory.hpp"

MidiSystem::MidiSystem(ProfileManager& profileManager)
    : rawMidiOut_(),
      bufferedMidiOut_(rawMidiOut_),
      midiInHandler_(),
      commandManager_(),
      midiMapper_(bufferedMidiOut_, commandManager_),
      profileManager_(profileManager) {
    // Note: InputRouter n'est plus utilisé directement, on utilise MidiMapper à la place
}

void MidiSystem::init() {
    Serial.println("MidiSystem::init() called");
    initSubscriptions();

    // Initialiser les mappings à partir du ProfileManager
    for (const auto& mapping : profileManager_.getAllMappings()) {
        // Par défaut, utiliser une stratégie absolue pour les encodeurs
        // et une stratégie relative pour les boutons
        bool isRelative = mapping.midiControl.relative;

        std::unique_ptr<IMidiMappingStrategy> strategy;
        if (isRelative) {
            strategy = MidiMappingFactory::createRelative();
        } else {
            strategy = MidiMappingFactory::createAbsolute(0, 127);
        }

        midiMapper_.setMapping(mapping.controlId, mapping.midiControl, std::move(strategy));
    }
}

void MidiSystem::init(NavigationConfigService& navService) {
    Serial.println("MidiSystem::init(NavigationConfigService&) called");
    init();  // Appel à l'initialisation de base

    // S'abonner aux événements d'entrée
    auto& encoderEventBus = ServiceLocator::getEncoderTurnedEventBus();
    auto& encoderButtonEventBus = ServiceLocator::getEncoderButtonEventBus();
    auto& buttonPressedEventBus = ServiceLocator::getButtonPressedEventBus();
    auto& buttonReleasedEventBus = ServiceLocator::getButtonReleasedEventBus();

    // S'abonner aux événements d'encodeur
    encoderEventBus.subscribe([this, &navService](const EventTypes::EncoderTurned& event) {
        // Débogage
        Serial.print("MIDISYSTEM: EncoderTurned event received for encoder ");
        Serial.print(event.id);
        Serial.print(" pos:");
        Serial.println(event.absolutePosition);

        // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
        if (!navService.isNavigationControl(event.id)) {
            Serial.println("Calling MidiMapper::processEncoderChange");
            midiMapper_.processEncoderChange(event.id, event.absolutePosition);
        } else {
            Serial.println("Skipping navigation control");
        }
    });

    // S'abonner aux événements de bouton d'encodeur
    encoderButtonEventBus.subscribe([this, &navService](const EventTypes::EncoderButton& event) {
        // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
        if (!navService.isNavigationControl(event.id)) {
            midiMapper_.processEncoderButton(event.id, event.pressed);
        }
    });

    // S'abonner aux événements de bouton pressé
    buttonPressedEventBus.subscribe([this, &navService](const EventTypes::ButtonPressed& event) {
        // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
        if (!navService.isNavigationControl(event.id)) {
            midiMapper_.processButtonPress(event.id, true);
        }
    });

    // S'abonner aux événements de bouton relâché
    buttonReleasedEventBus.subscribe([this, &navService](const EventTypes::ButtonReleased& event) {
        // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
        if (!navService.isNavigationControl(event.id)) {
            midiMapper_.processButtonPress(event.id, false);
        }
    });
}

void MidiSystem::update() {
    // Traiter les messages MIDI entrants
    midiInHandler_.update();

    // Mettre à jour le mapper MIDI (pour les commandes temporisées)
    midiMapper_.update();

    // Envoyer les messages en attente dans le buffer
    bufferedMidiOut_.update();
}

IMidiOut& MidiSystem::getMidiOut() {
    return bufferedMidiOut_;
}

CommandManager& MidiSystem::getCommandManager() {
    return commandManager_;
}

MidiMapper& MidiSystem::getMidiMapper() {
    return midiMapper_;
}

void MidiSystem::initSubscriptions() {
    Serial.println("MidiSystem::initSubscriptions() called");
    // S'abonner aux événements d'entrée pour le routage MIDI
    auto& encoderEventBus = ServiceLocator::getEncoderTurnedEventBus();
    auto& encoderButtonEventBus = ServiceLocator::getEncoderButtonEventBus();
    auto& buttonPressedEventBus = ServiceLocator::getButtonPressedEventBus();
    auto& buttonReleasedEventBus = ServiceLocator::getButtonReleasedEventBus();

    // S'abonner à tous les événements d'entrée
    encoderEventBus.subscribe([this](const EventTypes::EncoderTurned& event) {
        Serial.print("MIDISYSTEM (initSubscriptions): EncoderTurned event for encoder ");
        Serial.print(event.id);
        Serial.print(" pos:");
        Serial.println(event.absolutePosition);
        midiMapper_.processEncoderChange(event.id, event.absolutePosition);
    });

    encoderButtonEventBus.subscribe([this](const EventTypes::EncoderButton& event) {
        midiMapper_.processEncoderButton(event.id, event.pressed);
    });

    buttonPressedEventBus.subscribe([this](const EventTypes::ButtonPressed& event) {
        midiMapper_.processButtonPress(event.id, true);
    });

    buttonReleasedEventBus.subscribe([this](const EventTypes::ButtonReleased& event) {
        midiMapper_.processButtonPress(event.id, false);
    });
}
