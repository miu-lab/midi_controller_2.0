#include "interface_adapters/midi/MidiInHandler.hpp"
#include "interface_adapters/midi/TeensyUsbMidi.hpp"
#include <Arduino.h>

MidiInHandler::MidiInHandler() {
    // Rien à initialiser
}

void MidiInHandler::onCcReceived(CcCallback callback) {
    ccCallbacks_.push_back(callback);
}

void MidiInHandler::onNoteOnReceived(NoteCallback callback) {
    noteOnCallbacks_.push_back(callback);
}

void MidiInHandler::onNoteOffReceived(NoteCallback callback) {
    noteOffCallbacks_.push_back(callback);
}

void MidiInHandler::update() {
    // Vérifier les messages MIDI entrants
    if (usbMIDI.read()) {
        // Type de message
        uint8_t type = usbMIDI.getType();
        // Canal MIDI (0-15)
        uint8_t channel = usbMIDI.getChannel() - 1; // Lib Teensy utilise canaux 1-16, nous utilisons 0-15
        
        switch (type) {
            case usbMIDI.ControlChange:
                handleCcMessage(channel, usbMIDI.getData1(), usbMIDI.getData2());
                break;
                
            case usbMIDI.NoteOn:
                if (usbMIDI.getData2() == 0) {
                    // NoteOn avec vélocité 0 est traité comme NoteOff
                    handleNoteOffMessage(channel, usbMIDI.getData1(), 0);
                } else {
                    handleNoteOnMessage(channel, usbMIDI.getData1(), usbMIDI.getData2());
                }
                break;
                
            case usbMIDI.NoteOff:
                handleNoteOffMessage(channel, usbMIDI.getData1(), usbMIDI.getData2());
                break;
                
            default:
                // Ignorer les autres types de messages
                break;
        }
    }
}

void MidiInHandler::handleCcMessage(MidiChannel channel, MidiCC cc, uint8_t value) {
    for (auto& callback : ccCallbacks_) {
        callback(channel, cc, value);
    }
}

void MidiInHandler::handleNoteOnMessage(MidiChannel channel, MidiNote note, uint8_t velocity) {
    for (auto& callback : noteOnCallbacks_) {
        callback(channel, note, velocity);
    }
}

void MidiInHandler::handleNoteOffMessage(MidiChannel channel, MidiNote note, uint8_t velocity) {
    for (auto& callback : noteOffCallbacks_) {
        callback(channel, note, velocity);
    }
}