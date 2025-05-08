#pragma once
#include <cstdint> // Pour uint8_t

/**
 * @brief Inclusions et définitions unifiées pour l'interface USB MIDI de Teensy
 *
 * Ce fichier centralise l'inclusion des bibliothèques MIDI et définit
 * automatiquement les stubs nécessaires pour les plateformes non-Teensy.
 */

// Détection des plateformes Teensy
#if defined(CORE_TEENSY)

// Inclure la bibliothèque USB MIDI de Teensy
#include <usb_midi.h>

// La variable usbMIDI est déjà définie par la bibliothèque

#else // Pour toutes les autres plateformes

#warning "La bibliothèque TeensyUsbMidi requiert une carte Teensy. Utilisation d'un stub."

/**
 * @brief Stub pour remplacer l'objet usbMIDI sur les plateformes non-Teensy
 */
class TeensyUsbMidiStub
{
public:
    // Méthodes de base
    bool read() { return false; }
    void sendControlChange(uint8_t cc, uint8_t value, uint8_t channel) {}
    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {}
    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {}

    // Méthodes pour la récupération des données
    uint8_t getType() { return 0; }
    uint8_t getChannel() { return 0; }
    uint8_t getData1() { return 0; }
    uint8_t getData2() { return 0; }

    // Constantes pour les types de messages MIDI
    static const uint8_t ControlChange = 0xB0;
    static const uint8_t NoteOn = 0x90;
    static const uint8_t NoteOff = 0x80;
};

// Définir une variable globale qui sera utilisée comme usbMIDI
extern TeensyUsbMidiStub usbMIDI;

#endif // CORE_TEENSY
