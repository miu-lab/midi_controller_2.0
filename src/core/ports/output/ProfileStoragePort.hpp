#pragma once

#include <optional>
#include <vector>

#include "config/unified/ControlDefinition.hpp"
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
     * @brief Obtient la définition de contrôle complète pour un ID
     * @param id Identifiant du contrôle
     * @return La définition complète s'elle existe, std::nullopt sinon
     */
    virtual std::optional<ControlDefinition> getControlDefinition(InputId id) const = 0;

    /**
     * @brief Définit une définition de contrôle complète
     * @param controlDef Définition complète du contrôle
     */
    virtual void setControlDefinition(const ControlDefinition& controlDef) = 0;

    /**
     * @brief Obtient toutes les définitions de contrôles
     * @return Un vecteur de toutes les définitions configurées
     */
    virtual std::vector<ControlDefinition> getAllControlDefinitions() const = 0;

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
    virtual bool removeBinding(InputId id) = 0;
};
