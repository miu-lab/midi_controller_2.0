#pragma once
#include <optional>
#include <variant>

#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderConfig.hpp"

/**
 * @brief Configuration générique pour un contrôle d'entrée (encodeur ou bouton).
 */
struct InputConfig {
    InputId id = NULL;           // Identifiant du contrôle
    InputName name{"input_"};    // Nom du contrôle
    InputType type;              // Type de contrôle (encodeur ou bouton)
    InputLabel label{"Input_"};  // Étiquette du contrôle
    std::variant<ButtonConfig, EncoderConfig>
        config;  // Configuration du bouton ou de l'encodeur (si présent)

    template <typename T>
    std::optional<T> getConfig() const {
        if (auto* config = std::get_if<T>(&this->config)) {
            return *config;
        }
        return std::nullopt;
    }
};