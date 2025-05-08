// app/ControllerApp.hpp
#pragma once

#include <Arduino.h>
#include <vector>

#include "interface_adapters/encoders/EncoderManager.hpp"
#include "use_cases/ProcessEncoders.hpp"
#include "interface_adapters/buttons/DigitalButtonManager.hpp"
#include "use_cases/ProcessButtons.hpp"
#include "interface_adapters/midi/TeensyUsbMidiOut.hpp"
#include "interface_adapters/midi/BufferedMidiOut.hpp"
#include "interface_adapters/midi/MidiInHandler.hpp"
#include "storage/ProfileManager.hpp"
#include "input/InputRouter.hpp"

// Config externes
#include "config/ControlEncodersConfig.hpp"
#include "config/ControlButtonsConfig.hpp"


/**
 * @brief Application principale du contrôleur MIDI
 */
class ControllerApp {
public:
    ControllerApp();

    /// alias pour setup()
    void init()   { begin(); }
    /// alias pour loop()
    void update() { tick(); }

    void begin();
    void tick();

private:
    // Gestionnaires des contrôles physiques
    EncoderManager       encoderManager_;
    ProcessEncoders      processEncoders_;
    DigitalButtonManager buttonManager_;
    ProcessButtons       processButtons_;
    
    // Composants MIDI
    TeensyUsbMidiOut    rawMidiOut_;     // Sortie MIDI USB native
    BufferedMidiOut     bufferedMidiOut_; // Sortie MIDI avec buffer
    MidiInHandler       midiInHandler_;   // Gestion des messages MIDI entrants
    
    // Gestion des profils et routage
    ProfileManager      profileManager_;  // Stockage des mappings MIDI
    InputRouter         inputRouter_;     // Routage des événements vers MIDI
};
