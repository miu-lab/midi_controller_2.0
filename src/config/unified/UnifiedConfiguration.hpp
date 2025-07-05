#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "config/unified/ControlDefinition.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Configuration unifiée moderne
 *
 * Cette classe centralise toute la configuration des contrôles
 * et fournit une interface unifiée pour tous les sous-systèmes.
 */
class UnifiedConfiguration {
public:
    UnifiedConfiguration();

    // === INTERFACE PRINCIPALE ===

    /**
     * @brief Ajoute une définition de contrôle complète
     */
    void addControl(ControlDefinition control);

    // === REQUÊTES ===

    /**
     * @brief Obtient tous les contrôles définis
     */
    const std::vector<ControlDefinition>& getAllControls() const;

    /**
     * @brief Trouve un contrôle par ID
     */
    std::optional<ControlDefinition> findControlById(InputId id) const;

    /**
     * @brief Liste tous les contrôles ayant un rôle spécifique
     */
    std::vector<ControlDefinition> getControlsByRole(MappingRole role) const;

    /**
     * @brief Valide la cohérence de toute la configuration
     * @return Result<void> avec détails d'erreur en cas d'échec de validation
     */
    Result<void> validate() const;

    /**
     * @brief Obtient les statistiques de la configuration
     */
    struct Stats {
        size_t totalControls;
        size_t encoders;
        size_t buttons;
        size_t midiMappings;
        size_t navigationMappings;
    };

    Stats getStats() const;

private:
    std::vector<ControlDefinition> controls_;
    std::unordered_map<InputId, size_t> idIndex_;  // ID -> index dans controls_
};
