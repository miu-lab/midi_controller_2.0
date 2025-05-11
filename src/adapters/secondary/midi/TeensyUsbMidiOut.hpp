#pragma once
#include <Arduino.h>

#include "core/ports/output/MidiOutputPort.hpp"

/**
 * @brief Implémentation de MidiOutputPort pour le port USB MIDI natif de Teensy
 *
 * Cette classe utilise l'interface USB MIDI native de Teensy pour envoyer
 * des messages MIDI via USB sans avoir besoin d'une bibliothèque externe.
 */
class TeensyUsbMidiOut : public MidiOutputPort {
public:
    TeensyUsbMidiOut();

    void sendCc(MidiChannel ch, MidiCC cc, uint8_t value) override;
    void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) override;
    void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) override;
    
    // Nouvelles méthodes à implémenter pour satisfaire l'interface
    void sendProgramChange(MidiChannel ch, uint8_t program) override;
    void sendPitchBend(MidiChannel ch, uint16_t value) override;
    void sendChannelPressure(MidiChannel ch, uint8_t pressure) override;
    void sendSysEx(const uint8_t* data, uint16_t length) override;

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
