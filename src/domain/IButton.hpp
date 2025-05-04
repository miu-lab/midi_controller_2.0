#pragma once

#include "types.hpp"

/**
 * @brief Interface pour un bouton
 */
class IButton {
public:
    virtual ~IButton() = default;

    /**
     * @brief Met à jour l'état du bouton (lecture du hardware)
     */
    virtual void update() = 0;

    /**
     * @brief Retourne true si le bouton est pressé
     */
    virtual bool isPressed() const = 0;

    /**
     * @brief Identifiant unique du contrôle
     */
    virtual ControlId getId() const = 0;
};
