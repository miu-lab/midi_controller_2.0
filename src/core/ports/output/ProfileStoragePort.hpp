#pragma once

#include <optional>
#include <vector>

#include "config/unified/ControlDefinition.hpp"
#include "core/domain/types.hpp"
#include "core/utils/Result.hpp"

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
     * @return Result contenant la définition ou une erreur
     */
    virtual Result<ControlDefinition> getControlDefinition(InputId id) const = 0;

    /**
     * @brief Définit une définition de contrôle complète
     * @param controlDef Définition complète du contrôle
     * @return Result<void> avec détails d'erreur en cas d'échec
     */
    virtual Result<void> setControlDefinition(const ControlDefinition& controlDef) = 0;

    /**
     * @brief Obtient toutes les définitions de contrôles
     * @return Result contenant le vecteur de définitions ou une erreur
     */
    virtual Result<std::vector<ControlDefinition>> getAllControlDefinitions() const = 0;

    /**
     * @brief Sauvegarde les mappings actuels dans le stockage persistant
     * @return Result<void> avec détails d'erreur en cas d'échec
     */
    virtual Result<void> saveProfile() = 0;

    /**
     * @brief Charge des mappings depuis le stockage persistant
     * @return Result<void> avec détails d'erreur en cas d'échec
     */
    virtual Result<void> loadProfile() = 0;

    /**
     * @brief Réinitialise tous les mappings aux valeurs par défaut
     * @return Result<void> avec détails d'erreur en cas d'échec
     */
    virtual Result<void> resetToDefaults() = 0;

    /**
     * @brief Supprime un mapping
     * @param id Identifiant du contrôle
     * @return Result<void> avec détails d'erreur en cas d'échec
     */
    virtual Result<void> removeBinding(InputId id) = 0;
};
