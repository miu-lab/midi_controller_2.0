#pragma once

#include "core/domain/events/core/Event.hpp"

/**
 * @brief Identifiants des types d'événements spécifiques
 */
namespace EventTypes {
    // Types d'événements d'entrée - plage 1-999
    constexpr EventType EncoderTurned = 1;
    constexpr EventType EncoderButton = 2;
    constexpr EventType ButtonPressed = 3;
    constexpr EventType ButtonReleased = 4;
    
    // Types d'événements UI - plage 1000-1999
    constexpr EventType ScreenChange = 1000;
    constexpr EventType MenuNavigation = 1001;
    constexpr EventType MenuSelection = 1002;
    constexpr EventType DialogShow = 1003;
    constexpr EventType DialogClose = 1004;
    
    // Types d'événements MIDI - plage 2000-2999
    constexpr EventType MidiNoteOn = 2000;
    constexpr EventType MidiNoteOff = 2001;
    constexpr EventType MidiControlChange = 2002;
    constexpr EventType MidiProgramChange = 2003;
    constexpr EventType MidiPitchBend = 2004;
    constexpr EventType MidiMapping = 2005;
    
    // Types d'événements MIDI haute performance (chemin critique) - plage 2500-2599
    constexpr EventType HighPriorityEncoderChanged = 2500;
    constexpr EventType HighPriorityEncoderButton = 2501;
    constexpr EventType HighPriorityButtonPress = 2502;
    
    // Types d'événements système - plage 3000-3999
    constexpr EventType SystemStartup = 3000;
    constexpr EventType SystemShutdown = 3001;
    constexpr EventType SystemError = 3002;
};

/**
 * @brief Événement de rotation d'encodeur
 */
class EncoderTurnedEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param id Identifiant de l'encodeur
     * @param position Position absolue de l'encodeur
     * @param delta Changement relatif (peut être négatif)
     */
    EncoderTurnedEvent(uint8_t id, int32_t position, int8_t delta)
        : Event(EventTypes::EncoderTurned, EventCategory::Input),
          id(id), position(position), delta(delta) {}
          
    virtual const char* getEventName() const override { return "EncoderTurned"; }
    
    uint8_t id;        // Identifiant de l'encodeur
    int32_t position;  // Position absolue
    int8_t delta;      // Changement relatif
};

/**
 * @brief Événement de bouton d'encodeur
 */
class EncoderButtonEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param id Identifiant de l'encodeur
     * @param pressed true si le bouton est pressé, false sinon
     */
    EncoderButtonEvent(uint8_t id, bool pressed)
        : Event(EventTypes::EncoderButton, EventCategory::Input),
          id(id), pressed(pressed) {}
          
    virtual const char* getEventName() const override { return "EncoderButton"; }
    
    uint8_t id;     // Identifiant de l'encodeur
    bool pressed;   // État du bouton
};

/**
 * @brief Événement de bouton pressé
 */
class ButtonPressedEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param id Identifiant du bouton
     */
    ButtonPressedEvent(uint8_t id)
        : Event(EventTypes::ButtonPressed, EventCategory::Input), id(id) {}
          
    virtual const char* getEventName() const override { return "ButtonPressed"; }
    
    uint8_t id;   // Identifiant du bouton
};

/**
 * @brief Événement de bouton relâché
 */
class ButtonReleasedEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param id Identifiant du bouton
     */
    ButtonReleasedEvent(uint8_t id)
        : Event(EventTypes::ButtonReleased, EventCategory::Input), id(id) {}
          
    virtual const char* getEventName() const override { return "ButtonReleased"; }
    
    uint8_t id;   // Identifiant du bouton
};
