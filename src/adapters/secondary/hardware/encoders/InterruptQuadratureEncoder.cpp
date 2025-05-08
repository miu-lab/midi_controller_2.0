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

    // Si pas de changement, retourner 0
    if (delta == 0) return 0;

    // Facteur de normalisation commun : 24 correspond au PPR standard des encodeurs mécaniques
    const int32_t REFERENCE_PPR = 24;

    // Calculer l'angle de rotation en unités de référence
    // (delta / ppr_) donne la fraction de tour
    // * REFERENCE_PPR convertit cette fraction en équivalent pour un encodeur de référence
    int32_t normalizedDelta = (delta * REFERENCE_PPR) / ppr_;

    // S'assurer qu'un mouvement physique réel produise toujours au moins 1 delta
    if (delta != 0 && normalizedDelta == 0) {
        normalizedDelta = (delta > 0) ? 1 : -1;
    }

    // Aucun facteur d'accélération appliqué - sensibilité constante

    // Mettre à jour la dernière position
    lastPosition_ = newPosition;

    // Mettre à jour la position physique totale
    physicalPosition_ += delta;

    // Calculer la position absolue directement à partir de la position physique totale
    // pour garantir une cohérence parfaite entre les encodeurs de différents PPR
    absolutePosition_ = (physicalPosition_ * REFERENCE_PPR) / ppr_;

    // Limiter le delta à la plage d'un int8_t
    int8_t result = 0;
    if (normalizedDelta > INT8_MAX)
        result = INT8_MAX;
    else if (normalizedDelta < INT8_MIN)
        result = INT8_MIN;
    else
        result = static_cast<int8_t>(normalizedDelta);

    // Débogage de niveau 1 (léger - pour les mouvements significatifs)
#if defined(DEBUG) && defined(DEBUG_RAW_CONTROLS) && (DEBUG_RAW_CONTROLS == 1) && (delta != 0)
    Serial.print("ENC_RAW ");
    Serial.print(id_);
    Serial.print(" raw:");
    Serial.print(delta);
    Serial.print(" ppr:");
    Serial.print(ppr_);
    Serial.print(" norm:");
    Serial.print(normalizedDelta);
    Serial.print(" result:");
    Serial.print(result);
    Serial.print(" abs_pos:");
    Serial.println(absolutePosition_);
#endif

    // Débogage de niveau 2 (complet - pour tous les appels)
#if defined(DEBUG) && defined(DEBUG_RAW_CONTROLS) && (DEBUG_RAW_CONTROLS >= 2)
    Serial.print("ENC_RAW ");
    Serial.print(id_);
    Serial.print(" raw:");
    Serial.print(delta);
    Serial.print(" norm:");
    Serial.print(normalizedDelta);
    Serial.print(" result:");
    Serial.print(result);
    Serial.print(" abs_pos:");
    Serial.println(absolutePosition_);
#endif

    return result;
}

bool InterruptQuadratureEncoder::isPressed() const {
    if (!hasButton_) return false;
    // Lire l'état du bouton
    int raw = digitalRead(buttonPin_);
    return activeLowButton_ ? (raw == LOW) : (raw == HIGH);
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