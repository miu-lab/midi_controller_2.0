#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"

#include <Arduino.h>

#include "adapters/secondary/midi/TeensyUsbMidi.hpp"

TeensyUsbMidiOut::TeensyUsbMidiOut() {
    // Initialiser le tableau de notes actives
    for (size_t i = 0; i < MAX_ACTIVE_NOTES; i++) {
        activeNotes_[i].active = false;
    }

    // S'assurer que l'USB MIDI est prêt
    // Rien à faire, Teensy gère cela automatiquement
}

void TeensyUsbMidiOut::sendControlChange(MidiChannel ch, MidiCC cc, uint8_t value) {
    // Envoyer via usbMIDI
    usbMIDI.sendControlChange(cc, value, ch + 1);  // Les canaux MIDI commencent à 1 pour Teensy
}

void TeensyUsbMidiOut::sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) {
    // Enregistrer cette note comme active
    markNoteActive(ch, note);

    // Envoyer via usbMIDI
    usbMIDI.sendNoteOn(note, velocity, ch + 1);  // Les canaux MIDI commencent à 1 pour Teensy
}

void TeensyUsbMidiOut::sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) {
    // Marquer cette note comme inactive
    markNoteInactive(ch, note);

    // Envoyer via usbMIDI
    usbMIDI.sendNoteOff(note, velocity, ch + 1);  // Les canaux MIDI commencent à 1 pour Teensy
}

// Nouvelles méthodes implémentées
void TeensyUsbMidiOut::sendProgramChange(MidiChannel ch, uint8_t program) {
    usbMIDI.sendProgramChange(program, ch + 1);
}

void TeensyUsbMidiOut::sendPitchBend(MidiChannel ch, uint16_t value) {
    usbMIDI.sendPitchBend(value, ch + 1);
}

void TeensyUsbMidiOut::sendChannelPressure(MidiChannel ch, uint8_t pressure) {
    usbMIDI.sendAfterTouch(pressure, ch + 1);
}

void TeensyUsbMidiOut::sendSysEx(const uint8_t* data, uint16_t length) {
    usbMIDI.sendSysEx(length, data);
}

void TeensyUsbMidiOut::flush() {
    // Traiter les messages USB
    usbMIDI.read();
}

void TeensyUsbMidiOut::markNoteActive(MidiChannel ch, MidiNote note) {
    // Rechercher un emplacement libre ou la même note
    for (size_t i = 0; i < MAX_ACTIVE_NOTES; i++) {
        if (!activeNotes_[i].active) {
            // Emplacement libre trouvé
            activeNotes_[i].channel = ch;
            activeNotes_[i].note = note;
            activeNotes_[i].active = true;
            return;
        } else if (activeNotes_[i].channel == ch && activeNotes_[i].note == note) {
            // Note déjà active, mettre à jour l'état
            activeNotes_[i].active = true;
            return;
        }
    }

    // Si nous arrivons ici, le tableau est plein, mais nous devons quand même enregistrer cette
    // note Dans ce cas, nous réutilisons le premier emplacement
    activeNotes_[0].channel = ch;
    activeNotes_[0].note = note;
    activeNotes_[0].active = true;
}

void TeensyUsbMidiOut::markNoteInactive(MidiChannel ch, MidiNote note) {
    // Rechercher cette note dans les notes actives
    for (size_t i = 0; i < MAX_ACTIVE_NOTES; i++) {
        if (activeNotes_[i].active && activeNotes_[i].channel == ch &&
            activeNotes_[i].note == note) {
            // Note trouvée, marquer comme inactive
            activeNotes_[i].active = false;
            return;
        }
    }

    // Note non trouvée, rien à faire
}