// MechanicalEncoder impl.

#include "interface_adapters/encoders/MechanicalEncoder.hpp"

MechanicalEncoder::MechanicalEncoder(const MechanicalEncoderConfig& cfg)
    : id_(cfg.id)
    , pinA_(cfg.pinA)
    , pinB_(cfg.pinB)
    , pinSwitch_(cfg.pinSwitch)
    , activeLowSwitch_(cfg.activeLowSwitch)
{
    pinMode(pinA_, INPUT_PULLUP);
    pinMode(pinB_, INPUT_PULLUP);
    pinMode(pinSwitch_, activeLowSwitch_ ? INPUT_PULLUP : INPUT);
    lastState_ = (digitalRead(pinA_) << 1) | digitalRead(pinB_);
}

int8_t MechanicalEncoder::readDelta() {
    uint8_t state = (digitalRead(pinA_) << 1) | digitalRead(pinB_);
    int8_t delta = 0;
    if (state != lastState_) {
        // Séquence Gray code : compte +1 ou -1
        if ((lastState_ == 0 && state == 1) ||
            (lastState_ == 1 && state == 3) ||
            (lastState_ == 3 && state == 2) ||
            (lastState_ == 2 && state == 0)) {
            delta = 1;
        } else if ((lastState_ == 0 && state == 2) ||
                   (lastState_ == 2 && state == 3) ||
                   (lastState_ == 3 && state == 1) ||
                   (lastState_ == 1 && state == 0)) {
            delta = -1;
        }
    }
    lastState_ = state;
    return delta;
}

bool MechanicalEncoder::isPressed() const {
    int raw = digitalRead(pinSwitch_);
    return activeLowSwitch_ ? (raw == LOW) : (raw == HIGH);
}

EncoderId MechanicalEncoder::getId() const {
    return id_;
}
