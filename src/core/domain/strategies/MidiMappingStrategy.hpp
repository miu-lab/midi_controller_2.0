#pragma once

#include <cstdint>

#include "core/domain/types.hpp"

/**
 * @brief Interface pour les stratégies de mapping MIDI
 *
 * Cette interface définit le contrat pour les différentes stratégies
 * de mapping entre les valeurs physiques des contrôles et les valeurs MIDI.
 */
class IMidiMappingStrategy {
public:
    /**
     * @brief Convertit une valeur physique en valeur MIDI
     * @param physicalValue Valeur physique du contrôle (ex: position d'encodeur, état de bouton)
     * @param previousValue Valeur MIDI précédente, utile pour les stratégies relatives
     * @return Valeur MIDI calculée (0-127)
     */
    virtual uint8_t mapToMidi(int32_t physicalValue, uint8_t previousValue) const = 0;

    /**
     * @brief Convertit une valeur MIDI en valeur physique
     * @param midiValue Valeur MIDI (0-127)
     * @return Valeur physique correspondante
     */
    virtual int32_t mapFromMidi(uint8_t midiValue) const = 0;

    /**
     * @brief Obtient le nom de la stratégie
     * @return Nom de la stratégie
     */
    virtual const char* getName() const = 0;

    /**
     * @brief Destructeur virtuel
     */
    virtual ~IMidiMappingStrategy() = default;
};
