#include "UnifiedButton.hpp"
#include <Arduino.h>

UnifiedButton::UnifiedButton(const ButtonConfig& cfg, std::unique_ptr<IPinReader> pinReader)
    : cfg_(cfg),
      pinReader_(std::move(pinReader)),
      pressed_(false),
      toggleState_(false),
      lastRawState_(false),
      lastDebounceTime_(0),
      stableState_(false) {

    if (!pinReader_) {
        Serial.print("[UnifiedButton] ERROR: Null pinReader for button ");
        Serial.println(cfg_.id);
        return;
    }

    // Initialiser le lecteur de pin
    pinReader_->initialize();

    // Lire l'état initial
    bool initialState = readCurrentState();
    lastRawState_ = initialState;
    stableState_ = initialState;

    // Debug : afficher la configuration du bouton
    #ifdef DEBUG_UNIFIED_BUTTONS
    Serial.print("[UnifiedButton] Button ");
    Serial.print(cfg_.id);
    Serial.print(" initialized, initial state: ");
    Serial.println(stableState_ ? "PRESSED" : "RELEASED");
    #endif
}

void UnifiedButton::update() {
    if (!pinReader_) {
        return;
    }

    // Lire l'état actuel
    bool currentState = readCurrentState();

    // Implémenter le debounce
    if (currentState != lastRawState_) {
        lastDebounceTime_ = millis();
        lastRawState_ = currentState;
    }

    // Si l'état est stable depuis assez longtemps
    if ((millis() - lastDebounceTime_) > cfg_.debounceMs) {
        // Détecter un front montant
        bool rising = currentState && !stableState_;

        // Debug : afficher les changements d'état (optionnel)
        #ifdef DEBUG_UNIFIED_BUTTONS
        bool stateChanged = currentState != stableState_;
        if (stateChanged) {
            Serial.print("[UnifiedButton] Button ");
            Serial.print(cfg_.id);
            Serial.print(" ");
            Serial.println(currentState ? "PRESSED" : "RELEASED");
        }
        #endif

        stableState_ = currentState;

        // Gérer selon le mode
        if (cfg_.mode == ButtonMode::TOGGLE) {
            if (rising) {
                toggleState_ = !toggleState_;
            }
            pressed_ = toggleState_;
        } else {
            pressed_ = stableState_;
        }
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