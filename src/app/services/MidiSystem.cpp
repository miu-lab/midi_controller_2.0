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
      profileManager_(profileManager),
      simpleListener_(nullptr),
      eventListener_(nullptr),
      simpleListenerSubId_(0),
      eventListenerSubId_(0) {
    // Note: InputRouter n'est plus utilisé directement, on utilise MidiMapper à la place
}

MidiSystem::~MidiSystem() {
    // Se désabonner du bus d'événements
    auto& eventBus = EventBus::getInstance();
    
    if (simpleListenerSubId_ != 0) {
        eventBus.unsubscribe(simpleListenerSubId_);
    }
    
    if (eventListenerSubId_ != 0) {
        eventBus.unsubscribe(eventListenerSubId_);
    }
    
    // Les écouteurs seront automatiquement libérés par les destructeurs des smart pointers
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
    
    // D'abord se désabonner de tout écouteur existant, y compris le simpleListener
    auto& eventBus = EventBus::getInstance();
    
    // Désabonner l'écouteur simple et le réinitialiser (à nullptr)
    if (simpleListener_) {
        if (simpleListenerSubId_ != 0) {
            eventBus.unsubscribe(simpleListenerSubId_);
            simpleListenerSubId_ = 0;
        }
        simpleListener_.reset();
    }
    
    // Désabonner l'écouteur existant avec navigation et le réinitialiser
    if (eventListener_) {
        if (eventListenerSubId_ != 0) {
            eventBus.unsubscribe(eventListenerSubId_);
            eventListenerSubId_ = 0;
        }
        eventListener_.reset();
    }
    
    // Initialiser les mappings comme dans init() mais sans créer un simple listener
    for (const auto& mapping : profileManager_.getAllMappings()) {
        bool isRelative = mapping.midiControl.relative;

        std::unique_ptr<IMidiMappingStrategy> strategy;
        if (isRelative) {
            strategy = MidiMappingFactory::createRelative();
        } else {
            strategy = MidiMappingFactory::createAbsolute(0, 127);
        }

        midiMapper_.setMapping(mapping.controlId, mapping.midiControl, std::move(strategy));
    }
    
    // Créer uniquement l'écouteur avec navigation
    eventListener_ = std::make_unique<MidiSystemEventListener>(*this, navService);
    
    // S'abonner au bus d'événements avec l'objet et enregistrer l'ID
    eventListenerSubId_ = eventBus.subscribe(eventListener_.get());
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
    
    // Libérer l'écouteur existant s'il y en a un
    if (simpleListener_) {
        auto& eventBus = EventBus::getInstance();
        if (simpleListenerSubId_ != 0) {
            eventBus.unsubscribe(simpleListenerSubId_);
        }
        simpleListener_.reset();
    }
    
    // Créer une instance de l'écouteur
    simpleListener_ = std::make_unique<MidiSystemSimpleListener>(*this);
    
    // S'abonner au bus d'événements et enregistrer l'ID d'abonnement
    auto& eventBus = EventBus::getInstance();
    simpleListenerSubId_ = eventBus.subscribe(simpleListener_.get());
}