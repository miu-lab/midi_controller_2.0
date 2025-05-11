// adapters/secondary/hardware/encoders/InterruptQuadratureEncoder.cpp
#include "adapters/secondary/hardware/encoders/InterruptQuadratureEncoder.hpp"

#include <Arduino.h>

InterruptQuadratureEncoder::InterruptQuadratureEncoder(const EncoderConfig &cfg)
    : id_(cfg.id),
      encoder_(cfg.pinA, cfg.pinB)  // Initialisation de l'objet Encoder
      ,
      ppr_(cfg.ppr),
      hasButton_(cfg.hasButton),
      buttonPin_(cfg.hasButton ? cfg.pinButton : 0),
      activeLowButton_(cfg.activeLowButton),
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
        pinMode(buttonPin_, activeLowButton_ ? INPUT_PULLUP : INPUT);
    }
}

InterruptQuadratureEncoder::~InterruptQuadratureEncoder() {
    // La bibliothèque Encoder gère le nettoyage des interruptions
}

int8_t InterruptQuadratureEncoder::readDelta() {
    // Lire la position actuelle de l'encodeur
    int32_t newPosition = encoder_.read();
    int32_t delta = newPosition - lastPosition_;

    // Si pas de changement, retourner immédiatement
    if (delta == 0) return 0;

    // Mettre à jour la dernière position immédiatement
    lastPosition_ = newPosition;

    // Mettre à jour la position physique totale
    physicalPosition_ += delta;

    // Calculer le delta normalisé avec facteur d'échelle pour éviter la perte de précision
    int32_t normalizedDelta = (delta * normalizationFactor_) >> 8;

    // S'assurer qu'un mouvement physique réel produise toujours au moins 1 delta
    if (delta != 0 && normalizedDelta == 0) {
        normalizedDelta = (delta > 0) ? 1 : -1;
    }

    // Limiter le delta à la plage d'un int8_t en une seule opération
    int8_t result = (normalizedDelta > INT8_MAX)   ? INT8_MAX
                    : (normalizedDelta < INT8_MIN) ? INT8_MIN
                                                   : static_cast<int8_t>(normalizedDelta);

    // Recalculer la position absolue directement à partir de la position physique totale
    // pour garantir une cohérence parfaite entre les encodeurs de différents PPR
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