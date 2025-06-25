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
    // Utiliser le type défini dans EventTypes
    constexpr EventType UIParameterUpdate = EventTypes::UIParameterUpdate;
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

/**
 * @brief Événement UI pour mise à jour de paramètre optimisé (batchés)
 * 
 * Cet événement est généré par le système de batching à partir des événements MIDI bruts,
 * avec une fréquence limitée pour éviter la surcharge de l'UI.
 */
class UIParameterUpdateEvent : public Event {
public:
    UIParameterUpdateEvent(uint8_t controller, uint8_t channel, uint8_t value, const String& name = "")
        : Event(UIDisplayEvents::UIParameterUpdate, EventCategory::UI),
          controller(controller), channel(channel), value(value), parameter_name(name) {}

    const char* getEventName() const override { return "UIParameterUpdate"; }

    const uint8_t controller;     ///< Numéro du contrôleur CC (0-127)
    const uint8_t channel;        ///< Canal MIDI (0-15)
    const uint8_t value;          ///< Valeur du paramètre (0-127)
    const String parameter_name;  ///< Nom du paramètre (optionnel)
};
