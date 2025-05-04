// interface_adapters/buttons/DigitalButtonConfig.hpp
#pragma once
#include "domain/types.hpp"

/**
 * @brief Configuration pour un bouton simple (push-button).
 */
struct ButtonConfig {
    ButtonId id;          ///< Identifiant logique du bouton
    uint8_t  pin;         ///< GPIO du bouton
    bool     activeLow = true; ///< true si le bouton est actif à LOW (INPUT_PULLUP)
};