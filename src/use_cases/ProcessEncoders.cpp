#include "use_cases/ProcessEncoders.hpp"
#include "input/InputEvent.hpp"
#include "utils/EventBus.hpp"

ProcessEncoders::ProcessEncoders(const std::vector<IEncoder*>& encoders)
    : encoders_(encoders), lastPressed_(encoders.size(), false) {}

void ProcessEncoders::update() {
    for (size_t i = 0; i < encoders_.size(); ++i) {
        IEncoder* encoder = encoders_[i];
        int8_t delta = encoder->readDelta();
        if (delta != 0) {
            EventBus<EncoderTurnedEvent>::publish({ encoder->getId(), delta });
        }

        bool pressed = encoder->isPressed();
        if (pressed != lastPressed_[i]) {
            lastPressed_[i] = pressed;
            EventBus<EncoderButtonEvent>::publish({ encoder->getId(), pressed });
        }
    }
}
