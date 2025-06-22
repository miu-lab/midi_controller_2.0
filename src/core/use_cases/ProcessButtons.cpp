#include "core/use_cases/ProcessButtons.hpp"

#include "core/use_cases/ButtonStateProcessor.hpp"

ProcessButtons::ProcessButtons(const std::vector<ButtonPort*>& buttons)
    : buttons_(buttons),
      lastPressed_(buttons.size(), false),
      initialized_(false),
      onButtonStateChangedCallback_(nullptr),
      inputController_(nullptr),
      useInputController_(false) {}

void ProcessButtons::initStates() {
    for (size_t i = 0; i < buttons_.size(); ++i) {
        lastPressed_[i] = buttons_[i]->isPressed();
    }
    initialized_ = true;
}

void ProcessButtons::setOnButtonStateChangedCallback(ButtonStateChangedCallback callback) {
    onButtonStateChangedCallback_ = callback;
}

void ProcessButtons::setInputController(InputController* inputController) {
    inputController_ = inputController;
    useInputController_ = (inputController_ != nullptr);
}

void ProcessButtons::update() {
    if (!initialized_) {
        initStates();
        return;
    }

    // Utilise le template pour éviter la duplication de code
    processButtonChanges(buttons_, lastPressed_, [this](uint8_t id, bool pressed) {
        if (onButtonStateChangedCallback_) {
            onButtonStateChangedCallback_(id, pressed);
        } else if (useInputController_) {
            inputController_->processButtonPress(id, pressed);
        }
    });
}