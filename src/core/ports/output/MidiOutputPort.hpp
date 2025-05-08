#pragma once

#include "core/domain/types.hpp"

/**
 * @brief Port de sortie pour les messages MIDI
 *
 * Cette interface définit le contrat pour l'envoi de messages MIDI.
 */
class MidiOutputPort {
public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~MidiOutputPort() = default;

    /**
     * @brief Envoie un message MIDI Control Change
     * @param ch Canal MIDI (0-15)
     * @param cc Numéro de contrôleur (0-127)
     * @param value Valeur (0-127)
     */
    virtual void sendCc(MidiChannel ch, MidiCC cc, uint8_t value) = 0;

    /**
     * @brief Envoie un message MIDI Note On
     * @param ch Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (1-127, 0 = Note Off)
     */
    virtual void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) = 0;

    /**
     * @brief Envoie un message MIDI Note Off
     * @param ch Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité de relâchement (généralement 0)
     */
    virtual void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) = 0;

    /**
     * @brief Envoie un message MIDI Program Change
     * @param ch Canal MIDI (0-15)
     * @param program Numéro de programme (0-127)
     */
    virtual void sendProgramChange(MidiChannel ch, uint8_t program) = 0;

    /**
     * @brief Envoie un message MIDI Pitch Bend
     * @param ch Canal MIDI (0-15)
     * @param value Valeur du pitch bend (0-16383, 8192 = centre)
     */
    virtual void sendPitchBend(MidiChannel ch, uint16_t value) = 0;

    /**
     * @brief Envoie un message MIDI Channel Pressure (Aftertouch)
     * @param ch Canal MIDI (0-15)
     * @param pressure Valeur de pression (0-127)
     */
    virtual void sendChannelPressure(MidiChannel ch, uint8_t pressure) = 0;

    /**
     * @brief Envoie un message MIDI System Exclusive
     * @param data Données SysEx (sans les octets F0 et F7)
     * @param length Longueur des données
     */
    virtual void sendSysEx(const uint8_t* data, uint16_t length) = 0;
};
