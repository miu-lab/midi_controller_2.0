#pragma once

/**
 * @brief Valeurs par défaut et constantes de configuration
 *
 * Ce namespace contient toutes les constantes et valeurs par défaut
 * utilisées par ApplicationConfiguration.
 */
namespace ConfigDefaults {
// Performance
constexpr bool PERFORMANCE_MODE = false;
constexpr unsigned long MAX_UPDATE_TIME_US = 30000;  // 30ms
constexpr unsigned long MAX_INPUT_TIME_US = 1000;    // 1ms
constexpr unsigned long MAX_MIDI_TIME_US = 2000;     // 2ms
constexpr unsigned long MAX_UI_TIME_US = 2000;       // 2ms

// Interface utilisateur
constexpr int DISPLAY_REFRESH_RATE_HZ = 60;
constexpr unsigned long DISPLAY_REFRESH_PERIOD_MS = 1000 / DISPLAY_REFRESH_RATE_HZ;  // ~16.67ms
constexpr bool SHOW_DEBUG_INFO = false;
constexpr bool ENABLE_FULL_UI = true;

// Sensibilité des contrôles
constexpr float DEFAULT_ENCODER_SENSITIVITY = 1.0f;
constexpr unsigned long ENCODER_RATE_LIMIT_MS = 20;  // 30ms au lieu de 2ms -> limite à ~33Hz
constexpr unsigned long DUPLICATE_CHECK_MS = 10;     // 15ms au lieu de 4ms

// MIDI
constexpr uint8_t DEFAULT_MIDI_CHANNEL = 0;  // Canal MIDI 1 (0-15)
constexpr uint8_t CC_VALUE_MIN = 0;
constexpr uint8_t CC_VALUE_MAX = 127;

// Système
constexpr unsigned long SERIAL_BAUD_RATE = 115200;
constexpr size_t MAX_COMMAND_HISTORY = 10;
}  // namespace ConfigDefaults
