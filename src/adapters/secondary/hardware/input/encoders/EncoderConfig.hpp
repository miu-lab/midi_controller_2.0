// adapters/secondary/hardware/input/encoders/EncoderConfig.hpp
#pragma once
#include <optional>

#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "config/InputConfig.hpp"
#include "core/domain/types.hpp"

/**
 * @brief Configuration générique pour un encodeur quadrature + switch optionnel et PPR.
 */
struct EncoderConfig {
    EncoderId id;                                    ///< Identifiant unique de l'encodeur
    GpioPin pinA;                                    ///< Broche GPIO du signal A (phase A)
    GpioPin pinB;                                    ///< Broche GPIO du signal B (phase B)
    uint16_t ppr = 24;                               ///< Impulsions par rotation (défaut cohérent)
    std::optional<ButtonConfig> buttonConfig;        ///< Configuration du bouton intégré (optionnel)
    
    // === NOUVELLES FONCTIONNALITÉS ===
    bool invertDirection = false;                    ///< Inverse le sens de rotation
    float sensitivity = InputConfig::DEFAULT_ENCODER_SENSITIVITY;  ///< Utilise InputConfig
    bool enableAcceleration = true;                  ///< Active l'accélération basée sur la vitesse
    uint8_t stepsPerDetent = 4;                      ///< Nombre de steps par cran mécanique
    uint16_t accelerationThreshold = 100;           ///< Seuil d'accélération en ms entre steps
    float maxAcceleration = 5.0f;                   ///< Facteur d'accélération maximum
    
    /**
     * @brief Vérifie si la configuration de l'encodeur est valide
     * @return true si tous les paramètres sont cohérents
     */
    constexpr bool isValid() const {
        return id != 0 && 
               pinA.isValid() && 
               pinB.isValid() && 
               pinA.pin != pinB.pin &&              // Les broches doivent être différentes
               ppr > 0 && ppr <= 10000 &&          // PPR raisonnable
               sensitivity > 0.0f && sensitivity <= 10.0f &&  // Sensibilité raisonnable
               stepsPerDetent > 0 && stepsPerDetent <= 8 &&   // Steps par détent raisonnable
               accelerationThreshold > 0 &&        // Seuil d'accélération positif
               maxAcceleration >= 1.0f && maxAcceleration <= 20.0f;  // Accélération raisonnable
    }
    
    /**
     * @brief Calcule la résolution effective en tenant compte des steps par détent
     * @return Résolution effective
     */
    constexpr float getEffectiveResolution() const {
        return static_cast<float>(ppr) / static_cast<float>(stepsPerDetent);
    }
};
