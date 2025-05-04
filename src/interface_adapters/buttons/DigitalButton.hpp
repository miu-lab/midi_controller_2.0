// interface_adapters/buttons/DigitalButton.hpp
#pragma once
#include "domain/IButton.hpp"
#include "interface_adapters/buttons/DigitalButtonConfig.hpp"
#include <Bounce2.h>
#include <Arduino.h>

/**
 * @brief Gestion d'un bouton numérique avec debounce.
 */
class DigitalButton : public IButton {
public:
    explicit DigitalButton(const ButtonConfig& cfg);
    void update() override;
    bool isPressed() const override;
    ButtonId getId() const override;

private:
    ButtonConfig cfg_;
    Bounce       button_;
    bool         pressed_;
};