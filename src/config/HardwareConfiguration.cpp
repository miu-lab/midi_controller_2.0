#include "config/HardwareConfiguration.hpp"

HardwareConfiguration::HardwareConfiguration() {
    initializeControlButtonConfigurations();
    initializeEncoderConfigurations();
}

const std::vector<ButtonConfig>& HardwareConfiguration::getControlButtonConfigurations() const {
    return controlButtonConfigurations;
}

const std::vector<EncoderConfig>& HardwareConfiguration::getEncoderConfigurations() const {
    return encoderConfigurations;
}

void HardwareConfiguration::initializeControlButtonConfigurations() {
    // Reprise des configurations actuelles avec des commentaires plus clairs
    controlButtonConfigurations = {

        {.id = 51,
         .pin = 32,
         .activeLow = true,
         .mode = ButtonMode::TOGGLE},  // Bouton de Menu (Mode Toggle)

        {.id = 52,
         .pin = 31,
         .activeLow = true,
         .mode = ButtonMode::MOMENTARY},  // Bouton de Validation (Mode Momentary)
    };
}

void HardwareConfiguration::initializeEncoderConfigurations() {
    // Reprise des configurations actuelles avec des commentaires plus descriptifs
    encoderConfigurations = {

        // Encodeurs de contrôle (Mécaniques)
        {.id = 71,
         .pinA = 22,
         .pinB = 23,
         .ppr = 24,
         .hasButton = true,
         .pinButton = 21,
         .activeLowButton = true},  // Encodeur 1

        {.id = 72,
         .pinA = 19,
         .pinB = 20,
         .ppr = 24,
         .hasButton = true,
         .pinButton = 18,
         .activeLowButton = true},  // Encodeur 2

        {.id = 73,
         .pinA = 16,
         .pinB = 17,
         .ppr = 24,
         .hasButton = true,
         .pinButton = 15,
         .activeLowButton = true},  // Encodeur 3

        {.id = 74,
         .pinA = 13,
         .pinB = 14,
         .ppr = 24,
         .hasButton = true,
         .pinButton = 41,
         .activeLowButton = true},  // Encodeur 4

        {.id = 75,
         .pinA = 39,
         .pinB = 40,
         .ppr = 24,
         .hasButton = true,
         .pinButton = 38,
         .activeLowButton = true},  // Encodeur 5

        {.id = 76,
         .pinA = 36,
         .pinB = 37,
         .ppr = 24,
         .hasButton = true,
         .pinButton = 35,
         .activeLowButton = true},  // Encodeur 6

        {.id = 77,
         .pinA = 33,
         .pinB = 34,
         .ppr = 24,
         .hasButton = true,
         .pinButton = 30,
         .activeLowButton = true},  // Encodeur 7

        {.id = 78,
         .pinA = 28,
         .pinB = 29,
         .ppr = 24,
         .hasButton = true,
         .pinButton = 27,
         .activeLowButton = true},  // Encodeur 8

        // Encodeur de Navigation
        {.id = 79,
         .pinA = 9,
         .pinB = 10,
         .ppr = 96,
         .hasButton = true,
         .pinButton = 8,
         .activeLowButton = true},  // Encodeur Navigation

        // Encodeur Optique (haute résolution)
        {.id = 80,
         .pinA = 11,
         .pinB = 12,
         .ppr = 600,
         .hasButton = false,
         .activeLowButton = true}  // Encodeur optique
    };
}
