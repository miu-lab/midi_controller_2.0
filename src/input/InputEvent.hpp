#pragma once
#include "domain/types.hpp"

struct EncoderTurnedEvent {
    EncoderId id;
    int8_t delta;
};

struct EncoderButtonEvent {
    EncoderId id;
    bool pressed;
};

struct ButtonPressed {
    ButtonId id;
};

struct ButtonReleased {
    ButtonId id;
};