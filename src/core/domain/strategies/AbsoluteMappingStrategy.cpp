#include "core/domain/strategies/AbsoluteMappingStrategy.hpp"

#include <Arduino.h>

AbsoluteMappingStrategy::AbsoluteMappingStrategy(int32_t minPhysical, int32_t maxPhysical,
                                                 bool clamp)
    : minPhysical_(minPhysical), maxPhysical_(maxPhysical), clamp_(clamp) {}

uint8_t AbsoluteMappingStrategy::mapToMidi(int32_t physicalValue, uint8_t previousValue) const {
    // Limiter la valeur physique à la plage définie si clamp est activé
    if (clamp_) {
        physicalValue = constrain(physicalValue, minPhysical_, maxPhysical_);
    }

    // Calculer le rapport entre la valeur physique et la plage
    float ratio = (float)(physicalValue - minPhysical_) / (maxPhysical_ - minPhysical_);

    // Convertir en valeur MIDI (0-127)
    int32_t midiValue = (int32_t)(ratio * 127.0f + 0.5f);

    // Limiter à la plage MIDI valide
    return (uint8_t)constrain(midiValue, 0, 127);
}

int32_t AbsoluteMappingStrategy::mapFromMidi(uint8_t midiValue) const {
    // Calculer la proportion dans la plage MIDI
    float ratio = (float)midiValue / 127.0f;

    // Convertir en valeur physique
    int32_t physicalValue = (int32_t)(minPhysical_ + ratio * (maxPhysical_ - minPhysical_) + 0.5f);

    return physicalValue;
}

const char* AbsoluteMappingStrategy::getName() const {
    return "Absolute";
}
