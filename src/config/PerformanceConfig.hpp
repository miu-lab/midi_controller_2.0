#pragma once

#include "PerformanceDefines.h"

/**
 * @brief Configuration des performances et timings
 * 
 * Centralise tous les paramètres liés aux performances :
 * refresh rates, timeouts, limites de temps d'exécution.
 */
namespace PerformanceConfig {

// === REFRESH RATES (utilise PerformanceDefines.h) ===
constexpr int DISPLAY_REFRESH_RATE_HZ = LVGL_REFRESH_RATE_HZ;
constexpr int VSYNC_SPACING = 2;
constexpr int DIFF_GAP = 6;
constexpr unsigned long DISPLAY_REFRESH_PERIOD_MS = LVGL_REFRESH_PERIOD_MS;

// === TIMEOUTS D'EXÉCUTION ===
constexpr unsigned long MAX_UPDATE_TIME_US = 30000;  // 30ms cycle principal
constexpr unsigned long MAX_INPUT_TIME_US = 1000;    // 1ms pour input
constexpr unsigned long MAX_MIDI_TIME_US = 2000;     // 2ms pour MIDI
constexpr unsigned long MAX_UI_TIME_US = 16000;      // 16ms pour UI

// === RATE LIMITING ===
constexpr unsigned long ENCODER_RATE_LIMIT_MS = 20;  // Limite encodeurs à ~50Hz
constexpr unsigned long DUPLICATE_CHECK_MS = 10;     // Vérification doublons

// === MODES PERFORMANCE ===
constexpr bool PERFORMANCE_MODE_DEFAULT = false;     // Mode normal par défaut

}  // namespace PerformanceConfig