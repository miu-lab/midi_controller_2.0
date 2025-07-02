#include "core/use_cases/ProcessEncoders.hpp"

#include "core/use_cases/ButtonStateProcessor.hpp"


ProcessEncoders::ProcessEncoders(const std::vector<EncoderPort*>& encoders)
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

void ProcessEncoders::setInputController(InputController* inputController) {
    inputController_ = inputController;
    useInputController_ = (inputController_ != nullptr);
}

void ProcessEncoders::update() {
    // Process rotations
    for (auto* encoder : encoders_) {
        int8_t delta = encoder->readDelta();
        if (delta != 0) {
            int32_t absPos = encoder->getAbsolutePosition();

            if (onEncoderTurnedCallback_) {
                onEncoderTurnedCallback_(encoder->getId(), absPos, delta);
            } else if (useInputController_) {
                inputController_->processEncoderTurn(encoder->getId(), absPos, delta);
            }
        }
    }

    // Process buttons using shared template logic
    processButtonChanges(encoders_, lastPressed_, [this](uint8_t id, bool pressed) {
        // DEBUG MSG TO IMPLEMENT
        if (onEncoderButtonCallback_) {
            onEncoderButtonCallback_(id, pressed);
        } else if (useInputController_) {
            // DEBUG MSG TO IMPLEMENT
            inputController_->processEncoderButton(id, pressed);
        }
    });
}