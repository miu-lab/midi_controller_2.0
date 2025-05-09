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
    
    // Libérer les écouteurs (peuvent être nullptr si non initialisés)
    delete simpleListener_;
    delete eventListener_;
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
    
    // Désabonner et libérer l'écouteur simple
    if (simpleListener_ != nullptr) {
        if (simpleListenerSubId_ != 0) {
            eventBus.unsubscribe(simpleListenerSubId_);
            simpleListenerSubId_ = 0;
        }
        delete simpleListener_;
        simpleListener_ = nullptr;
    }
    
    // Désabonner et libérer l'écouteur existant avec navigation
    if (eventListener_ != nullptr) {
        if (eventListenerSubId_ != 0) {
            eventBus.unsubscribe(eventListenerSubId_);
            eventListenerSubId_ = 0;
        }
        delete eventListener_;
        eventListener_ = nullptr;
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
    eventListener_ = new MidiSystemEventListener(*this, navService);
    
    // S'abonner au bus d'événements avec l'objet et enregistrer l'ID
    eventListenerSubId_ = eventBus.subscribe(eventListener_);
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
    if (simpleListener_ != nullptr) {
        auto& eventBus = EventBus::getInstance();
        if (simpleListenerSubId_ != 0) {
            eventBus.unsubscribe(simpleListenerSubId_);
        }
        delete simpleListener_;
    }
    
    // Créer une instance statique de l'écouteur
    simpleListener_ = new MidiSystemSimpleListener(*this);
    
    // S'abonner au bus d'événements et enregistrer l'ID d'abonnement
    auto& eventBus = EventBus::getInstance();
    simpleListenerSubId_ = eventBus.subscribe(simpleListener_);
}