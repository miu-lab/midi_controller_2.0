#pragma once

#include "core/domain/events/core/Event.hpp"
#include "core/domain/events/core/EventTypes.hpp"

/**
 * @brief Événement haute priorité pour les changements d'encodeur (chemin critique MIDI)
 */
class HighPriorityEncoderChangedEvent : public Event {
public:
    /**
     * @brief Constructeur optimisé pour les performances
     * @param encoderId Identifiant de l'encodeur
     * @param position Position absolue actuelle
     * @param delta Changement relatif
     */
    HighPriorityEncoderChangedEvent(uint16_t encoderId, int32_t position, int32_t delta)
        : Event(EventTypes::HighPriorityEncoderChanged, EventCategory::MIDI),
          encoderId(encoderId), position(position), delta(delta) {}
          
    virtual const char* getEventName() const override { return "HighPriorityEncoderChanged"; }
    
    uint16_t encoderId;  // Identifiant de l'encodeur
    int32_t position;   // Position absolue
    int32_t delta;      // Changement relatif
};


/**
 * @brief Événement haute priorité pour les pressions de bouton (chemin critique MIDI)
 */
class HighPriorityButtonPressEvent : public Event {
public:
    /**
     * @brief Constructeur optimisé pour les performances
     * @param buttonId Identifiant du bouton
     * @param pressed État du bouton (pressé ou relâché)
     */
    HighPriorityButtonPressEvent(uint16_t buttonId, bool pressed)
        : Event(EventTypes::HighPriorityButtonPress, EventCategory::MIDI),
          buttonId(buttonId), pressed(pressed) {}
          
    virtual const char* getEventName() const override { return "HighPriorityButtonPress"; }
    
    uint16_t buttonId;   // Identifiant du bouton
    bool pressed;       // État du bouton
};


/**
 * @brief Événement émis lorsqu'un message MIDI CC est envoyé
 */
class MidiCCEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param channel Canal MIDI (0-15)
     * @param controller Numéro de contrôleur (0-127)
     * @param value Valeur (0-127)
     * @param source Identifiant de la source (encodeur, bouton, etc.)
     */
    MidiCCEvent(uint8_t channel, uint8_t controller, uint8_t value, uint8_t source = 0)
        : Event(EventTypes::MidiControlChange, EventCategory::MIDI),
          channel(channel), controller(controller), value(value), source(source) {}
          
    virtual const char* getEventName() const override { return "MidiControlChange"; }
    
    uint8_t channel;      // Canal MIDI
    uint8_t controller;   // Numéro de contrôleur
    uint8_t value;        // Valeur
    uint8_t source;       // Source du message (ID d'encodeur, bouton, etc.)
};

/**
 * @brief Événement émis lorsqu'un message MIDI Note On est envoyé
 */
class MidiNoteOnEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (0-127)
     * @param source Identifiant de la source (encodeur, bouton, etc.)
     */
    MidiNoteOnEvent(uint8_t channel, uint8_t note, uint8_t velocity, uint8_t source = 0)
        : Event(EventTypes::MidiNoteOn, EventCategory::MIDI),
          channel(channel), note(note), velocity(velocity), source(source) {}
          
    virtual const char* getEventName() const override { return "MidiNoteOn"; }
    
    uint8_t channel;     // Canal MIDI
    uint8_t note;        // Numéro de note
    uint8_t velocity;    // Vélocité
    uint8_t source;      // Source du message
};

/**
 * @brief Événement émis lorsqu'un message MIDI Note Off est envoyé
 */
class MidiNoteOffEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité de relâchement (0-127)
     * @param source Identifiant de la source (encodeur, bouton, etc.)
     */
    MidiNoteOffEvent(uint8_t channel, uint8_t note, uint8_t velocity, uint8_t source = 0)
        : Event(EventTypes::MidiNoteOff, EventCategory::MIDI),
          channel(channel), note(note), velocity(velocity), source(source) {}
          
    virtual const char* getEventName() const override { return "MidiNoteOff"; }
    
    uint8_t channel;     // Canal MIDI
    uint8_t note;        // Numéro de note
    uint8_t velocity;    // Vélocité
    uint8_t source;      // Source du message
};

/**
 * @brief Événement émis lorsqu'un mapping MIDI est activé
 */
class MidiMappingEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param controlId Identifiant du contrôle (encodeur, bouton)
     * @param midiType Type de message MIDI (CC, Note, etc.)
     * @param midiChannel Canal MIDI (0-15)
     * @param midiNumber Numéro MIDI (CC, Note, etc.)
     * @param midiValue Valeur MIDI
     */
    MidiMappingEvent(uint8_t controlId, uint8_t midiType, uint8_t midiChannel, 
                     uint8_t midiNumber, uint8_t midiValue)
        : Event(EventTypes::MidiMapping, EventCategory::MIDI),
          controlId(controlId), midiType(midiType), midiChannel(midiChannel),
          midiNumber(midiNumber), midiValue(midiValue) {}
          
    virtual const char* getEventName() const override { return "MidiMapping"; }
    
    uint8_t controlId;     // ID du contrôle
    uint8_t midiType;      // Type de message MIDI
    uint8_t midiChannel;   // Canal MIDI
    uint8_t midiNumber;    // Numéro MIDI (CC, Note)
    uint8_t midiValue;     // Valeur MIDI
};
