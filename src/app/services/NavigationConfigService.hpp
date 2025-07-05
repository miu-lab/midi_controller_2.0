#pragma once

#include "core/domain/interfaces/INavigationService.hpp"
#include "core/domain/types.hpp"
#include <set>

/**
 * @brief Service permettant de configurer quels contrôles sont dédiés à la navigation
 *
 * REFACTOR: Maintenant implémente INavigationService pour une architecture plus propre.
 * Ce service permet de distinguer les contrôles (encodeurs, boutons) qui sont
 * utilisés pour la navigation dans l'interface utilisateur plutôt que pour
 * envoyer des messages MIDI.
 * 
 * Changements:
 * - Implémente INavigationService interface
 * - Ajout de méthodes pour gestion groupée des contrôles
 * - Ajout de l'état d'initialisation
 */
class NavigationConfigService : public INavigationService {
public:
    /**
     * @brief Constructeur
     */
    NavigationConfigService();

    /**
     * @brief Destructeur
     */
    ~NavigationConfigService() override = default;

    // Implémentation INavigationService
    void setControlForNavigation(InputId id, bool isNavigation = true) override;
    bool isNavigationControl(InputId id) const override;
    void setNavigationControls(const std::set<InputId>& navigationControlIds) override;
    const std::set<InputId>& getNavigationControlIds() const override;
    void clearNavigationControls() override;
    bool isInitialized() const override;

    /**
     * @brief Initialise le service de navigation
     * @return true si l'initialisation réussit
     */
    bool init();

private:
    std::set<InputId> navigationControls_;
    bool initialized_;
};
