#pragma once

#include <cstdint>

/**
 * @brief Configuration hardware centralisée
 * 
 * Pins, vitesses SPI, paramètres hardware physiques.
 * Séparé de DisplayConfig pour permettre la réutilisation.
 */
namespace HardwareConfig {

// === COMMUNICATION SÉRIE ===
constexpr unsigned long SERIAL_BAUD_RATE = 115200;     // Vitesse série principale
constexpr size_t MAX_COMMAND_HISTORY = 10;             // Historique commandes

// === SPI CONFIGURATION ===
constexpr uint32_t SPI_SPEED_DISPLAY = 40000000;       // 40MHz pour affichage
constexpr uint32_t SPI_SPEED_DEFAULT = 8000000;        // 8MHz par défaut

// === PINS SYSTÈME ===
constexpr uint8_t LED_BUILTIN_PIN = 13;                // LED intégrée Teensy

// === TIMERS ET SCHEDULING ===
constexpr unsigned long MAIN_LOOP_INTERVAL_US = 1000;  // 1ms cycle principal
constexpr unsigned long WATCHDOG_TIMEOUT_MS = 5000;    // 5s timeout watchdog

}  // namespace HardwareConfig