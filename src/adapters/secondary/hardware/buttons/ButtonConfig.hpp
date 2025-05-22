// adapters/secondary/hardware/buttons/ButtonConfig.hpp
#pragma once
#include "core/domain/types.hpp"

// Énumération pour définir le mode du bouton
enum class ButtonMode {
    MOMENTARY,  // Mode par défaut : actif uniquement lorsqu'il est pressé
    TOGGLE      // Mode toggle : chaque pression inverse l'état
};

/**
 * @brief Configuration pour un bouton simple (push-button).
 */
struct ButtonConfig {
    ButtonId id;                              ///< Identifiant logique du bouton
    uint8_t pin;                              ///< GPIO du bouton
    bool activeLow = true;                    ///< true si le bouton est actif à LOW (INPUT_PULLUP)
    ButtonMode mode = ButtonMode::MOMENTARY;  ///< Mode du bouton (momentary ou toggle)
};