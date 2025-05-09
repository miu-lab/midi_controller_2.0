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
    explicit BufferedMidiOut(IMidiOut& output, uint16_t bufferSize = DEFAULT_BUFFER_SIZE);
    ~BufferedMidiOut();

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
     * 
     * @param maxMessages Nombre maximum de messages à envoyer par cycle (0 = tous)
     * @return Nombre de messages envoyés
     */
    uint16_t update(uint16_t maxMessages = 8);

    /**
     * @brief Renvoie le nombre de messages en attente dans le buffer
     */
    uint16_t getPendingCount() const { return dirtyCount_; }

    /**
     * @brief Accélère les envois en temporisant moins entre messages
     */
    void setHighPriority(bool highPriority) { highPriority_ = highPriority; }

private:
    IMidiOut& output_;

    // Taille par défaut du buffer et de la table de hachage
    static constexpr uint16_t DEFAULT_BUFFER_SIZE = 64;
    static constexpr uint16_t HASH_TABLE_SIZE = 128;  // Puissance de 2 pour optimiser le hachage
    static constexpr uint16_t INVALID_INDEX = UINT16_MAX;
    static constexpr uint16_t STATIC_BUFFER_SIZE = 32; // Taille du buffer statique
    
    MidiMessage* buffer_;                   // Buffer de messages MIDI
    MidiMessage staticBuffer_[STATIC_BUFFER_SIZE]; // Buffer statique pour éviter l'allocation dynamique
    uint16_t bufferSize_;                   // Taille du buffer
    uint16_t nextIndex_;                    // Prochain index à utiliser dans le buffer
    uint16_t dirtyCount_;                   // Nombre de messages à envoyer
    uint16_t hashTable_[HASH_TABLE_SIZE];   // Table de hachage pour un accès rapide
    bool highPriority_;                     // Mode haute priorité (moins de temporisation)
    bool usingDynamicBuffer_;                // Si nous utilisons un buffer dynamique

    // Fonction de hachage simple et efficace pour les messages MIDI
    inline uint16_t hashMessage(MessageType type, MidiChannel ch, uint8_t control) const {
        // Fonction de hachage améliorée : meilleure distribution
        // FNV-1a hash function adaptée pour les messages MIDI
        constexpr uint32_t FNV_PRIME = 16777619;
        constexpr uint32_t FNV_OFFSET_BASIS = 2166136261;
        
        uint32_t hash = FNV_OFFSET_BASIS;
        hash ^= static_cast<uint8_t>(type);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint8_t>(ch);
        hash *= FNV_PRIME;
        hash ^= control;
        hash *= FNV_PRIME;
        
        // Masque pour obtenir un index dans la table de hachage
        return hash & (HASH_TABLE_SIZE - 1);
    }
    
    // Cherche un message existant ou un nouvel espace dans le buffer (avec table de hachage)
    uint16_t findOrCreateMessage(MessageType type, MidiChannel ch, uint8_t control);
    
    // Optimise le buffer en libérant les espaces des messages envoyés
    void optimizeBuffer();
};
