#pragma once

#include <memory>
#include <vector>

#include "config/unified/ControlDefinition.hpp"
#include "core/domain/strategies/MidiMappingFactory.hpp"
#include "core/domain/strategies/MidiMappingStrategy.hpp"
#include "core/domain/types.hpp"
#include "core/ports/output/ProfileStoragePort.hpp"

/**
 * @brief Contrôleur dédié à la gestion des profils
 *
 * Cette classe est responsable de toutes les opérations liées
 * à la gestion des profils (mapping MIDI, préférences, etc.)
 */
class ProfileController {
public:
    /**
     * @brief Constructeur
     * @param profileManager Gestionnaire de profils
     */
    ProfileController(ProfileStoragePort& profileManager);

    /**
     * @brief Obtient toutes les définitions de contrôles
     * @return Un vecteur de toutes les définitions configurées
     */
    std::vector<ControlDefinition> getAllControlDefinitions() const;

    /**
     * @brief Obtient la définition pour un contrôle spécifique
     * @param controlId ID du contrôle
     * @return La définition complète, ou une valeur par défaut si non trouvée
     */
    std::optional<ControlDefinition> getControlDefinition(InputId controlId) const;

    /**
     * @brief Définit la configuration complète pour un contrôle
     * @param controlDef Définition complète du contrôle
     */
    void setControlDefinition(const ControlDefinition& controlDef);

    /**
     * @brief Supprime le mapping pour un contrôle
     * @param controlId ID du contrôle
     * @return true si le mapping a été supprimé, false s'il n'existait pas
     */
    bool removeMapping(InputId controlId);

    /**
     * @brief Réinitialise tous les mappings aux valeurs par défaut
     */
    void resetToDefaults();

    /**
     * @brief Sauvegarde le profil actuel
     * @return true si la sauvegarde a réussi, false sinon
     */
    bool saveProfile();

    /**
     * @brief Charge un profil
     * @return true si le chargement a réussi, false sinon
     */
    bool loadProfile();

    /**
     * @brief Crée une stratégie de mapping appropriée pour un contrôle
     * @param controlId ID du contrôle
     * @param controlDef Définition complète du contrôle
     * @return Une stratégie de mapping appropriée
     */
    std::unique_ptr<IMidiMappingStrategy> createMappingStrategy(
        InputId controlId, const ControlDefinition& controlDef) const;

private:
    ProfileStoragePort& profileManager_;
};
