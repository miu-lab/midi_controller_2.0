#pragma once

#include "config/SystemConstants.hpp"

/**
 * @brief Agrégation des configurations pour l'ApplicationConfiguration
 *
 * Ce namespace fournit un accès unifié aux configurations modulaires
 * pour maintenir la compatibilité avec ApplicationConfiguration.
 */
namespace ConfigDefaults {

// === PERFORMANCE ===
constexpr bool PERFORMANCE_MODE = SystemConstants::Performance::PERFORMANCE_MODE_DEFAULT;
constexpr unsigned long MAX_UPDATE_TIME_US = SystemConstants::Performance::MAX_UPDATE_TIME_US;
constexpr unsigned long MAX_INPUT_TIME_US = SystemConstants::Performance::INPUT_TIME_INTERVAL;
constexpr unsigned long MAX_MIDI_TIME_US = SystemConstants::Performance::MIDI_TIME_INTERVAL;
constexpr unsigned long MAX_UI_TIME_US = SystemConstants::Performance::UI_TIME_INTERVAL;

// === INTERFACE UTILISATEUR ===
constexpr int DISPLAY_REFRESH_RATE_HZ = SystemConstants::Performance::DISPLAY_REFRESH_RATE_HZ;
constexpr unsigned long DISPLAY_REFRESH_PERIOD_MS = SystemConstants::Performance::DISPLAY_REFRESH_PERIOD_MS;
constexpr bool SHOW_DEBUG_INFO = false;
constexpr bool ENABLE_FULL_UI = true;

// === CONTRÔLES ===
constexpr float DEFAULT_ENCODER_SENSITIVITY = SystemConstants::Input::DEFAULT_ENCODER_SENSITIVITY;
constexpr unsigned long ENCODER_RATE_LIMIT_MS = SystemConstants::Performance::ENCODER_RATE_LIMIT_MS;
constexpr unsigned long DUPLICATE_CHECK_MS = SystemConstants::Performance::DUPLICATE_CHECK_MS;

// === MIDI ===
constexpr uint8_t DEFAULT_MIDI_CHANNEL = SystemConstants::Audio::DEFAULT_CHANNEL;
constexpr uint8_t CC_VALUE_MIN = SystemConstants::Audio::CC_VALUE_MIN;
constexpr uint8_t CC_VALUE_MAX = SystemConstants::Audio::CC_VALUE_MAX;

// === SYSTÈME ===
constexpr unsigned long SERIAL_BAUD_RATE = SystemConstants::Hardware::SERIAL_BAUD_RATE;
constexpr size_t MAX_COMMAND_HISTORY = SystemConstants::Hardware::MAX_COMMAND_HISTORY;

}  // namespace ConfigDefaults
