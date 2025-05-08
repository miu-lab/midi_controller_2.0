#include "core/use_cases/ProcessEncoders.hpp"

#include "core/domain/EventBus.hpp"
#include "core/domain/events/InputEvent.hpp"

ProcessEncoders::ProcessEncoders(const std::vector<IEncoder *> &encoders)
    : encoders_(encoders),
      lastPressed_(encoders.size(), false),
      lastAbsPos_(encoders.size(), 0),
      inputController_(nullptr),
      useInputController_(false) {}

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

            // Utiliser le contrôleur d'entrée si disponible, sinon publier via l'EventBus
            if (useInputController_) {
                inputController_->processEncoderTurn(encoder->getId(), absPos, delta);
            } else {
                // Publier l'événement avec uniquement la position absolue
                EventBus<EventTypes::EncoderTurned>::publish(
                    {.id = encoder->getId(), .absolutePosition = absPos});
            }
        }

        bool pressed = encoder->isPressed();
        if (pressed != lastPressed_[i]) {
            lastPressed_[i] = pressed;

            // Utiliser le contrôleur d'entrée si disponible, sinon publier via l'EventBus
            if (useInputController_) {
                inputController_->processEncoderButton(encoder->getId(), pressed);
            } else {
                EventBus<EventTypes::EncoderButton>::publish({encoder->getId(), pressed});
            }
        }
    }
}
