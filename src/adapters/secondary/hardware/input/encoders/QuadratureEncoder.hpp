// adapters/secondary/hardware/input/encoders/QuadratureEncoder.hpp
#pragma once

#include <Arduino.h>
#include <Encoder.h>  // Ajout de la bibliothèque Encoder

#include "adapters/secondary/hardware/input/encoders/EncoderConfig.hpp"
#include "core/ports/input/EncoderPort.hpp"

/**
 * @brief Encodeur quadrature pur utilisant la bibliothèque Encoder + PPR.
 *
 * Utilise la bibliothèque Encoder pour une lecture fiable des rotations
 * avec gestion automatique des interruptions et du debounce.
 */
class QuadratureEncoder : public EncoderPort {
public:
    explicit QuadratureEncoder(const EncoderConfig& cfg);
    ~QuadratureEncoder() override;

    int8_t readDelta() override;
    EncoderId getId() const override;
    uint16_t getPpr() const override;

    // Méthodes pour la position absolue
    int32_t getAbsolutePosition() const override;
    int32_t getPhysicalPosition() const override;
    void resetPosition() override;

private:
    EncoderId id_;
    Encoder encoder_;  // Utilisation de la classe Encoder au lieu de la gestion manuelle
    uint16_t ppr_;     // Pulses Par Revolution
    uint8_t stepsPerDetent_;  // Nombre de steps par cran mécanique

    // Variables pour le calcul du delta
    int32_t lastPosition_;

    // Position physique totale (non normalisée)
    int32_t physicalPosition_;

    // Position absolue normalisée (cumulative)
    int32_t absolutePosition_;

    // Facteur de normalisation pré-calculé
    int32_t normalizationFactor_;
    
    // Accumulation pour les encodeurs crantés
    int32_t stepAccumulator_;  // Accumule les steps jusqu'au seuil stepsPerDetent
};