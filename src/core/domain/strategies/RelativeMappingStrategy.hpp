#pragma once

#include "core/domain/strategies/MidiMappingStrategy.hpp"

/**
 * @brief Stratégie de mapping MIDI relatif
 *
 * Cette stratégie effectue un mapping relatif où les changements de valeur
 * physique sont interprétés comme des incréments/décréments de la valeur MIDI.
 */
class RelativeMappingStrategy : public IMidiMappingStrategy {
public:
    /**
     * @brief Types d'encodage relatif MIDI
     */
    enum class EncodingType {
        BINARY_OFFSET,  ///< Format binaire avec offset (valeur 64 = pas de changement)
        SIGNED_BIT,     ///< Format bit de signe (bit 7 = direction, bits 0-6 = magnitude)
        SIGNED_2,  ///< Format signé sur 2's complement (0x01-0x3F = positif, 0x41-0x7F = négatif)
        INCREMENT_TYPE  ///< Format simple incrémentation (0x01 = +1, 0x7F = -1)
    };

    /**
     * @brief Constructeur
     * @param sensitivity Sensibilité du changement (facteur multiplicateur)
     * @param encoding Type d'encodage relatif à utiliser
     * @param acceleration Si true, applique une accélération basée sur la vitesse de rotation
     */
    RelativeMappingStrategy(float sensitivity = 1.0f,
                            EncodingType encoding = EncodingType::BINARY_OFFSET,
                            bool acceleration = true);

    /**
     * @brief Convertit un changement de valeur physique en valeur MIDI relative
     * @param physicalValue Valeur physique actuelle
     * @param previousValue Dernière valeur MIDI envoyée (ignorée dans cette stratégie)
     * @return Valeur MIDI encodée selon le format relatif choisi
     */
    uint8_t mapToMidi(int32_t physicalValue, uint8_t previousValue) const override;

    /**
     * @brief Convertit une valeur MIDI en changement physique
     * @param midiValue Valeur MIDI (0-127)
     * @return Changement de valeur physique correspondant
     */
    int32_t mapFromMidi(uint8_t midiValue) const override;

    /**
     * @brief Obtient le nom de la stratégie
     * @return "Relative" suivi du type d'encodage
     */
    const char* getName() const override;

    /**
     * @brief Définit la dernière valeur physique pour calculer le delta
     * @param lastPhysicalValue Dernière valeur physique
     */
    void setLastPhysicalValue(int32_t lastPhysicalValue);

    /**
     * @brief Définit le dernier temps pour calculer l'accélération
     * @param lastTime Dernier temps en millisecondes
     */
    void setLastTime(unsigned long lastTime);

private:
    float sensitivity_;
    EncodingType encoding_;
    bool acceleration_;
    mutable int32_t lastPhysicalValue_;
    mutable unsigned long lastTime_;

    /**
     * @brief Encode une différence de valeur dans le format relatif choisi
     * @param delta Différence de valeur
     * @return Valeur MIDI encodée
     */
    uint8_t encodeRelative(int delta) const;

    /**
     * @brief Décode une valeur MIDI relative en différence de valeur
     * @param midiValue Valeur MIDI encodée
     * @return Différence de valeur décodée
     */
    int decodeRelative(uint8_t midiValue) const;

    /**
     * @brief Calcule un facteur d'accélération basé sur la vitesse de rotation
     * @param delta Différence de valeur
     * @param timeDelta Différence de temps en millisecondes
     * @return Facteur d'accélération (>= 1.0)
     */
    float calculateAcceleration(int delta, unsigned long timeDelta) const;
};
