#pragma once

#include <memory>

#include "core/domain/commands/Command.hpp"
#include "core/ports/output/MidiOutputPort.hpp"

/**
 * @brief Commande pour envoyer un message MIDI Note On/Off
 */
class SendMidiNoteCommand : public ICommand {
public:
    /**
     * @brief Constructeur
     * @param midiOut Interface de sortie MIDI
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (0-127), 0 pour Note Off
     * @param duration Durée de la note en ms, 0 pour une note qui reste active
     */
    SendMidiNoteCommand(MidiOutputPort& midiOut, uint8_t channel, uint8_t note, uint8_t velocity,
                        unsigned long duration = 0);

    /**
     * @brief Constructeur par défaut pour le pool d'objets
     */
    SendMidiNoteCommand() = default;

    /**
     * @brief Réinitialise la commande avec de nouveaux paramètres
     * @param midiOut Interface de sortie MIDI
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (0-127), 0 pour Note Off
     * @param duration Durée de la note en ms, 0 pour une note qui reste active
     */
    void reset(MidiOutputPort& midiOut, uint8_t channel, uint8_t note, uint8_t velocity,
               unsigned long duration = 0);

    /**
     * @brief Exécute la commande : envoie le message MIDI Note On
     * (et Note Off après duration si spécifié)
     */
    void execute() override;

    /**
     * @brief Annule la commande en envoyant un Note Off
     * @return true si la commande a été annulée, false sinon
     */
    bool undo() override;

    /**
     * @brief Vérifie si la commande peut être annulée
     * @return true si la commande est annulable, false sinon
     */
    bool isUndoable() const override;

    /**
     * @brief Obtient la description de la commande
     * @return Description textuelle de la commande
     */
    const char* getDescription() const override;

    /**
     * @brief Met à jour la commande et envoie Note Off si nécessaire
     * Doit être appelée régulièrement pour gérer les notes temporisées
     */
    void update();

    /**
     * @brief Vérifie si la note est encore active
     * @return true si la note est active, false sinon
     */
    bool isNoteActive() const;

private:
    MidiOutputPort* midiOut_ = nullptr;
    uint8_t channel_ = 0;
    uint8_t note_ = 0;
    uint8_t velocity_ = 0;
    unsigned long duration_ = 0;
    unsigned long startTime_ = 0;
    bool noteActive_ = false;
    bool hasExecuted_ = false;

    /**
     * @brief Envoie un Note Off pour cette note
     */
    void sendNoteOff();
};