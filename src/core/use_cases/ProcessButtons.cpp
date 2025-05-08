#include "core/use_cases/ProcessButtons.hpp"

#include "core/domain/EventBus.hpp"
#include "core/domain/events/InputEvent.hpp"

ProcessButtons::ProcessButtons(const std::vector<IButton*>& buttons)
    : buttons_(buttons),
      lastPressed_(buttons.size(), false),
      initialized_(false),
      inputController_(nullptr),
      useInputController_(false) {}

void ProcessButtons::initStates() {
    // Capture l'état initial de tous les boutons sans déclencher d'événements
    for (size_t i = 0; i < buttons_.size(); ++i) {
        IButton* btn = buttons_[i];
        lastPressed_[i] = btn->isPressed();
    }
    initialized_ = true;
}

void ProcessButtons::setInputController(InputController* inputController) {
    inputController_ = inputController;
    useInputController_ = (inputController_ != nullptr);
}

void ProcessButtons::update() {
    // Si nous n'avons pas encore initialisé les états, le faire maintenant
    if (!initialized_) {
        initStates();
        return;
    }

    for (size_t i = 0; i < buttons_.size(); ++i) {
        IButton* btn = buttons_[i];
        bool pressed = btn->isPressed();
        if (pressed != lastPressed_[i]) {
            lastPressed_[i] = pressed;

            if (useInputController_) {
                // Utiliser le contrôleur d'entrée
                inputController_->processButtonPress(btn->getId(), pressed);
            } else {
                // Utiliser l'EventBus classique
                if (pressed)
                    EventBus<EventTypes::ButtonPressed>::publish({btn->getId()});
                else
                    EventBus<EventTypes::ButtonReleased>::publish({btn->getId()});
            }
        }
    }
}
