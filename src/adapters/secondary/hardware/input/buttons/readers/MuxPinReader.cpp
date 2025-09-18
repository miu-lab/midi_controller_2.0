#include "MuxPinReader.hpp"
#include "adapters/secondary/hardware/multiplexer/MultiplexerManager.hpp"
#include <Arduino.h>

MuxPinReader::MuxPinReader(uint8_t channel)
    : channel_(channel), initialized_(false) {

    if (channel > 15) {
        Serial.print("[MuxPinReader] ERROR: Invalid channel ");
        Serial.println(channel);
    }
}

void MuxPinReader::initialize() {
    if (initialized_) {
        return;
    }

    // S'assurer que le MultiplexerManager est initialisé
    MultiplexerManager& mux = MultiplexerManager::getInstance();
    if (!mux.isInitialized()) {
        mux.initialize();
    }

    initialized_ = true;

    #ifdef DEBUG_PIN_READERS
    Serial.print("[MuxPinReader] Channel ");
    Serial.print(channel_);
    Serial.println(" initialized");
    #endif
}

bool MuxPinReader::readPin() {
    if (!initialized_) {
        initialize();
    }

    MultiplexerManager& mux = MultiplexerManager::getInstance();
    if (!mux.isInitialized()) {
        return false;  // Valeur par défaut sécurisée
    }

    return mux.readDigitalFromChannel(channel_);
}