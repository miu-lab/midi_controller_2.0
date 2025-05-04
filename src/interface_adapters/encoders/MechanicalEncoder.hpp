// MechanicalEncoder � wrapper

#pragma once
#include "domain/IEncoder.hpp"
#ifdef UNIT_TEST_NATIVE
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif

/**
 * @brief Configuration d'un encodeur mécanique à 2 phases + push switch
 */
struct MechanicalEncoderConfig {
    EncoderId id;
    uint8_t pinA;
    uint8_t pinB;
    uint8_t pinSwitch;        ///< pin du bouton poussoir intégré
    bool    activeLowSwitch;  ///< true si le switch est tiré vers HIGH et actif à LOW
};

/**
 * @brief Implémentation matérielle d'un encoder mécanique (quadrature + bouton)
 */
class MechanicalEncoder : public IEncoder {
public:
    explicit MechanicalEncoder(const MechanicalEncoderConfig& cfg);

    int8_t  readDelta() override;
    bool    isPressed() const override;
    EncoderId getId() const override;

private:
    EncoderId  id_;
    uint8_t    pinA_, pinB_;
    uint8_t    pinSwitch_;
    bool       activeLowSwitch_;
    uint8_t    lastState_;
};
