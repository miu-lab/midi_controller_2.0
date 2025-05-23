#pragma once

#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/core/Event.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/ports/output/MidiOutputPort.hpp"
#include "core/utils/AppStrings.hpp"
#include "core/utils/FlashStrings.hpp"

/**
 * @brief Adaptateur MIDI qui émet des événements en plus de transmettre les messages MIDI
 *
 * Cette classe décore un MidiOutputPort pour ajouter l'émission d'événements
 * à chaque fois qu'un message MIDI est envoyé.
 */
class EventEnabledMidiOut : public MidiOutputPort {
public:
    /**
     * @brief Constructeur
     * @param basePort Port MIDI de base à décorer
     */
    explicit EventEnabledMidiOut(MidiOutputPort& basePort) : m_basePort(basePort) {
        char buffer[64];
        FlashStrings::copy(buffer, sizeof(buffer), DBG_EVENT_ENABLED_MIDI_OUT);
        Serial.println(buffer);
    }
    
    /**
     * @brief Vérifie si le port MIDI supporte les événements
     * @return Toujours true pour cette classe
     */
    bool supportsEvents() const override { return true; }

    /**
     * @brief Envoie un message MIDI Control Change et émet un événement
     * @param ch Canal MIDI (0-15)
     * @param cc Numéro de contrôleur (0-127)
     * @param value Valeur (0-127)
     * @param source Identifiant de la source du message (encodeur, bouton, etc.)
     */
    void sendCc(MidiChannel ch, MidiCC cc, uint8_t value, uint8_t source = 0) {
        // Log de débogage compact
        char buffer[64];
        FlashStrings::copy(buffer, sizeof(buffer), DBG_SENDING_CC);
        Serial.print(F("\n"));
        Serial.println(buffer);
        Serial.println();
        
        // Utiliser le format compact pour le débogage MIDI
        char labelBuffer[8];
        
        // Source ID
        FlashStrings::copy(buffer, sizeof(buffer), DBG_FMT);
        FlashStrings::copy(labelBuffer, sizeof(labelBuffer), DBG_SRC);
        Serial.printf(buffer, labelBuffer, source);
        Serial.println();
        
        // Channel
        FlashStrings::copy(labelBuffer, sizeof(labelBuffer), DBG_CH);
        Serial.printf(buffer, labelBuffer, ch);
        Serial.println();
        
        // CC
        FlashStrings::copy(labelBuffer, sizeof(labelBuffer), DBG_CC);
        Serial.printf(buffer, labelBuffer, cc);
        Serial.println();
        
        // Value
        FlashStrings::copy(labelBuffer, sizeof(labelBuffer), DBG_VAL);
        Serial.printf(buffer, labelBuffer, value);
        Serial.println();
        // Utiliser le séparateur optimisé
        FlashStrings::copy(buffer, sizeof(buffer), DBG_SEPARATOR);
        Serial.println(buffer);
        Serial.println("\n");

        // Envoyer le message MIDI via le port de base
        m_basePort.sendControlChange(ch, cc, value);

        // Émettre un événement MidiCC
        MidiCCEvent event(ch, cc, value, source);
        EventBus::getInstance().publish(event);
    }

    /**
     * @brief Envoie un message MIDI Control Change
     * @param ch Canal MIDI (0-15)
     * @param cc Numéro de contrôleur (0-127)
     * @param value Valeur (0-127)
     */
    void sendControlChange(MidiChannel ch, MidiCC cc, uint8_t value) override {
        sendCc(ch, cc, value, 0);
    }

