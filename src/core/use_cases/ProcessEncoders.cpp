#include "core/use_cases/ProcessEncoders.hpp"

#include "core/domain/events/EventSystem.hpp"
#include "tools/Diagnostics.hpp"

ProcessEncoders::ProcessEncoders(const std::vector<IEncoder *> &encoders)
    : encoders_(encoders),
      lastPressed_(encoders.size(), false),
      lastAbsPos_(encoders.size(), 0),
      onEncoderTurnedCallback_(nullptr),
      onEncoderButtonCallback_(nullptr),
      inputController_(nullptr),
      useInputController_(false) {}

void ProcessEncoders::setOnEncoderTurnedCallback(EncoderTurnedCallback callback) {
    onEncoderTurnedCallback_ = callback;
}

void ProcessEncoders::setOnEncoderButtonCallback(EncoderButtonCallback callback) {
    onEncoderButtonCallback_ = callback;
}

void ProcessEncoders::setInputController(InputController *inputController) {
    inputController_ = inputController;
    useInputController_ = (inputController_ != nullptr);
}

void ProcessEncoders::update() {
    for (size_t i = 0; i < encoders_.size(); ++i) {
        IEncoder *encoder = encoders_[i];

        // Lire le delta pour mettre à jour la position interne
        int8_t delta = encoder->readDelta();

        // Obtenir la position absolue actuelle
        int32_t absPos = encoder->getAbsolutePosition();

        // N'envoyer un événement que si la position absolue a changé
        if (absPos != lastAbsPos_[i]) {
            // Diagnostic pour le changement d'encodeur
            char diagEvent[60];
            snprintf(diagEvent, sizeof(diagEvent), 
                    "ProcessEncoders: ID=%d absPos=%ld->%ld delta=%d", 
                    encoder->getId(), lastAbsPos_[i], absPos, delta);
            DIAG_ON_EVENT(diagEvent);
            
            // Déboguer uniquement les changements de position absolue
#if defined(DEBUG) && defined(DEBUG_RAW_CONTROLS) && (DEBUG_RAW_CONTROLS >= 1)
            Serial.print("ENC_ABS_CHANGED ");
            Serial.print(encoder->getId());
            Serial.print(" old:");
            Serial.print(lastAbsPos_[i]);
            Serial.print(" new:");
            Serial.println(absPos);
#endif
            // Mettre à jour la dernière position absolue connue
            lastAbsPos_[i] = absPos;

            // Ordre de priorité pour traiter l'événement:
            // 1. Nouveau callback (onEncoderTurnedCallback_)
            // 2. Ancien contrôleur d'entrée (inputController_)
            if (onEncoderTurnedCallback_) {
                onEncoderTurnedCallback_(encoder->getId(), absPos, delta);
            } else if (useInputController_) {
                inputController_->processEncoderTurn(encoder->getId(), absPos, delta);
            }
            // Les EventBus ne sont plus utilisés dans la nouvelle version
        }

        bool pressed = encoder->isPressed();
        if (pressed != lastPressed_[i]) {
            // Diagnostic pour le changement d'état du bouton d'encodeur
            char diagEvent[60];
            snprintf(diagEvent, sizeof(diagEvent), 
                    "ProcessEncoders: Bouton ID=%d %s", 
                    encoder->getId(), pressed ? "pressé" : "relâché");
            DIAG_ON_EVENT(diagEvent);
            
            lastPressed_[i] = pressed;

            // Ordre de priorité pour traiter l'événement:
            // 1. Nouveau callback (onEncoderButtonCallback_)
            // 2. Ancien contrôleur d'entrée (inputController_)
            if (onEncoderButtonCallback_) {
                onEncoderButtonCallback_(encoder->getId(), pressed);
            } else if (useInputController_) {
                inputController_->processEncoderButton(encoder->getId(), pressed);
            }
            // Les EventBus ne sont plus utilisés dans la nouvelle version
        }
    }
}
