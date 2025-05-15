#include "core/use_cases/ProcessEncoders.hpp"

#include <Arduino.h>

#include "config/debug/DebugMacros.hpp"
#include "core/domain/events/core/Events.hpp"
#include "tools/Diagnostics.hpp"

ProcessEncoders::ProcessEncoders(const std::vector<EncoderPort *> &encoders)
    : encoders_(encoders),
      lastPressed_(encoders.size(), false),
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
        EncoderPort *encoder = encoders_[i];

        // Lire les valeurs actuelles
        int32_t absPos = encoder->getAbsolutePosition();
        int8_t delta = encoder->readDelta();
        
        // Transmettre les rotations uniquement si delta est non nul
        // Note: Le filtrage supplémentaire et la limitation de taux sont gérés par MidiMapper
        if (delta != 0) {
            if (onEncoderTurnedCallback_) {
                onEncoderTurnedCallback_(encoder->getId(), absPos, delta);
            } else if (useInputController_) {
                inputController_->processEncoderTurn(encoder->getId(), absPos, delta);
            } else {
                DEBUG_ERROR("ProcessEncoders: NO HANDLER FOR ENCODER EVENTS!");
            }
        }
        
        // Gérer les boutons d'encodeur (détecter uniquement les changements d'état)
        bool pressed = encoder->isPressed();
        if (pressed != lastPressed_[i]) {
            lastPressed_[i] = pressed;
            
            #ifndef PERFORMANCE_MODE
            // Diagnostic pour le changement d'état du bouton d'encodeur
            char diagEvent[60];
            snprintf(diagEvent,
                     sizeof(diagEvent),
                     "ProcessEncoders: Bouton ID=%d %s",
                     encoder->getId(),
                     pressed ? "pressé" : "relâché");
            DIAG_ON_EVENT(diagEvent);
            #endif
            
            if (onEncoderButtonCallback_) {
                onEncoderButtonCallback_(encoder->getId(), pressed);
            } else if (useInputController_) {
                inputController_->processEncoderButton(encoder->getId(), pressed);
            } else {
                DEBUG_ERROR("ProcessEncoders: NO HANDLER FOR ENCODER BUTTON EVENTS!");
            }
        }
    }
}