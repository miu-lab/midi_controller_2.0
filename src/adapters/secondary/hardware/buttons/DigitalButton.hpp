// adapters/secondary/hardware/buttons/DigitalButton.hpp
#pragma once
#include <Arduino.h>
#include <Bounce2.h>

#include "adapters/secondary/hardware/buttons/DigitalButtonConfig.hpp"
#include "core/ports/input/ButtonPort.hpp"

/**
 * @brief Gestion d'un bouton numérique avec debounce.
 */
class DigitalButton : public ButtonPort {
public:
    explicit DigitalButton(const ButtonConfig& cfg);
    void update() override;
    bool isPressed() const override;
    ButtonId getId() const override;

    // Nouvelle méthode pour réinitialiser l'état des boutons en mode toggle
    void resetState();

private:
    ButtonConfig cfg_;
    Bounce button_;
    bool pressed_;      // État physique du bouton
    bool toggleState_;  // État logique pour les boutons en mode toggle
    bool prevPressed_;  // État précédent pour détecter les changements
};