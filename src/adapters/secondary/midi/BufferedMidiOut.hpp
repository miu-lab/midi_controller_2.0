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
        uint16_t hashNext; // Index du prochain message dans la même entrée de hachage (ou UINT16_MAX)

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

    // Taille par défaut du buffer et de la table de hachage
    static constexpr uint16_t DEFAULT_BUFFER_SIZE = 32;
    static constexpr uint16_t HASH_TABLE_SIZE = 64;  // Puissance de 2 pour optimiser le hachage
    static constexpr uint16_t INVALID_INDEX = UINT16_MAX;
    
    MidiMessage* buffer_;                   // Buffer de messages MIDI
    uint16_t bufferSize_;                   // Taille du buffer
    uint16_t nextIndex_;                    // Prochain index à utiliser dans le buffer
    uint16_t dirtyCount_;                   // Nombre de messages à envoyer
    uint16_t hashTable_[HASH_TABLE_SIZE];   // Table de hachage pour un accès rapide

    // Fonction de hachage simple et efficace pour les messages MIDI
    uint16_t hashMessage(MessageType type, MidiChannel ch, uint8_t control) const {
        // Combinaison des composants en une seule valeur, puis modulo par la taille de la table
        uint16_t hash = ((static_cast<uint16_t>(type) << 12) | 
                        (static_cast<uint16_t>(ch) << 8) | 
                        control) & (HASH_TABLE_SIZE - 1);
        return hash;
    }
    
    // Cherche un message existant ou un nouvel espace dans le buffer (avec table de hachage)
    uint16_t findOrCreateMessage(MessageType type, MidiChannel ch, uint8_t control);
    
    // Optimise le buffer en libérant les espaces des messages envoyés
    void optimizeBuffer();
};
