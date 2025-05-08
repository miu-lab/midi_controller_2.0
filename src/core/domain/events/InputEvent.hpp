#pragma once
#include "core/domain/types.hpp"

struct EncoderTurnedEvent {
    EncoderId id;
    int32_t absolutePosition;  // Seule la position absolue est transmise
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