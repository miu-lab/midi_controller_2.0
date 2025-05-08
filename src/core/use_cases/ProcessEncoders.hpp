#pragma once
#include <vector>
#include <functional>

#include "config/common/CommonIncludes.hpp"
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
     * @brief Types de callbacks pour la rotation des encodeurs
     */
    using EncoderTurnedCallback = std::function<void(uint8_t, int32_t, int8_t)>;
    
    /**
     * @brief Types de callbacks pour les boutons d'encodeurs
     */
    using EncoderButtonCallback = std::function<void(uint8_t, bool)>;
    
    /**
     * @brief Définit le callback pour la rotation des encodeurs
     * @param callback Fonction à appeler
     */
    void setOnEncoderTurnedCallback(EncoderTurnedCallback callback);
    
    /**
     * @brief Définit le callback pour les boutons d'encodeurs
     * @param callback Fonction à appeler
     */
    void setOnEncoderButtonCallback(EncoderButtonCallback callback);
    
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
    EncoderTurnedCallback onEncoderTurnedCallback_;
    EncoderButtonCallback onEncoderButtonCallback_;
    InputController* inputController_;
    bool useInputController_;
};
