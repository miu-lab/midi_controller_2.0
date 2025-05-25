#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include "config/unified/ControlDefinition.hpp"
#include "config/HardwareConfiguration.hpp"
#include "config/MappingConfiguration.hpp"

/**
 * @brief Configuration unifiée qui génère automatiquement Hardware et Mapping
 * 
 * Cette classe centralise toute la configuration et génère automatiquement
 * les objets HardwareConfiguration et MappingConfiguration compatibles
 * avec le système existant.
 */
class UnifiedConfiguration {
public:
    UnifiedConfiguration();

    // === INTERFACE PRINCIPALE ===

    /**
     * @brief Ajoute une définition de contrôle complète
     */
    void addControl(ControlDefinition control);

    /**
     * @brief Génère la configuration hardware compatible
     */
    std::unique_ptr<HardwareConfiguration> generateHardwareConfig() const;

    /**
     * @brief Génère la configuration mapping compatible
     */
    std::unique_ptr<MappingConfiguration> generateMappingConfig() const;

    // === REQUÊTES ===

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
     */
    bool validate() const;

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

    // === MÉTHODES DE CONVERSION ===

    /**
     * @brief Convertit une ControlDefinition en InputConfig
     */
    InputConfig toInputConfig(const ControlDefinition& def) const;

    /**
     * @brief Convertit les mappings d'un contrôle en InputMapping
     */
    std::vector<InputMapping> toInputMappings(const ControlDefinition& def) const;

    /**
     * @brief Génère un ButtonConfig depuis une définition
     */
    ButtonConfig toButtonConfig(const ControlDefinition& def, bool isEncoderButton = false) const;

    /**
     * @brief Génère un EncoderConfig depuis une définition
     */
    EncoderConfig toEncoderConfig(const ControlDefinition& def) const;
    
    /**
     * @brief Crée un InputConfig pour le bouton d'un encodeur
     */
    InputConfig createEncoderButtonInputConfig(const ControlDefinition& def) const;
};
