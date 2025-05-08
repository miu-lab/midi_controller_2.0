#pragma once
#include <cstdint>  // Pour uint8_t

/**
 * @brief Classe de substitution pour usbMIDI pour la compilation sur des plateformes non-Teensy
 */
class DummyUsbMidi {
public:
    bool read() {
        return false;
    }

    void sendControlChange(uint8_t, uint8_t, uint8_t) {}

    void sendNoteOn(uint8_t, uint8_t, uint8_t) {}

    void sendNoteOff(uint8_t, uint8_t, uint8_t) {}

    uint8_t getType() {
        return 0;
    }

    uint8_t getChannel() {
        return 0;
    }

    uint8_t getData1() {
        return 0;
    }

    uint8_t getData2() {
        return 0;
    }

    static const uint8_t ControlChange = 0xB0;
    static const uint8_t NoteOn = 0x90;
    static const uint8_t NoteOff = 0x80;
};

#ifndef CORE_TEENSY
extern DummyUsbMidi usbMIDI;
#endif