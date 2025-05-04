#include "input/InputRouter.hpp"
#include "input/InputEvent.hpp"
#include "utils/EventBus.hpp"

InputRouter::InputRouter(IMidiOut& midiOut, IProfileManager& profileManager)
    : midiOut_(midiOut), profileManager_(profileManager) {}

void InputRouter::init() {
    EventBus<EncoderTurnedEvent>::subscribe([this](const EncoderTurnedEvent& e) {
        auto binding = profileManager_.getBinding(e.id);
        if (binding) {
            int16_t newValue = binding->relative ? e.delta * 2 : e.delta;
            midiOut_.sendCc(binding->channel, binding->control, static_cast<uint8_t>(newValue));
        }
    });

    EventBus<ButtonPressed>::subscribe([this](const ButtonPressed& e) {
        auto binding = profileManager_.getBinding(e.id);
        if (binding) {
            midiOut_.sendNoteOn(binding->channel, binding->control, 127);
        }
    });

    EventBus<ButtonReleased>::subscribe([this](const ButtonReleased& e) {
        auto binding = profileManager_.getBinding(e.id);
        if (binding) {
            midiOut_.sendNoteOff(binding->channel, binding->control, 0);
        }
    });
}
