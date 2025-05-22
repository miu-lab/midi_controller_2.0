// adapters/secondary/hardware/input/encoders/InterruptQuadratureEncoder.cpp
#include "adapters/secondary/hardware/input/encoders/InterruptQuadratureEncoder.hpp"

#include <Arduino.h>

InterruptQuadratureEncoder::InterruptQuadratureEncoder(const EncoderConfig& cfg)
    : id_(cfg.id),
      encoder_(cfg.pinA.pin, cfg.pinB.pin)  // Utiliser les nouvelles propriétés GpioPin
      ,
      ppr_(cfg.ppr),
      hasButton_(cfg.buttonConfig.has_value()),
      buttonPin_(hasButton_ ? cfg.buttonConfig->gpio.pin : 0),
      activeLowButton_(hasButton_ ? cfg.buttonConfig->activeLow : false),
      lastPosition_(0),
      physicalPosition_(0),
      absolutePosition_(0) {
    // Pre-calcul du facteur de normalisation
    // REFERENCE_PPR correspond au PPR standard des encodeurs mécaniques
    const int32_t REFERENCE_PPR = 24;

    // Calculer avec un facteur d'échelle pour une meilleure précision
    // Shift de 8 bits pour avoir un facteur d'échelle de 256 (comme une virgule fixe)
    normalizationFactor_ = (REFERENCE_PPR << 8) / ppr_;
    // Configuration du bouton si présent
    if (hasButton_) {
        const auto& buttonCfg = cfg.buttonConfig.value();
        int pinModeValue = (buttonCfg.gpio.mode == PinMode::PULLUP)     ? INPUT_PULLUP
                           : (buttonCfg.gpio.mode == PinMode::PULLDOWN) ? INPUT_PULLDOWN
                                                                        : INPUT;
        pinMode(buttonPin_, pinModeValue);
    }
}

InterruptQuadratureEncoder::~InterruptQuadratureEncoder() {
    // La bibliothèque Encoder gère le nettoyage des interruptions
}

int8_t InterruptQuadratureEncoder::readDelta() {
    // Variables statiques pour le filtrage temporel
    static uint32_t lastChangeTime = 0;
    static const uint32_t MIN_CHANGE_INTERVAL_MS = 1;  // 5ms minimum entre changements

    // Filtrage temporel de base
    uint32_t currentTime = millis();
    if (currentTime - lastChangeTime < MIN_CHANGE_INTERVAL_MS) {
        return 0;  // Ignorer les changements trop rapides
    }

    // Lire la position actuelle de l'encodeur
    int32_t newPosition = encoder_.read();
    int32_t delta = newPosition - lastPosition_;

    // Si pas de changement, retourner immédiatement
    if (delta == 0) return 0;

    // Pour les valeurs extrêmes, appliquer un filtrage plus strict
    if ((physicalPosition_ <= 1 && delta < 0) ||    // Proche de 0 et décroissant
        (physicalPosition_ >= 126 && delta > 0)) {  // Proche de 127 et croissant
        static uint32_t extremeChangeTime = 0;
        static const uint32_t EXTREME_DEBOUNCE_MS = 4;  // Plus long aux extrémités

        if (currentTime - extremeChangeTime < EXTREME_DEBOUNCE_MS) {
            return 0;  // Filtrage rigoureux aux extrémités
        }

        extremeChangeTime = currentTime;
    }

    // Enregistrer le moment du changement
    lastChangeTime = currentTime;

    // Mettre à jour la position
    lastPosition_ = newPosition;
    physicalPosition_ += delta;

    // Calculer le delta normalisé avec facteur d'échelle
    int32_t normalizedDelta = (delta * normalizationFactor_) >> 8;

    // S'assurer qu'un mouvement physique réel produise au moins 1 delta
    if (delta != 0 && normalizedDelta == 0) {
        normalizedDelta = (delta > 0) ? 1 : -1;
    }

    // Limiter le delta à la plage d'un int8_t
    int8_t result = (normalizedDelta > INT8_MAX)   ? INT8_MAX
                    : (normalizedDelta < INT8_MIN) ? INT8_MIN
                                                   : static_cast<int8_t>(normalizedDelta);

    // Recalculer la position absolue
    absolutePosition_ = (physicalPosition_ * normalizationFactor_) >> 8;

    return result;
}

bool InterruptQuadratureEncoder::isPressed() const {
    if (!hasButton_) return false;
    // Lire l'état du bouton
    int raw = digitalRead(buttonPin_);
    bool pressed = activeLowButton_ ? (raw == LOW) : (raw == HIGH);

    return pressed;
}

EncoderId InterruptQuadratureEncoder::getId() const {
    return id_;
}

uint16_t InterruptQuadratureEncoder::getPpr() const {
    return ppr_;
}

int32_t InterruptQuadratureEncoder::getAbsolutePosition() const {
    return absolutePosition_;
}

int32_t InterruptQuadratureEncoder::getPhysicalPosition() const {
    return physicalPosition_;
}

void InterruptQuadratureEncoder::resetPosition() {
    // Réinitialiser la position physique et la position absolue à zéro
    physicalPosition_ = 0;
    absolutePosition_ = 0;

    // Garder la dernière position physique, sinon cela pourrait générer des deltas indésirables
    // Nous réinitialisons seulement le compteur de position absolue
}