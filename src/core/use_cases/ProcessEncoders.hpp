#pragma once
#include <functional>
#include <vector>

#include "config/common/CommonIncludes.hpp"
#include "core/controllers/InputController.hpp"
#include "core/ports/input/EncoderPort.hpp"

/**
 * @brief Cas d'utilisation qui traite la lecture des encodeurs physiques purs
 *
 * Cette classe est responsable uniquement de lire les rotations des encodeurs physiques
 * et de transmettre les événements de rotation sans filtrage ni stockage d'état.
 * La limitation de taux et le suivi de position sont gérés par les classes en aval (MidiMapper).
 */
class ProcessEncoders {
public:
    /**
     * @brief Constructeur
     * @param encoders Liste des encodeurs à traiter
     */
    explicit ProcessEncoders(const std::vector<EncoderPort*>& encoders);

    /**
     * @brief Types de callbacks pour la rotation des encodeurs
     */
    using EncoderTurnedCallback = std::function<void(uint8_t, int32_t, int8_t)>;


    /**
     * @brief Définit le callback pour la rotation des encodeurs
     * @param callback Fonction à appeler
     */
    void setOnEncoderTurnedCallback(EncoderTurnedCallback callback);


    /**
     * @brief Définit le contrôleur d'entrée à utiliser
     * @param inputController Contrôleur d'entrée
     */
    void setInputController(InputController* inputController);

    /**
 * @brief Mise à jour des encodeurs
 * 
 * Cette méthode lit l'état des encodeurs physiques et transmet les événements bruts
 * sans filtrage ni stockage d'état. Seuls les changements de delta non nuls et
 * les changements d'état des boutons sont transmis.
 */
    void update();

private:
    std::vector<EncoderPort*> encoders_;
    EncoderTurnedCallback onEncoderTurnedCallback_;
    InputController* inputController_;
    bool useInputController_;
};