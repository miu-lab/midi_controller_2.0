#pragma once

#include <cstddef>  // Pour std::size_t
#include <unordered_map>
#include <vector>

#include "core/domain/types.hpp"

/**
 * @brief Configuration des mappings MIDI et des contrôles de navigation
 *
 * Cette classe centralise toutes les configurations de mapping entre les contrôles
 * physiques et les fonctions MIDI ou de navigation.
 */
class MappingConfiguration {
public:
    MappingConfiguration();

    /**
     * @brief Obtient le mapping MIDI pour un ID de contrôle spécifique
     * @param controlId L'ID du contrôle
     * @param controlType Le type de contrôle (rotation d'encodeur, bouton d'encodeur, etc.)
     * @return Le mapping MIDI correspondant, ou nullptr si aucun mapping n'est défini
     */
    const MidiControl* getMidiMappingForControl(
        InputId controlId, MappingControlType controlType = MappingControlType::ENCODER) const;

    /**
     * @brief Vérifie si un contrôle est dédié à la navigation
     * @param controlId L'ID du contrôle à vérifier
     * @return true si le contrôle est dédié à la navigation, false sinon
     */
    bool isNavigationControl(InputId controlId) const;

    /**
     * @brief Obtient tous les mappings MIDI
     * @return Un vecteur de tous les mappings MIDI configurés
     */
    const std::vector<InputMapping>& getMappedControls() const;

    /**
     * @brief Obtient tous les contrôles de navigation
     * @return Un vecteur de tous les contrôles de navigation configurés
     */
    const std::vector<NavigationControl>& getNavigationControls() const;

private:
    std::vector<InputMapping> controlsMapping;
    std::vector<NavigationControl> navigationControls;
    std::unordered_map<uint32_t, std::size_t>
        mappingIndex;  // Pour accès rapide par ID composite (ID << 8 | type)

    // Méthodes d'initialisation
    void setupMapping();
    void initializeNavigationControls();
    void buildMappingIndex();
};
