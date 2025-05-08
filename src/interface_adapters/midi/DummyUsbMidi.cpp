#include "interface_adapters/midi/DummyUsbMidi.hpp"

#if !defined(ARDUINO_TEENSY31) && !defined(ARDUINO_TEENSY32) && !defined(ARDUINO_TEENSY35) && \
    !defined(ARDUINO_TEENSY36) && !defined(ARDUINO_TEENSY40) && !defined(ARDUINO_TEENSY41) && \
    !defined(CORE_TEENSY)

// Définition de l'instance globale pour les plateformes non-Teensy
DummyUsbMidi usbMIDI;

#endif