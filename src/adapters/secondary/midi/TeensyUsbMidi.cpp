#include "adapters/secondary/midi/TeensyUsbMidi.hpp"

// Définition de l'instance usbMIDI pour les plateformes non-Teensy
#if !defined(CORE_TEENSY) && !defined(ARDUINO_TEENSY31) && !defined(ARDUINO_TEENSY32) && \
    !defined(ARDUINO_TEENSY35) && !defined(ARDUINO_TEENSY36) && !defined(ARDUINO_TEENSY40) && \
    !defined(ARDUINO_TEENSY41)

    // Définition de l'instance globale
    TeensyUsbMidiStub usbMIDI;

#endif
