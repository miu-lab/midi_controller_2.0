#include "app/MidiSystem.hpp"

MidiSystem::MidiSystem(ProfileManager& profileManager)
    : rawMidiOut_()
    , bufferedMidiOut_(rawMidiOut_)
    , midiInHandler_()
    , inputRouter_(bufferedMidiOut_, profileManager)
    , profileManager_(profileManager)
{
}

void MidiSystem::init() {
    // Initialisation sans service de navigation
    inputRouter_.init();
}

void MidiSystem::init(NavigationConfigService& navService) {
    // Initialisation avec service de navigation
    inputRouter_.init(navService);
}

void MidiSystem::update() {
    // Lire les messages MIDI entrants
    midiInHandler_.update();
    
    // Envoyer les messages MIDI en attente
    bufferedMidiOut_.flush();
    rawMidiOut_.flush();  // Traiter les messages USB
}

IMidiOut& MidiSystem::getMidiOut() {
    return bufferedMidiOut_;
}