    /**
     * @brief Envoie un message MIDI Note On et émet un événement
     * @param ch Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (1-127, 0 = Note Off)
     * @param source Identifiant de la source du message
     */
    void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity, uint8_t source = 0) {
        // Log de débogage compact
        char buffer[64];
        FlashStrings::copy(buffer, sizeof(buffer), DBG_SENDING_CC); // Réutiliser la même chaîne
        Serial.print(F("\n"));
        Serial.println(buffer);
        Serial.println();
        
        // Utiliser le format compact pour le débogage MIDI
        char labelBuffer[8];
        
        // Source ID
        FlashStrings::copy(buffer, sizeof(buffer), DBG_FMT);
        FlashStrings::copy(labelBuffer, sizeof(labelBuffer), DBG_SRC);
        Serial.printf(buffer, labelBuffer, source);
        Serial.println();
        
        // Channel
        FlashStrings::copy(labelBuffer, sizeof(labelBuffer), DBG_CH);
        Serial.printf(buffer, labelBuffer, ch);
        Serial.println();
        
        // Note
        Serial.print(F("Note:"));
        Serial.println(note);
        
        // Velocity
        Serial.print(F("Vel:"));
        Serial.println(velocity);
        
        // Utiliser le séparateur optimisé
        FlashStrings::copy(buffer, sizeof(buffer), DBG_SEPARATOR);
        Serial.println(buffer);
        Serial.println("\n");

        // Envoyer le message MIDI via le port de base
        m_basePort.sendNoteOn(ch, note, velocity);

        // Émettre un événement MidiNoteOn
        MidiNoteOnEvent event(ch, note, velocity, source);
        EventBus::getInstance().publish(event);
    }

    /**
     * @brief Envoie un message MIDI Note On
     * @param ch Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (1-127, 0 = Note Off)
     */
    void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) override {
        sendNoteOn(ch, note, velocity, 0);
    }

    /**
     * @brief Envoie un message MIDI Note Off et émet un événement
     * @param ch Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité de relâchement (généralement 0)
     * @param source Identifiant de la source du message
     */
    void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity, uint8_t source = 0) {
        // Log de débogage compact
        char buffer[64];
        FlashStrings::copy(buffer, sizeof(buffer), DBG_SENDING_CC); // Réutiliser la même chaîne
        Serial.print(F("\n"));
        Serial.println(buffer);
        Serial.println();
        
        // Utiliser le format compact pour le débogage MIDI
        char labelBuffer[8];
        
        // Source ID
        FlashStrings::copy(buffer, sizeof(buffer), DBG_FMT);
        FlashStrings::copy(labelBuffer, sizeof(labelBuffer), DBG_SRC);
        Serial.printf(buffer, labelBuffer, source);
        Serial.println();
        
        // Channel
        FlashStrings::copy(labelBuffer, sizeof(labelBuffer), DBG_CH);
        Serial.printf(buffer, labelBuffer, ch);
        Serial.println();
        
        // Note
        Serial.print(F("Note:"));
        Serial.println(note);
        
        // Velocity
        Serial.print(F("Vel:"));
        Serial.println(velocity);
        
        // Utiliser le séparateur optimisé
        FlashStrings::copy(buffer, sizeof(buffer), DBG_SEPARATOR);
        Serial.println(buffer);
        Serial.println("\n");

        // Envoyer le message MIDI via le port de base
        m_basePort.sendNoteOff(ch, note, velocity);

        // Émettre un événement MidiNoteOff
        MidiNoteOffEvent event(ch, note, velocity, source);
        EventBus::getInstance().publish(event);
    }

    /**
     * @brief Envoie un message MIDI Note Off
     * @param ch Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité de relâchement (généralement 0)
     */
    void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) override {
        sendNoteOff(ch, note, velocity, 0);
    }

    /**
     * @brief Envoie un message MIDI Program Change
     * @param ch Canal MIDI (0-15)
     * @param program Numéro de programme (0-127)
     */
    void sendProgramChange(MidiChannel ch, uint8_t program) override {
        m_basePort.sendProgramChange(ch, program);
        // Événement non implémenté pour le moment
    }

    /**
     * @brief Envoie un message MIDI Pitch Bend
     * @param ch Canal MIDI (0-15)
     * @param value Valeur du pitch bend (0-16383, 8192 = centre)
     */
    void sendPitchBend(MidiChannel ch, uint16_t value) override {
        m_basePort.sendPitchBend(ch, value);
        // Événement non implémenté pour le moment
    }

    /**
     * @brief Envoie un message MIDI Channel Pressure (Aftertouch)
     * @param ch Canal MIDI (0-15)
     * @param pressure Valeur de pression (0-127)
     */
    void sendChannelPressure(MidiChannel ch, uint8_t pressure) override {
        m_basePort.sendChannelPressure(ch, pressure);
        // Événement non implémenté pour le moment
    }

    /**
     * @brief Envoie un message MIDI System Exclusive
     * @param data Données SysEx (sans les octets F0 et F7)
     * @param length Longueur des données
     */
    void sendSysEx(const uint8_t* data, uint16_t length) override {
        m_basePort.sendSysEx(data, length);
        // Événement non implémenté pour le moment
    }

private:
    MidiOutputPort& m_basePort;  // Port MIDI de base
};