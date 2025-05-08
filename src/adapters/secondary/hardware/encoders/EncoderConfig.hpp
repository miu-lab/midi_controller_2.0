// adapters/secondary/hardware/encoders/EncoderConfig.hpp
#pragma once
#include "core/domain/types.hpp"

/**
 * @brief Configuration générique pour un encodeur quadrature + switch optionnel et PPR.
 */
struct EncoderConfig {
    EncoderId id;
    uint8_t  pinA;
    uint8_t  pinB;

    // Pulses per revolution : résolution de l'encodeur
    uint16_t ppr = 600;

    // Switch optionnel :
    bool     hasButton       = false;
    uint8_t  pinButton       = 0;
    bool     activeLowButton = true;
};