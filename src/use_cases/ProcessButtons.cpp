#include "use_cases/ProcessButtons.hpp"
#include "input/InputEvent.hpp"
#include "utils/EventBus.hpp"

ProcessButtons::ProcessButtons(const std::vector<IButton*>& buttons)
    : buttons_(buttons), lastPressed_(buttons.size(), false) {}

void ProcessButtons::update() {
    for (size_t i = 0; i < buttons_.size(); ++i) {
        IButton* btn = buttons_[i];
        bool pressed = btn->isPressed();
        if (pressed != lastPressed_[i]) {
            lastPressed_[i] = pressed;
            if (pressed)
                EventBus<ButtonPressed>::publish({ btn->getId() });
            else
                EventBus<ButtonReleased>::publish({ btn->getId() });
        }
    }
}
