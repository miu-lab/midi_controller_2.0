#pragma once

#include "core/domain/strategies/MidiMappingStrategy.hpp"

/**
 * @brief Stratégie de mapping MIDI absolu
 *
 * Cette stratégie effectue un mapping direct (absolu) entre une plage de valeurs
 * physiques et la plage MIDI 0-127.
 */
class AbsoluteMappingStrategy : public IMidiMappingStrategy {
public:
    /**
     * @brief Constructeur
     * @param minPhysical Valeur physique minimale
     * @param maxPhysical Valeur physique maximale
     * @param clamp Si true, les valeurs hors plage sont ramenées à la plage, sinon saturées
     */
    AbsoluteMappingStrategy(int32_t minPhysical, int32_t maxPhysical, bool clamp = true);

    /**
     * @brief Convertit une valeur physique en valeur MIDI selon un mapping absolu
     * @param physicalValue Valeur physique du contrôle
     * @param previousValue Valeur MIDI précédente (ignorée dans cette stratégie)
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
     * @return "Absolute"
     */
    const char* getName() const override;

private:
    int32_t minPhysical_;
    int32_t maxPhysical_;
    bool clamp_;
};
