// interface_adapters/encoders/InterruptQuadratureEncoder.hpp
#pragma once

#include "domain/IEncoder.hpp"
#include "interface_adapters/encoders/EncoderConfig.hpp"
#include "interface_adapters/buttons/DigitalButtonConfig.hpp"
#include "interface_adapters/buttons/DigitalButton.hpp"
#include <Arduino.h>
#include <vector>
#include <algorithm>

/**
 * @brief Encodeur quadrature interrupt-driven + bouton optionnel + PPR.
 *
 * Utilise interruption RISING sur pinA, filtre rebonds via double lecture et debounce logiciel.
 */
class InterruptQuadratureEncoder : public IEncoder {
public:
    explicit InterruptQuadratureEncoder(const EncoderConfig& cfg);
    ~InterruptQuadratureEncoder();

    int8_t    readDelta() override;
    bool      isPressed() const override;
    EncoderId getId() const override;
    uint16_t  getPpr() const override;

private:
    EncoderId        id_;
    uint8_t          pinA_, pinB_;
    volatile int16_t count_;
    volatile int32_t position_;  ///< Position cumulée pour debug
    uint8_t          lastAB_;
    uint16_t         ppr_;
    bool             hasButton_;
    ButtonConfig     btnCfg_;

    // Debounce logiciel
    uint32_t         lastPublishUs_;
    static constexpr uint32_t debounceUs_ = 500; // µs

    static std::vector<InterruptQuadratureEncoder*>& instances();
    static void handleAllInterrupts();
    void handleInterrupt(uint8_t newAB);
};
