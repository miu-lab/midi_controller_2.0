#pragma once
#include <set>

#include "core/domain/types.hpp"

/**
 * @brief Service permettant de configurer quels contrôles sont dédiés à la navigation
 *
 * Ce service permet de distinguer les contrôles (encodeurs, boutons) qui sont
 * utilisés pour la navigation dans l'interface utilisateur plutôt que pour
 * envoyer des messages MIDI.
 */
class NavigationConfigService {
public:
    /**
     * @brief Définit un contrôle comme étant dédié à la navigation
     * @param id Identifiant du contrôle
     * @param isNavigation Indique si le contrôle est pour la navigation (true) ou MIDI normal
     * (false)
     */
    void setControlForNavigation(InputId id, bool isNavigation = true);

    /**
     * @brief Vérifie si un contrôle est dédié à la navigation
     * @param id Identifiant du contrôle à vérifier
     * @return true si le contrôle est dédié à la navigation, false sinon
     */
    bool isNavigationControl(InputId id) const;

private:
    std::set<InputId> navigationControls_;
};
