#pragma once

#include "core/domain/events/InputEvent.hpp"

// Type alias centralisé pour tous les types d'événements
namespace EventTypes {
    using EncoderTurned = EncoderTurnedEvent;
    using EncoderButton = EncoderButtonEvent;
    using ButtonPressed = ButtonPressed;
    using ButtonReleased = ButtonReleased;
}
