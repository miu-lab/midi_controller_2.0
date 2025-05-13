#pragma once
#include "core/domain/events/core/Event.hpp"
#include "core/domain/events/core/EventTypes.hpp"

/**
 * @brief Types d'événements UI spécifiques pour l'affichage
 */
namespace UIDisplayEvents {
    constexpr EventType DisplayUpdateRequested = EventTypes::ScreenChange + 1;
    constexpr EventType ViewChanged = EventTypes::ScreenChange + 2;
    constexpr EventType ControlUpdated = EventTypes::ScreenChange + 3;
}

/**
 * @brief Événement de demande de mise à jour de l'affichage
 */
class DisplayUpdateRequestedEvent : public Event {
public:
    /**
     * @brief Constructeur
     */
    DisplayUpdateRequestedEvent()
        : Event(UIDisplayEvents::DisplayUpdateRequested, EventCategory::UI) {}
          
    const char* getEventName() const override { return "DisplayUpdateRequested"; }
};
