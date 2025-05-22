// adapters/secondary/hardware/encoders/EncoderConfig.hpp
#pragma once
#include <optional>

#include "adapters/secondary/hardware/buttons/ButtonConfig.hpp"
#include "core/domain/types.hpp"

/**
 * @brief Configuration générique pour un encodeur quadrature + switch optionnel et PPR.
 */
struct EncoderConfig {
    EncoderId id;
    uint8_t pinA;
    uint8_t pinB;
    uint16_t ppr = 600;
    std::optional<ButtonConfig> buttonConfig;  // Configuration du bouton (si présent)
};