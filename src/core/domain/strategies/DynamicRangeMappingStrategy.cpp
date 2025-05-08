#include "core/domain/strategies/DynamicRangeMappingStrategy.hpp"

#include <Arduino.h>

DynamicRangeMappingStrategy::DynamicRangeMappingStrategy(int32_t initialMin, int32_t initialMax,
                                                         uint32_t resetThreshold)
    : minPhysical_(initialMin),
      maxPhysical_(initialMax),
      lastActivityTime_(millis()),
      resetThreshold_(resetThreshold) {}

uint8_t DynamicRangeMappingStrategy::mapToMidi(int32_t physicalValue, uint8_t previousValue) const {
    // Mettre à jour la plage en fonction de la valeur physique
    updateRange(physicalValue);

    // Si la plage est trop petite, éviter la division par zéro
    if (maxPhysical_ - minPhysical_ <= 0) {
        return previousValue;  // Conserver la valeur précédente
    }

    // Calculer le rapport entre la valeur physique et la plage dynamique
    float ratio = (float)(physicalValue - minPhysical_) / (maxPhysical_ - minPhysical_);

    // Convertir en valeur MIDI (0-127)
    int32_t midiValue = (int32_t)(ratio * 127.0f + 0.5f);

    // Limiter à la plage MIDI valide
    return (uint8_t)constrain(midiValue, 0, 127);
}

int32_t DynamicRangeMappingStrategy::mapFromMidi(uint8_t midiValue) const {
    // Calculer la proportion dans la plage MIDI
    float ratio = (float)midiValue / 127.0f;

    // Convertir en valeur physique
    int32_t physicalValue = (int32_t)(minPhysical_ + ratio * (maxPhysical_ - minPhysical_) + 0.5f);

    return physicalValue;
}

const char* DynamicRangeMappingStrategy::getName() const {
    return "DynamicRange";
}

void DynamicRangeMappingStrategy::updateRange(int32_t physicalValue) const {
    // Mettre à jour le temps de dernière activité
    lastActivityTime_ = millis();

    // Vérifier si la valeur sort de la plage actuelle
    if (physicalValue < minPhysical_) {
        minPhysical_ = physicalValue;

        // Maintenir un écart minimum
        if (maxPhysical_ - minPhysical_ < 10) {
            maxPhysical_ = minPhysical_ + 10;
        }
    } else if (physicalValue > maxPhysical_) {
        maxPhysical_ = physicalValue;

        // Maintenir un écart minimum
        if (maxPhysical_ - minPhysical_ < 10) {
            minPhysical_ = maxPhysical_ - 10;
        }
    }

#if defined(DEBUG) && defined(DEBUG_MAPPING) && (DEBUG_MAPPING >= 1)
    // Débogage de la plage
    Serial.print("RANGE ");
    Serial.print(minPhysical_);
    Serial.print(" - ");
    Serial.print(maxPhysical_);
    Serial.print(" Value: ");
    Serial.println(physicalValue);
#endif
}
