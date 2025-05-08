#pragma once

#include "core/domain/strategies/MidiMappingStrategy.hpp"

/**
 * @brief Stratégie de mapping MIDI à plage dynamique
 *
 * Cette stratégie effectue un mapping adaptatif qui ajuste automatiquement
 * sa plage selon les valeurs extrêmes rencontrées. Elle est particulièrement
 * utile pour les encodeurs sans fin utilisés de manière relative mais produisant
 * des valeurs absolues hors limites.
 */
class DynamicRangeMappingStrategy : public IMidiMappingStrategy {
public:
    /**
     * @brief Constructeur
     * @param initialMin Valeur physique minimale initiale
     * @param initialMax Valeur physique maximale initiale
     * @param resetThreshold Seuil de temps (ms) pour réinitialiser la plage en cas d'inactivité
     */
    DynamicRangeMappingStrategy(int32_t initialMin = 0, int32_t initialMax = 127,
                                uint32_t resetThreshold = 5000);

    /**
     * @brief Convertit une valeur physique en valeur MIDI avec adaptation de plage
     * @param physicalValue Valeur physique du contrôle
     * @param previousValue Valeur MIDI précédente
     * @return Valeur MIDI calculée (0-127)
     */
    uint8_t mapToMidi(int32_t physicalValue, uint8_t previousValue) const override;

    /**
     * @brief Convertit une valeur MIDI en valeur physique
     * @param midiValue Valeur MIDI (0-127)
     * @return Valeur physique correspondante
     */
    int32_t mapFromMidi(uint8_t midiValue) const override;

    /**
     * @brief Obtient le nom de la stratégie
     * @return "DynamicRange"
     */
    const char* getName() const override;

private:
    mutable int32_t minPhysical_;        // Valeur physique minimale observée
    mutable int32_t maxPhysical_;        // Valeur physique maximale observée
    mutable uint32_t lastActivityTime_;  // Temps de la dernière activité
    uint32_t resetThreshold_;            // Seuil pour réinitialiser la plage

    /**
     * @brief Met à jour la plage physique si nécessaire
     * @param physicalValue Nouvelle valeur physique à prendre en compte
     */
    void updateRange(int32_t physicalValue) const;
};
