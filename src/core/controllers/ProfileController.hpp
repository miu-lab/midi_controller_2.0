#pragma once

#include <memory>
#include <vector>

#include "core/domain/IProfileManager.hpp"
#include "core/domain/strategies/MidiMappingFactory.hpp"
#include "core/domain/strategies/MidiMappingStrategy.hpp"
#include "core/domain/types.hpp"

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
    ProfileController(IProfileManager& profileManager);

    /**
     * @brief Obtient tous les mappings MIDI
     * @return Un vecteur de tous les mappings configurés
     */
    std::vector<MidiControlMapping> getAllMappings() const;

    /**
     * @brief Obtient le mapping pour un contrôle spécifique
     * @param controlId ID du contrôle
     * @return Le mapping MIDI, ou une valeur par défaut si non trouvé
     */
    MidiControl getMapping(ControlId controlId) const;

    /**
     * @brief Définit le mapping pour un contrôle
     * @param controlId ID du contrôle
     * @param midiControl Paramètres MIDI
     * @param relative Si true, utilise un mode relatif
     */
    void setMapping(ControlId controlId, const MidiControl& midiControl);

    /**
     * @brief Supprime le mapping pour un contrôle
     * @param controlId ID du contrôle
     * @return true si le mapping a été supprimé, false s'il n'existait pas
     */
    bool removeMapping(ControlId controlId);

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
     * @param midiControl Paramètres MIDI
     * @return Une stratégie de mapping appropriée
     */
    std::unique_ptr<IMidiMappingStrategy> createMappingStrategy(
        ControlId controlId, const MidiControl& midiControl) const;

private:
    IProfileManager& profileManager_;
};
