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
    encoderConfigurations.clear();

    // Encodeurs de contrôle (Mécaniques)
    encoderConfigurations.emplace_back(
        EncoderConfig{.id = 71,
                      .pinA = 22,
                      .pinB = 23,
                      .ppr = 24,
                      .buttonConfig = ButtonConfig{.id = 1071, .pin = 21, .activeLow = true}});

    encoderConfigurations.emplace_back(EncoderConfig{
        .id = 72,
        .pinA = 19,
        .pinB = 20,
        .ppr = 24,
        .buttonConfig = ButtonConfig{.id = 1072, .pin = 18, .activeLow = true}});  // Encodeur 2

    encoderConfigurations.emplace_back(EncoderConfig{
        .id = 73,
        .pinA = 16,
        .pinB = 17,
        .ppr = 24,
        .buttonConfig = ButtonConfig{.id = 1073, .pin = 15, .activeLow = true}});  // Encodeur 3

    encoderConfigurations.emplace_back(EncoderConfig{
        .id = 74,
        .pinA = 13,
        .pinB = 14,
        .ppr = 24,
        .buttonConfig = ButtonConfig{.id = 1074, .pin = 41, .activeLow = true}});  // Encodeur 4

    encoderConfigurations.emplace_back(EncoderConfig{
        .id = 75,
        .pinA = 39,
        .pinB = 40,
        .ppr = 24,
        .buttonConfig = ButtonConfig{.id = 1075, .pin = 38, .activeLow = true}});  // Encodeur 5

    encoderConfigurations.emplace_back(EncoderConfig{
        .id = 76,
        .pinA = 36,
        .pinB = 37,
        .ppr = 24,
        .buttonConfig = ButtonConfig{.id = 1076, .pin = 35, .activeLow = true}});  // Encodeur 6

    encoderConfigurations.emplace_back(EncoderConfig{
        .id = 77,
        .pinA = 33,
        .pinB = 34,
        .ppr = 24,
        .buttonConfig = ButtonConfig{.id = 1077, .pin = 30, .activeLow = true}});  // Encodeur 7

    encoderConfigurations.emplace_back(EncoderConfig{
        .id = 78,
        .pinA = 28,
        .pinB = 29,
        .ppr = 24,
        .buttonConfig = ButtonConfig{.id = 1078, .pin = 27, .activeLow = true}});  // Encodeur 8

    // Encodeur de Navigation
    encoderConfigurations.emplace_back(EncoderConfig{
        .id = 79,
        .pinA = 9,
        .pinB = 10,
        .ppr = 96,
        .buttonConfig =
            ButtonConfig{.id = 1079, .pin = 8, .activeLow = true}});  // Encodeur Navigation

    // Encodeur Optique (haute résolution) - sans bouton
    encoderConfigurations.emplace_back(
        EncoderConfig{.id = 80,
                      .pinA = 11,
                      .pinB = 12,
                      .ppr = 600});  // Encodeur optique (buttonConfig = nullopt par défaut)
}