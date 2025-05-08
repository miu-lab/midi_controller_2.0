#pragma once

#include <memory>

#include "core/domain/strategies/AbsoluteMappingStrategy.hpp"
#include "core/domain/strategies/DynamicRangeMappingStrategy.hpp"
#include "core/domain/strategies/MidiMappingStrategy.hpp"
#include "core/domain/strategies/RelativeMappingStrategy.hpp"

/**
 * @brief Factory pour la création de stratégies de mapping MIDI
 */
class MidiMappingFactory {
public:
    /**
     * @brief Types de stratégies disponibles
     */
    enum class StrategyType {
        ABSOLUTE,            ///< Mapping absolu
        RELATIVE_BINARY,     ///< Mapping relatif format binaire (64 = neutre)
        RELATIVE_SIGNED,     ///< Mapping relatif format bit de signe
        RELATIVE_SIGNED2,    ///< Mapping relatif format signé 2's complement
        RELATIVE_INCREMENT,  ///< Mapping relatif format incrément simple
        DYNAMIC_RANGE        ///< Mapping à plage dynamique
    };

    /**
     * @brief Crée une stratégie de mapping MIDI
     * @param type Type de stratégie à créer
     * @return Pointeur vers la stratégie créée
     */
    static std::unique_ptr<IMidiMappingStrategy> create(StrategyType type);

    /**
     * @brief Crée une stratégie de mapping absolu
     * @param minPhysical Valeur physique minimale
     * @param maxPhysical Valeur physique maximale
     * @param clamp Si true, les valeurs hors plage sont ramenées à la plage
     * @return Pointeur vers la stratégie créée
     */
    static std::unique_ptr<AbsoluteMappingStrategy> createAbsolute(int32_t minPhysical,
                                                                   int32_t maxPhysical,
                                                                   bool clamp = true);

    /**
     * @brief Crée une stratégie de mapping relatif
     * @param sensitivity Sensibilité du changement
     * @param encoding Type d'encodage relatif
     * @param acceleration Si true, applique une accélération
     * @return Pointeur vers la stratégie créée
     */
    static std::unique_ptr<RelativeMappingStrategy> createRelative(
        float sensitivity = 1.0f,
        RelativeMappingStrategy::EncodingType encoding =
            RelativeMappingStrategy::EncodingType::BINARY_OFFSET,
        bool acceleration = true);

    /**
     * @brief Crée une stratégie de mapping à plage dynamique
     * @param initialMin Valeur physique minimale initiale
     * @param initialMax Valeur physique maximale initiale
     * @param resetThreshold Seuil pour réinitialiser la plage
     * @return Pointeur vers la stratégie créée
     */
    static std::unique_ptr<DynamicRangeMappingStrategy> createDynamicRange(
        int32_t initialMin = 0, int32_t initialMax = 127, uint32_t resetThreshold = 5000);
};
