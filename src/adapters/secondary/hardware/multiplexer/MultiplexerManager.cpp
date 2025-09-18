#include "MultiplexerManager.hpp"
#include <Arduino.h>

using namespace SystemConstants::Multiplexer;

MultiplexerManager& MultiplexerManager::getInstance() {
    static MultiplexerManager instance;
    return instance;
}

MultiplexerManager::MultiplexerManager() : mux_(nullptr), currentChannel_(0), initialized_(false) {
}

void MultiplexerManager::initialize() {
    if (initialized_) {
        return;
    }

    // Créer l'instance du multiplexeur avec les pins de contrôle
    mux_ = std::make_unique<CD74HC4067>(S0_PIN, S1_PIN, S2_PIN, S3_PIN);

    // Configurer la pin de signal en entrée avec pull-up
    // Cela évite que les canaux non connectés flottent
    pinMode(SIGNAL_PIN, INPUT_PULLUP);

    // Sélectionner le canal 0 par défaut
    selectChannel(0);

    initialized_ = true;

    Serial.print("[MultiplexerManager] Initialized - Control pins: S0=");
    Serial.print(S0_PIN);
    Serial.print(" S1=");
    Serial.print(S1_PIN);
    Serial.print(" S2=");
    Serial.print(S2_PIN);
    Serial.print(" S3=");
    Serial.print(S3_PIN);
    Serial.print(" Signal=");
    Serial.println(SIGNAL_PIN);
}

void MultiplexerManager::selectChannel(uint8_t channel) {
    if (!initialized_) {
        // Ne pas faire d'initialisation automatique ici pour éviter les boucles
        return;
    }

    if (channel >= MAX_CHANNELS) {
        Serial.print("[MultiplexerManager] ERROR: Invalid channel ");
        Serial.println(channel);
        return;
    }

    if (channel != currentChannel_) {
        mux_->channel(channel);
        currentChannel_ = channel;

        // Délai pour stabilisation du signal après changement
        delayMicroseconds(SWITCH_DELAY_US);
    }
}

bool MultiplexerManager::readDigital() const {
    if (!initialized_) {
        // Retour silencieux avec valeur par défaut
        return false;
    }

    return digitalRead(SIGNAL_PIN);
}

uint16_t MultiplexerManager::readAnalog() const {
    if (!initialized_) {
        // Retour silencieux avec valeur par défaut
        return 0;
    }

    return analogRead(SIGNAL_PIN);
}

bool MultiplexerManager::readDigitalFromChannel(uint8_t channel) {
    selectChannel(channel);
    return readDigital();
}

uint16_t MultiplexerManager::readAnalogFromChannel(uint8_t channel) {
    selectChannel(channel);
    return readAnalog();
}