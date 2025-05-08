#include "core/domain/strategies/RelativeMappingStrategy.hpp"

#include <Arduino.h>

RelativeMappingStrategy::RelativeMappingStrategy(float sensitivity, EncodingType encoding,
                                                 bool acceleration)
    : sensitivity_(sensitivity),
      encoding_(encoding),
      acceleration_(acceleration),
      lastPhysicalValue_(0),
      lastTime_(0) {}

uint8_t RelativeMappingStrategy::mapToMidi(int32_t physicalValue, uint8_t previousValue) const {
    unsigned long currentTime = millis();

    // Calculer la différence par rapport à la dernière valeur
    int delta = physicalValue - lastPhysicalValue_;

    // Si aucun changement, retourner la valeur neutre
    if (delta == 0) {
        // 64 est neutre pour BINARY_OFFSET, 0 pour les autres
        return (encoding_ == EncodingType::BINARY_OFFSET) ? 64 : 0;
    }

    // Appliquer l'accélération si activée
    float deltaMultiplier = 1.0f;
    if (acceleration_ && lastTime_ > 0) {
        unsigned long timeDelta = currentTime - lastTime_;
        deltaMultiplier = calculateAcceleration(delta, timeDelta);
    }

    // Appliquer le facteur de sensibilité et l'accélération
    delta = (int)(delta * sensitivity_ * deltaMultiplier);

    // Mettre à jour les variables pour le prochain appel
    lastPhysicalValue_ = physicalValue;
    lastTime_ = currentTime;

    // Encoder selon le format choisi
    return encodeRelative(delta);
}

int32_t RelativeMappingStrategy::mapFromMidi(uint8_t midiValue) const {
    // Décoder la valeur MIDI selon le format
    return decodeRelative(midiValue);
}

const char* RelativeMappingStrategy::getName() const {
    switch (encoding_) {
    case EncodingType::BINARY_OFFSET:
        return "Relative (Binary Offset)";
    case EncodingType::SIGNED_BIT:
        return "Relative (Signed Bit)";
    case EncodingType::SIGNED_2:
        return "Relative (Signed 2's)";
    case EncodingType::INCREMENT_TYPE:
        return "Relative (Increment)";
    default:
        return "Relative";
    }
}

void RelativeMappingStrategy::setLastPhysicalValue(int32_t lastPhysicalValue) {
    lastPhysicalValue_ = lastPhysicalValue;
}

void RelativeMappingStrategy::setLastTime(unsigned long lastTime) {
    lastTime_ = lastTime;
}

uint8_t RelativeMappingStrategy::encodeRelative(int delta) const {
    // Limiter le delta à une plage raisonnable
    delta = constrain(delta, -63, 63);

    switch (encoding_) {
    case EncodingType::BINARY_OFFSET:
        // 64 est le point neutre, les valeurs s'additionnent : 65 = +1, 63 = -1, etc.
        return (uint8_t)(64 + delta);

    case EncodingType::SIGNED_BIT:
        // Bit 7 indique la direction (0 = positif, 1 = négatif)
        // Bits 0-6 indiquent la magnitude (0-63)
        if (delta >= 0) {
            return (uint8_t)(delta & 0x3F);  // Positif: 0x00-0x3F
        } else {
            return (uint8_t)(0x40 | (-delta & 0x3F));  // Négatif: 0x40-0x7F
        }

    case EncodingType::SIGNED_2:
        // Format signé deux's complement
        // Positif: 0x01-0x3F, Négatif: 0x41-0x7F
        if (delta > 0) {
            return (uint8_t)(delta & 0x3F);  // Positif: 0x01-0x3F
        } else if (delta < 0) {
            return (uint8_t)(0x40 | (-delta & 0x3F));  // Négatif: 0x41-0x7F
        } else {
            return 0;  // Pas de changement
        }

    case EncodingType::INCREMENT_TYPE:
        // Format simple : 0x01 = +1, 0x7F = -1
        if (delta > 0) {
            return 0x01;  // Toujours +1
        } else if (delta < 0) {
            return 0x7F;  // Toujours -1
        } else {
            return 0;  // Pas de changement
        }

    default:
        return 64;  // Valeur par défaut (pas de changement en BINARY_OFFSET)
    }
}

int RelativeMappingStrategy::decodeRelative(uint8_t midiValue) const {
    switch (encoding_) {
    case EncodingType::BINARY_OFFSET:
        // 64 est le point neutre
        return (int)midiValue - 64;

    case EncodingType::SIGNED_BIT:
        // Bit 7 indique la direction, bits 0-6 indiquent la magnitude
        if ((midiValue & 0x40) == 0) {
            return (int)(midiValue & 0x3F);  // Positif
        } else {
            return -(int)(midiValue & 0x3F);  // Négatif
        }

    case EncodingType::SIGNED_2:
        // Format signé two's complement
        if (midiValue == 0) {
            return 0;  // Pas de changement
        } else if ((midiValue & 0x40) == 0) {
            return (int)(midiValue & 0x3F);  // Positif
        } else {
            return -(int)(midiValue & 0x3F);  // Négatif
        }

    case EncodingType::INCREMENT_TYPE:
        // Format simple
        if (midiValue == 0x01) {
            return 1;  // Incrément
        } else if (midiValue == 0x7F) {
            return -1;  // Décrément
        } else {
            return 0;  // Pas de changement
        }

    default:
        return 0;  // Valeur par défaut (pas de changement)
    }
}

float RelativeMappingStrategy::calculateAcceleration(int delta, unsigned long timeDelta) const {
    // Éviter la division par zéro
    if (timeDelta == 0) return 1.0f;

    // Calculer la "vitesse" de rotation
    float speed = abs((float)delta) / timeDelta;

    // Formule d'accélération :
    // - Faible vitesse : peu ou pas d'accélération
    // - Haute vitesse : accélération plus importante
    // Ces valeurs peuvent être ajustées en fonction du comportement souhaité

    // Seuil en dessous duquel il n'y a pas d'accélération
    const float SPEED_THRESHOLD = 0.01f;

    if (speed < SPEED_THRESHOLD) {
        return 1.0f;  // Pas d'accélération
    } else {
        // Formule d'accélération progressive (peut être ajustée)
        // Donne une accélération de 1.0 à 5.0 en fonction de la vitesse
        return 1.0f +
               constrain(4.0f * (speed - SPEED_THRESHOLD) / (0.1f - SPEED_THRESHOLD), 0.0f, 4.0f);
    }
}
