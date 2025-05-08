#include "app/services/MidiSystem.hpp"

#include "app/services/ServiceLocator.hpp"
#include "core/domain/events/EventSystem.hpp"
#include "core/domain/strategies/MidiMappingFactory.hpp"
#include "app/services/MidiSystemListener.hpp"

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
    auto& eventBus = EventBus::getInstance();
    
    // S'abonner au bus d'événements
    eventBus.subscribe(new MidiSystemEventListener(*this, navService));
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
    auto& eventBus = EventBus::getInstance();
    
    // S'abonner au bus d'événements
    eventBus.subscribe(new MidiSystemSimpleListener(*this));
}
