#pragma once

#include "PerformanceConfig.hpp"
#include "MidiConfig.hpp"
#include "InputConfig.hpp"
#include "HardwareConfig.hpp"

/**
 * @brief Agrégation des configurations pour l'ApplicationConfiguration
 *
 * Ce namespace fournit un accès unifié aux configurations modulaires
 * pour maintenir la compatibilité avec ApplicationConfiguration.
 */
namespace ConfigDefaults {

// === PERFORMANCE ===
constexpr bool PERFORMANCE_MODE = PerformanceConfig::PERFORMANCE_MODE_DEFAULT;
constexpr unsigned long MAX_UPDATE_TIME_US = PerformanceConfig::MAX_UPDATE_TIME_US;
constexpr unsigned long MAX_INPUT_TIME_US = PerformanceConfig::INPUT_TIME_INTERVAL;
constexpr unsigned long MAX_MIDI_TIME_US = PerformanceConfig::MIDI_TIME_INTERVAL;
constexpr unsigned long MAX_UI_TIME_US = PerformanceConfig::UI_TIME_INTERVAL;

// === INTERFACE UTILISATEUR ===
constexpr int DISPLAY_REFRESH_RATE_HZ = PerformanceConfig::DISPLAY_REFRESH_RATE_HZ;
constexpr unsigned long DISPLAY_REFRESH_PERIOD_MS = PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS;
constexpr bool SHOW_DEBUG_INFO = false;
constexpr bool ENABLE_FULL_UI = true;

// === CONTRÔLES ===
constexpr float DEFAULT_ENCODER_SENSITIVITY = InputConfig::DEFAULT_ENCODER_SENSITIVITY;
constexpr unsigned long ENCODER_RATE_LIMIT_MS = PerformanceConfig::ENCODER_RATE_LIMIT_MS;
constexpr unsigned long DUPLICATE_CHECK_MS = PerformanceConfig::DUPLICATE_CHECK_MS;

// === MIDI ===
constexpr uint8_t DEFAULT_MIDI_CHANNEL = MidiConfig::DEFAULT_CHANNEL;
constexpr uint8_t CC_VALUE_MIN = MidiConfig::CC_VALUE_MIN;
constexpr uint8_t CC_VALUE_MAX = MidiConfig::CC_VALUE_MAX;

// === SYSTÈME ===
constexpr unsigned long SERIAL_BAUD_RATE = HardwareConfig::SERIAL_BAUD_RATE;
constexpr size_t MAX_COMMAND_HISTORY = HardwareConfig::MAX_COMMAND_HISTORY;

}  // namespace ConfigDefaults
