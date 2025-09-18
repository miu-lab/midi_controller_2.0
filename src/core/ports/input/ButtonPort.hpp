#pragma once

#include "core/domain/types.hpp"

/**
 * @brief Port d'entrée pour les boutons
 *
 * Cette interface définit le contrat pour l'interaction avec les boutons physiques.
 */
class ButtonPort {
public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~ButtonPort() = default;

    /**
     * @brief Met à jour l'état du bouton (lecture du hardware)
     */
    virtual void update() = 0;

    /**
     * @brief Retourne true si le bouton est pressé
     * @return true si le bouton est pressé, false sinon
     */
    virtual bool isPressed() const = 0;

    /**
     * @brief Identifiant unique du contrôle
     * @return ID du bouton
     */
    virtual InputId getId() const = 0;

    /**
     * @brief Réinitialise l'état du bouton (pour les boutons en mode toggle)
     * Implémentation par défaut vide pour compatibilité
     */
    virtual void resetState() {}
};
