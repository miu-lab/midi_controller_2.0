#pragma once
#include <Arduino.h>

#include "core/domain/IMidiOut.hpp"

/**
 * @brief Wrapper pour IMidiOut qui bufferise les messages et les envoie à intervalles réguliers
 *
 * Cette classe améliore les performances en évitant d'envoyer trop de messages MIDI
 * trop rapidement (ce qui peut causer des problèmes avec certains périphériques).
 */
class BufferedMidiOut : public IMidiOut {
public:
    // Types de messages MIDI
    enum class MessageType { CC, NOTE_ON, NOTE_OFF };

    struct MidiMessage {
        MessageType type;
        MidiChannel channel;
        uint8_t control;  // CC ou note
        uint8_t value;    // Valeur ou vélocité
        bool sent;        // Si le message a déjà été envoyé

        // Pour le tri efficace des messages
        bool operator==(const MidiMessage& other) const {
            return type == other.type && channel == other.channel && control == other.control;
        }
    };

    // Constructeur
    explicit BufferedMidiOut(IMidiOut& output, uint16_t bufferSize = 32);

    // Implémentation de IMidiOut
    void sendCc(MidiChannel ch, MidiCC cc, uint8_t value) override;
    void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) override;
    void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) override;

    // Méthodes spécifiques au buffer
    void flush();
    void clear();

    /**
     * @brief Met à jour le buffer et envoie les messages en attente
     *
     * Cette méthode doit être appelée régulièrement dans la boucle principale
     * pour envoyer les messages MIDI buffered.
     */
    void update();

private:
    IMidiOut& output_;

    static constexpr uint16_t DEFAULT_BUFFER_SIZE = 32;
    MidiMessage* buffer_;
    uint16_t bufferSize_;
    uint16_t nextIndex_;

    // Cherche un message existant ou un nouvel espace
    uint16_t findOrCreateMessage(MessageType type, MidiChannel ch, uint8_t control);
};
