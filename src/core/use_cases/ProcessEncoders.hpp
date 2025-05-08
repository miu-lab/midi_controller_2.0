#pragma once
#include <vector>

#include "core/controllers/InputController.hpp"
#include "core/domain/IEncoder.hpp"

class ProcessEncoders {
public:
    /**
     * @brief Constructeur
     * @param encoders Liste des encodeurs à traiter
     */
    explicit ProcessEncoders(const std::vector<IEncoder*>& encoders);

    /**
     * @brief Définit le contrôleur d'entrée à utiliser
     * @param inputController Contrôleur d'entrée
     */
    void setInputController(InputController* inputController);

    /**
     * @brief Mise à jour des encodeurs
     */
    void update();

private:
    std::vector<IEncoder*> encoders_;
    std::vector<bool> lastPressed_;
    std::vector<int32_t> lastAbsPos_;
    InputController* inputController_;
    bool useInputController_;
};
