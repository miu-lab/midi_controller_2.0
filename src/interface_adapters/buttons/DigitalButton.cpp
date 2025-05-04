// interface_adapters/buttons/DigitalButton.cpp
#include "interface_adapters/buttons/DigitalButton.hpp"

DigitalButton::DigitalButton(const ButtonConfig& cfg)
    : cfg_(cfg)
    , button_()
    , pressed_(false)
{
    pinMode(cfg_.pin, cfg_.activeLow ? INPUT_PULLUP : INPUT);
    button_.attach(cfg_.pin, cfg_.activeLow ? INPUT_PULLUP : INPUT);
    button_.interval(5); // debounce interval (ms)
}

void DigitalButton::update() {
    button_.update();
    int raw = digitalRead(cfg_.pin);
    pressed_ = cfg_.activeLow ? (raw == LOW) : (raw == HIGH);
}

bool DigitalButton::isPressed() const {
    return pressed_;
}

ButtonId DigitalButton::getId() const {
    return cfg_.id;
}