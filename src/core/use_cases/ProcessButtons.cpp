#include "core/use_cases/ProcessButtons.hpp"

#include "core/domain/events/EventSystem.hpp"
#include "tools/Diagnostics.hpp"

ProcessButtons::ProcessButtons(const std::vector<IButton*>& buttons)
    : buttons_(buttons),
      lastPressed_(buttons.size(), false),
      initialized_(false),
      onButtonStateChangedCallback_(nullptr),
      inputController_(nullptr),
      useInputController_(false) {}

void ProcessButtons::initStates() {
    // Capture l'état initial de tous les boutons sans déclencher d'événements
    for (size_t i = 0; i < buttons_.size(); ++i) {
        IButton* btn = buttons_[i];
        lastPressed_[i] = btn->isPressed();
    }
    initialized_ = true;
    
    // Diagnostic d'initialisation - utiliser un buffer plus grand pour éviter la troncature
    char diagEvent[50]; // Augmentation de la taille du buffer
    snprintf(diagEvent, sizeof(diagEvent), 
             "ProcessButtons: %d boutons initialisés", (int)buttons_.size());
    DIAG_ON_EVENT(diagEvent);
}

void ProcessButtons::setOnButtonStateChangedCallback(ButtonStateChangedCallback callback) {
    onButtonStateChangedCallback_ = callback;
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
            // Diagnostic pour le changement d'état du bouton
            char diagEvent[50];
            snprintf(diagEvent, sizeof(diagEvent), 
                     "ProcessButtons: Bouton ID=%d %s", 
                     btn->getId(), pressed ? "pressé" : "relâché");
            DIAG_ON_EVENT(diagEvent);
            
            lastPressed_[i] = pressed;

            // Ordre de priorité pour traiter l'événement:
            // 1. Nouveau callback (onButtonStateChangedCallback_)
            // 2. Ancien contrôleur d'entrée (inputController_)
            if (onButtonStateChangedCallback_) {
                onButtonStateChangedCallback_(btn->getId(), pressed);
            } else if (useInputController_) {
                // Utiliser le contrôleur d'entrée
                inputController_->processButtonPress(btn->getId(), pressed);
            }
            // Les EventBus ne sont plus utilisés dans la nouvelle version
        }
    }
}
