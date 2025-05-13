#include "adapters/secondary/midi/MidiInHandler.hpp"

#include <Arduino.h>

#include "adapters/secondary/midi/TeensyUsbMidi.hpp"

MidiInHandler::MidiInHandler() {
    // Rien à initialiser
}

void MidiInHandler::onControlChange(CcCallback callback) {
    ccCallbacks_.push_back(callback);
}

void MidiInHandler::onNoteOn(NoteCallback callback) {
    noteOnCallbacks_.push_back(callback);
}

void MidiInHandler::onNoteOff(NoteCallback callback) {
    noteOffCallbacks_.push_back(callback);
}

void MidiInHandler::update() {
    // Vérifier les messages MIDI entrants
    if (usbMIDI.read()) {
        // Type de message
        uint8_t type = usbMIDI.getType();
        // Canal MIDI (0-15)
        uint8_t channel =
            usbMIDI.getChannel() - 1;  // Lib Teensy utilise canaux 1-16, nous utilisons 0-15

        switch (type) {
        case usbMIDI.ControlChange:
            handleControlChange(channel, usbMIDI.getData1(), usbMIDI.getData2());
            break;

        case usbMIDI.NoteOn:
            if (usbMIDI.getData2() == 0) {
                // NoteOn avec vélocité 0 est traité comme NoteOff
                handleNoteOff(channel, usbMIDI.getData1(), 0);
            } else {
                handleNoteOn(channel, usbMIDI.getData1(), usbMIDI.getData2());
            }
            break;

        case usbMIDI.NoteOff:
            handleNoteOff(channel, usbMIDI.getData1(), usbMIDI.getData2());
            break;

        default:
            // Ignorer les autres types de messages
            break;
        }
    }
}

void MidiInHandler::handleControlChange(MidiChannel channel, MidiCC cc, uint8_t value) {
    for (auto& callback : ccCallbacks_) {
        callback(channel, cc, value);
    }
}

void MidiInHandler::handleNoteOn(MidiChannel channel, MidiNote note, uint8_t velocity) {
    for (auto& callback : noteOnCallbacks_) {
        callback(channel, note, velocity);
    }
}

void MidiInHandler::handleNoteOff(MidiChannel channel, MidiNote note, uint8_t velocity) {
    for (auto& callback : noteOffCallbacks_) {
        callback(channel, note, velocity);
    }
}