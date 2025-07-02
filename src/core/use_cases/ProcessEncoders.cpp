#include "core/use_cases/ProcessEncoders.hpp"


ProcessEncoders::ProcessEncoders(const std::vector<EncoderPort*>& encoders)
    : encoders_(encoders),
      onEncoderTurnedCallback_(nullptr),
      inputController_(nullptr),
      useInputController_(false) {}

void ProcessEncoders::setOnEncoderTurnedCallback(EncoderTurnedCallback callback) {
    onEncoderTurnedCallback_ = callback;
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
}