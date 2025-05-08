#pragma once
#include <vector>

#include "core/controllers/InputController.hpp"
#include "core/domain/IButton.hpp"

class ProcessButtons {
public:
    /**
     * @brief Constructeur
     * @param buttons Liste des boutons à traiter
     */
    explicit ProcessButtons(const std::vector<IButton*>& buttons);

    /**
     * @brief Définit le contrôleur d'entrée à utiliser
     * @param inputController Contrôleur d'entrée
     */
    void setInputController(InputController* inputController);

    /**
     * @brief Mise à jour des boutons
     */
    void update();

    /**
     * @brief Initialise les états sans déclencher d'événements
     */
    void initStates();

private:
    std::vector<IButton*> buttons_;
    std::vector<bool> lastPressed_;
    bool initialized_;
    InputController* inputController_;
    bool useInputController_;
};
