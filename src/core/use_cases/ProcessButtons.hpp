#pragma once
#include <vector>
#include <functional>

#include "config/common/CommonIncludes.hpp"
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
     * @brief Type de callback pour les événements de bouton
     */
    using ButtonStateChangedCallback = std::function<void(uint8_t, bool)>;
    
    /**
     * @brief Définit le callback pour les changements d'état des boutons
     * @param callback Fonction à appeler
     */
    void setOnButtonStateChangedCallback(ButtonStateChangedCallback callback);
    
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
    ButtonStateChangedCallback onButtonStateChangedCallback_;
    InputController* inputController_;
    bool useInputController_;
};
