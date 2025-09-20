// adapters/secondary/hardware/input/encoders/QuadratureEncoder.cpp
#include <Arduino.h>
#include <cmath>

#include "adapters/secondary/hardware/input/encoders/QuadratureEncoder.hpp"

QuadratureEncoder::QuadratureEncoder(const EncoderConfig& cfg)
    : id_(cfg.id),
      encoder_(cfg.pinA.pin, cfg.pinB.pin),
      ppr_(cfg.ppr),
      stepsPerDetent_(cfg.stepsPerDetent),
      lastPosition_(0),
      physicalPosition_(0),
      absolutePosition_(0),
      stepAccumulator_(0),
      normalizedAccumulator_(0.0f) {
    // Normalisation PPR : référence 24 PPR
    const int32_t REFERENCE_PPR = 24;
    normalizationRatio_ = static_cast<float>(REFERENCE_PPR) / static_cast<float>(ppr_);
}

QuadratureEncoder::~QuadratureEncoder() {
    // La bibliothèque Encoder gère le nettoyage des interruptions
}

int8_t QuadratureEncoder::readDelta() {
    // Filtrage temporel
    static uint32_t lastChangeTime = 0;
    static const uint32_t MIN_CHANGE_INTERVAL_MS = 1;

    uint32_t currentTime = millis();
    if (currentTime - lastChangeTime < MIN_CHANGE_INTERVAL_MS) {
        return 0;
    }

    // Lire l'encodeur
    int32_t newPosition = encoder_.read();
    int32_t delta = newPosition - lastPosition_;

    if (delta == 0) return 0;

    // Filtrage aux extrémités
    if ((physicalPosition_ <= 1 && delta < 0) ||
        (physicalPosition_ >= 126 && delta > 0)) {
        static uint32_t extremeChangeTime = 0;
        static const uint32_t EXTREME_DEBOUNCE_MS = 4;

        if (currentTime - extremeChangeTime < EXTREME_DEBOUNCE_MS) {
            return 0;
        }
        extremeChangeTime = currentTime;
    }

    lastChangeTime = currentTime;

    // Mettre à jour la position
    lastPosition_ = newPosition;
    physicalPosition_ += delta;

    // Accumulation par crans
    stepAccumulator_ += delta;
    
    int8_t detentSteps = 0;
    if (abs(stepAccumulator_) >= stepsPerDetent_) {
        detentSteps = stepAccumulator_ / static_cast<int32_t>(stepsPerDetent_);
        stepAccumulator_ %= static_cast<int32_t>(stepsPerDetent_);
    }
    
    if (detentSteps == 0) {
        return 0;
    }

    // Normalisation PPR avec accumulation flottante
    float normalizedFloat = detentSteps * normalizationRatio_;
    normalizedAccumulator_ += normalizedFloat;
    
    int32_t normalizedDelta = static_cast<int32_t>(normalizedAccumulator_);
    normalizedAccumulator_ -= normalizedDelta;

    // Limiter à int8_t
    int8_t result = (normalizedDelta > INT8_MAX)   ? INT8_MAX
                    : (normalizedDelta < INT8_MIN) ? INT8_MIN
                                                   : static_cast<int8_t>(normalizedDelta);

    // Inverser le sens des encodeurs
    result = -result;

    absolutePosition_ += result;
    return result;
}

EncoderId QuadratureEncoder::getId() const {
    return id_;
}

uint16_t QuadratureEncoder::getPpr() const {
    return ppr_;
}

int32_t QuadratureEncoder::getAbsolutePosition() const {
    return absolutePosition_;
}

int32_t QuadratureEncoder::getPhysicalPosition() const {
    return physicalPosition_;
}

void QuadratureEncoder::resetPosition() {
    physicalPosition_ = 0;
    absolutePosition_ = 0;
    stepAccumulator_ = 0;
    normalizedAccumulator_ = 0.0f;
}