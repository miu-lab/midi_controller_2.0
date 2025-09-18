#include "UnifiedButton.hpp"
#include <Arduino.h>

UnifiedButton::UnifiedButton(const ButtonConfig& cfg, std::unique_ptr<IPinReader> pinReader)
    : cfg_(cfg),
      pinReader_(std::move(pinReader)),
      pressed_(false),
      toggleState_(false),
      lastState_(false) {

    if (!pinReader_) {
        Serial.print("[UnifiedButton] ERROR: Null pinReader for button ");
        Serial.println(cfg_.id);
        return;
    }

    // Initialiser le lecteur de pin
    pinReader_->initialize();

    // Lire l'état initial
    bool initialState = readCurrentState();
    lastState_ = initialState;

    // Debug : afficher la configuration du bouton
    #ifdef DEBUG_UNIFIED_BUTTONS
    Serial.print("[UnifiedButton] Button ");
    Serial.print(cfg_.id);
    Serial.print(" initialized, initial state: ");
    Serial.println(lastState_ ? "PRESSED" : "RELEASED");
    #endif
}

void UnifiedButton::update() {
    if (!pinReader_) {
        return;
    }

    // Mettre à jour le debounce dans le pin reader
    pinReader_->update();

    // Lire l'état actuel (déjà débouncé par Bounce2)
    bool currentState = readCurrentState();

    // Détecter un front montant
    bool rising = currentState && !lastState_;

    // Debug : afficher les changements d'état (optionnel)
    #ifdef DEBUG_UNIFIED_BUTTONS
    bool stateChanged = currentState != lastState_;
    if (stateChanged) {
        Serial.print("[UnifiedButton] Button ");
        Serial.print(cfg_.id);
        Serial.print(" ");
        Serial.println(currentState ? "PRESSED" : "RELEASED");
    }
    #endif

    lastState_ = currentState;

    // Gérer selon le mode
    if (cfg_.mode == ButtonMode::TOGGLE) {
        if (rising) {
            toggleState_ = !toggleState_;
        }
        pressed_ = toggleState_;
    } else {
        pressed_ = currentState;
    }
}

bool UnifiedButton::isPressed() const {
    return pressed_;
}

ButtonId UnifiedButton::getId() const {
    return cfg_.id;
}

void UnifiedButton::resetState() {
    toggleState_ = false;
    pressed_ = false;
}

bool UnifiedButton::readCurrentState() {
    bool rawValue = pinReader_->readPin();

    // Appliquer la logique active-low si configurée
    if (cfg_.activeLow) {
        return !rawValue;  // Inverser : LOW = pressé, HIGH = non pressé
    } else {
        return rawValue;   // Direct : HIGH = pressé, LOW = non pressé
    }
}