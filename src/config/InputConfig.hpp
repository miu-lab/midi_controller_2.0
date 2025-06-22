#pragma once

#include <cstdint>

/**
 * @brief Configuration des contrôles d'entrée
 * 
 * Paramètres pour encodeurs, boutons et autres contrôles :
 * sensibilité, accélération, seuils.
 */
namespace InputConfig {

// === ENCODEURS ===
constexpr float DEFAULT_ENCODER_SENSITIVITY = 1.0f;    // Sensibilité par défaut
constexpr uint16_t DEFAULT_PPR = 24;                   // Impulsions par rotation
constexpr uint8_t STEPS_PER_DETENT = 4;                // Steps par cran mécanique

// === ACCÉLÉRATION ENCODEURS ===
constexpr bool ENABLE_ACCELERATION_DEFAULT = true;     // Accélération activée par défaut
constexpr uint16_t ACCELERATION_THRESHOLD_MS = 100;    // Seuil d'accélération en ms
constexpr float MAX_ACCELERATION_FACTOR = 5.0f;        // Facteur d'accélération maximum
constexpr float MIN_SENSITIVITY = 0.1f;                // Sensibilité minimale
constexpr float MAX_SENSITIVITY = 10.0f;               // Sensibilité maximale

// === BOUTONS ===
constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;       // Anti-rebond boutons
constexpr unsigned long BUTTON_LONG_PRESS_MS = 1000;   // Durée pression longue

// === VALIDATION ===
constexpr uint16_t MAX_PPR = 10000;                    // PPR maximum autorisé
constexpr uint8_t MAX_STEPS_PER_DETENT = 8;            // Steps par détent maximum

}  // namespace InputConfig