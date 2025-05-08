#pragma once

#include <optional>
#include <vector>

#include "core/domain/types.hpp"

/**
 * @brief Port de sortie pour le stockage des profils
 *
 * Cette interface définit le contrat pour le stockage et la récupération
 * des profils MIDI (mappings entre contrôles et fonctions MIDI).
 */
class ProfileStoragePort {
public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~ProfileStoragePort() = default;

    /**
     * @brief Obtient le mapping MIDI pour un contrôle
     * @param id Identifiant du contrôle
     * @return Le mapping MIDI s'il existe, std::nullopt sinon
     */
    virtual std::optional<MidiControl> getBinding(ControlId id) const = 0;

    /**
     * @brief Définit un mapping MIDI pour un contrôle
     * @param id Identifiant du contrôle
     * @param binding Mapping MIDI à associer
     */
    virtual void setBinding(ControlId id, MidiControl binding) = 0;

    /**
     * @brief Obtient tous les mappings MIDI
     * @return Un vecteur de tous les mappings configurés
     */
    virtual std::vector<MidiControlMapping> getAllMappings() const = 0;

    /**
     * @brief Sauvegarde les mappings actuels dans le stockage persistant
     * @return true si la sauvegarde a réussi, false sinon
     */
    virtual bool saveProfile() = 0;

    /**
     * @brief Charge des mappings depuis le stockage persistant
     * @return true si le chargement a réussi, false sinon
     */
    virtual bool loadProfile() = 0;

    /**
     * @brief Réinitialise tous les mappings aux valeurs par défaut
     */
    virtual void resetToDefaults() = 0;

    /**
     * @brief Supprime un mapping
     * @param id Identifiant du contrôle
     * @return true si le mapping a été supprimé, false s'il n'existait pas
     */
    virtual bool removeBinding(ControlId id) = 0;
};
