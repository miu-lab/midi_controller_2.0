// adapters/secondary/hardware/input/buttons/ButtonConfig.hpp
#pragma once
#include "core/domain/types.hpp"

/**
 * @brief Configuration pour un bouton simple (push-button).
 */
struct ButtonConfig {
    ButtonId id;                              ///< Identifiant logique du bouton
    GpioPin gpio;                             ///< Configuration GPIO sécurisée
    bool activeLow = true;                    ///< true si le bouton est actif à LOW
    ButtonMode mode = ButtonMode::MOMENTARY;  ///< Mode du bouton (momentary ou toggle)
    
    // === NOUVELLES FONCTIONNALITÉS ===
    uint16_t debounceMs = 50;                 ///< Délai anti-rebond en millisecondes
    uint16_t longPressMs = 800;               ///< Seuil de détection pression longue en ms
    bool enableLongPress = false;             ///< Active la détection de pression longue
    
    /**
     * @brief Vérifie si la configuration du bouton est valide
     * @return true si tous les paramètres sont cohérents
     */
    constexpr bool isValid() const {
        return id != 0 && 
               gpio.isValid() && 
               debounceMs > 0 && debounceMs < 1000 &&  // Debounce raisonnable
               longPressMs > debounceMs &&             // Long press > debounce
               longPressMs < 5000;                     // Long press < 5s
    }
};
