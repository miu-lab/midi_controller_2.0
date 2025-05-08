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

struct ButtonPressedEvent {
    ButtonId id;
};

struct ButtonReleasedEvent {
    ButtonId id;
};