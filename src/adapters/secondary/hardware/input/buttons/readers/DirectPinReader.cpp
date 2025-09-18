#include "DirectPinReader.hpp"
#include <Arduino.h>

DirectPinReader::DirectPinReader(uint8_t pin, PinMode mode)
    : pin_(pin), mode_(mode), initialized_(false), bounce_() {
}

void DirectPinReader::initialize() {
    if (initialized_) {
        return;
    }

    // Configuration du mode de pin selon le type
    int pinModeValue = (mode_ == PinMode::PULLUP)     ? INPUT_PULLUP
                       : (mode_ == PinMode::PULLDOWN) ? INPUT_PULLDOWN
                                                      : INPUT;

    // Attacher la pin à Bounce2
    bounce_.attach(pin_, pinModeValue);
    bounce_.interval(5); // 5ms de debounce par défaut

    initialized_ = true;

    #ifdef DEBUG_PIN_READERS
    Serial.print("[DirectPinReader] Pin ");
    Serial.print(pin_);
    Serial.print(" initialized with mode ");
    Serial.println(static_cast<int>(mode_));
    #endif
}

bool DirectPinReader::readPin() {
    if (!initialized_) {
        initialize();
    }

    return bounce_.read();
}

void DirectPinReader::update() {
    if (!initialized_) {
        return;
    }

    bounce_.update();
}