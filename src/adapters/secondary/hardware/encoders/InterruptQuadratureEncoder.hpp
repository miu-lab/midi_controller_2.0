// adapters/secondary/hardware/encoders/InterruptQuadratureEncoder.hpp
#pragma once

#include "core/domain/IEncoder.hpp"
#include "adapters/secondary/hardware/encoders/EncoderConfig.hpp"
#include "config/debug/DebugConfig.hpp"  // Configuration des modes de debug
#include <Arduino.h>
#include <Encoder.h>  // Ajout de la bibliothèque Encoder

/**
 * @brief Encodeur quadrature utilisant la bibliothèque Encoder + bouton optionnel + PPR.
 *
 * Utilise la bibliothèque Encoder pour une lecture fiable des rotations
 * avec gestion automatique des interruptions et du debounce.
 */
class InterruptQuadratureEncoder : public IEncoder {
public:
    explicit InterruptQuadratureEncoder(const EncoderConfig& cfg);
    ~InterruptQuadratureEncoder() override;

    int8_t    readDelta() override;
    bool      isPressed() const override;
    EncoderId getId() const override;
    uint16_t  getPpr() const override;
    
    // Méthodes pour la position absolue
    int32_t   getAbsolutePosition() const override;
    int32_t   getPhysicalPosition() const override;
    void      resetPosition() override;

private:
    EncoderId id_;
    Encoder   encoder_;  // Utilisation de la classe Encoder au lieu de la gestion manuelle
    uint16_t  ppr_;      // Pulses Par Revolution
    bool      hasButton_;
    uint8_t   buttonPin_;
    bool      activeLowButton_;
    
    // Variables pour le calcul du delta
    int32_t   lastPosition_;
    
    // Position physique totale (non normalisée)
    int32_t   physicalPosition_;
    
    // Position absolue normalisée (cumulative)
    int32_t   absolutePosition_;
    
    // Variables pour les options d'accélération
    uint32_t  lastReadTime_;
    static constexpr uint32_t ACCELERATION_THRESHOLD_MS = 50; // Seuil pour l'accélération
};