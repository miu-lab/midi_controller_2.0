// adapters/secondary/hardware/buttons/DigitalButton.cpp
#include "adapters/secondary/hardware/buttons/DigitalButton.hpp"

DigitalButton::DigitalButton(const ButtonConfig& cfg)
    : cfg_(cfg), button_(), pressed_(false), toggleState_(false), prevPressed_(false) {
    pinMode(cfg_.pin, cfg_.activeLow ? INPUT_PULLUP : INPUT);
    button_.attach(cfg_.pin, cfg_.activeLow ? INPUT_PULLUP : INPUT);
    button_.interval(5);  // debounce interval (ms)

    // Lire l'état initial du bouton sans déclencher d'événement
    button_.update();
    prevPressed_ = cfg_.activeLow ? (button_.read() == LOW) : (button_.read() == HIGH);
    // S'assurer que les états initiaux sont cohérents avec l'état physique
    if (cfg_.mode == ButtonMode::MOMENTARY) {
        pressed_ = prevPressed_;
    } else {
        // En mode toggle, on commence toujours relâché
        toggleState_ = false;
        pressed_ = false;
    }
}

void DigitalButton::update() {
    button_.update();

    // Mettre à jour l'état physique du bouton
    bool rawPressed = cfg_.activeLow ? (button_.read() == LOW) : (button_.read() == HIGH);

    // Détecter les changements d'état physique (front montant)
    bool rising = rawPressed && !prevPressed_;
    prevPressed_ = rawPressed;

    // Mettre à jour l'état logique selon le mode du bouton
    if (cfg_.mode == ButtonMode::TOGGLE) {
        // En mode toggle, inverser l'état logique sur le front montant
        if (rising) {
            toggleState_ = !toggleState_;
        }
        // L'état retourné est l'état du toggle, pas l'état physique
        pressed_ = toggleState_;
    } else {
        // En mode momentary, l'état retourné est l'état physique direct
        pressed_ = rawPressed;
    }
}

bool DigitalButton::isPressed() const {
    return pressed_;
}

ButtonId DigitalButton::getId() const {
    return cfg_.id;
}

void DigitalButton::resetState() {
    // Réinitialiser l'état des boutons en mode toggle
    toggleState_ = false;
    pressed_ = false;
}