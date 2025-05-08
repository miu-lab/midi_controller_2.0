#pragma once
#include <Arduino.h>

#include "core/domain/IMidiOut.hpp"

/**
 * @brief Implémentation de IMidiOut pour le port USB MIDI natif de Teensy
 *
 * Cette classe utilise l'interface USB MIDI native de Teensy pour envoyer
 * des messages MIDI via USB sans avoir besoin d'une bibliothèque externe.
 */
class TeensyUsbMidiOut : public IMidiOut {
public:
    TeensyUsbMidiOut();

    void sendCc(MidiChannel ch, MidiCC cc, uint8_t value) override;
    void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) override;
    void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) override;

    // Méthode additionelle pour vider le buffer et envoyer réellement les messages
    void flush();

private:
    // Buffer pour garder la trace des notes actives et éviter les notes bloquées
    static constexpr size_t MAX_ACTIVE_NOTES = 16;

    struct ActiveNote {
        MidiChannel channel;
        MidiNote note;
        bool active;
    };

    ActiveNote activeNotes_[MAX_ACTIVE_NOTES];

    // Marque une note comme active
    void markNoteActive(MidiChannel ch, MidiNote note);

    // Marque une note comme inactive
    void markNoteInactive(MidiChannel ch, MidiNote note);
};
