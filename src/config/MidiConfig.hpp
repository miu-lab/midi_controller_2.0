#pragma once

#include <cstdint>

/**
 * @brief Configuration MIDI centralisée
 * 
 * Tous les paramètres et constantes liés au MIDI :
 * canaux, valeurs CC, configurations par défaut.
 */
namespace MidiConfig {

// === CANAUX ET VALEURS ===
constexpr uint8_t DEFAULT_CHANNEL = 0;      // Canal MIDI 1 (0-15)
constexpr uint8_t CC_VALUE_MIN = 0;         // Valeur CC minimale
constexpr uint8_t CC_VALUE_MAX = 127;       // Valeur CC maximale
constexpr uint8_t CC_VALUE_DEFAULT = 64;    // Valeur CC par défaut (milieu)

// === PLAGES DE CC ===
constexpr uint8_t CC_RANGE = CC_VALUE_MAX - CC_VALUE_MIN + 1;  // 128 valeurs

// === CONFIGURATION USB ===
constexpr unsigned long USB_MIDI_BAUD_RATE = 115200;  // Vitesse série USB-MIDI

}  // namespace MidiConfig