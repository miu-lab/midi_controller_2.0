
// interface_adapters/encoders/InterruptQuadratureEncoder.cpp
#ifdef UNIT_TEST_NATIVE
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include "interface_adapters/encoders/InterruptQuadratureEncoder.hpp"

// Stocke instances pour ISR
std::vector<InterruptQuadratureEncoder*>& InterruptQuadratureEncoder::instances() {
    static std::vector<InterruptQuadratureEncoder*> inst;
    return inst;
}

InterruptQuadratureEncoder::InterruptQuadratureEncoder(const EncoderConfig& cfg)
    : id_(cfg.id)
    , pinA_(cfg.pinA)
    , pinB_(cfg.pinB)
    , count_(0)
    , position_(0)
    , lastAB_((uint8_t)((digitalRead(pinA_)<<1) | digitalRead(pinB_)))
    , ppr_(cfg.ppr)
    , hasButton_(cfg.hasButton)
    , btnCfg_{static_cast<ButtonId>(cfg.id), cfg.pinButton, cfg.activeLowButton}
    , lastPublishUs_(0)
{
    pinMode(pinA_, INPUT_PULLUP);
    pinMode(pinB_, INPUT_PULLUP);

    // Enregistre pour ISR
    instances().push_back(this);
    attachInterrupt(digitalPinToInterrupt(pinA_), handleAllInterrupts, CHANGE);
}

InterruptQuadratureEncoder::~InterruptQuadratureEncoder() {
    detachInterrupt(digitalPinToInterrupt(pinA_));
    auto &inst = instances();
    inst.erase(std::remove(inst.begin(), inst.end(), this), inst.end());
}

void InterruptQuadratureEncoder::handleAllInterrupts() {
    for (auto *enc : instances()) {
        // Double lecture pour filtrer rebonds
        uint8_t a1 = digitalRead(enc->pinA_);
        delayMicroseconds(50);
        uint8_t a2 = digitalRead(enc->pinA_);
        if (a1 != a2) continue;
        uint8_t b = digitalRead(enc->pinB_);
        uint8_t newAB = (uint8_t)((a1 << 1) | b);
        enc->handleInterrupt(newAB);
    }
}

void InterruptQuadratureEncoder::handleInterrupt(uint8_t newAB) {
    if (newAB != lastAB_) {
        // Gray-code: incr/decr
        bool positive = (lastAB_ == 0 && newAB == 1) ||
                        (lastAB_ == 1 && newAB == 3) ||
                        (lastAB_ == 3 && newAB == 2) ||
                        (lastAB_ == 2 && newAB == 0);
        count_ += positive ? 1 : -1;
        lastAB_ = newAB;
    }
}

int8_t InterruptQuadratureEncoder::readDelta() {
    noInterrupts();
    int16_t val = count_;
    count_ = 0;
    interrupts();
    if (val == 0) return 0;
    uint32_t now = micros();
    if ((uint32_t)(now - lastPublishUs_) < debounceUs_) {
        return 0;
    }
    lastPublishUs_ = now;
    if      (val > INT8_MAX) val = INT8_MAX;
    else if (val < INT8_MIN) val = INT8_MIN;
    return static_cast<int8_t>(val);
}

bool InterruptQuadratureEncoder::isPressed() const {
    if (!hasButton_) return false;
    int raw = digitalRead(btnCfg_.pin);          // Debug visuel
    return btnCfg_.activeLow ? (raw == LOW) : (raw == HIGH);
}

EncoderId InterruptQuadratureEncoder::getId() const { return id_; }
uint16_t InterruptQuadratureEncoder::getPpr() const { return ppr_; }
