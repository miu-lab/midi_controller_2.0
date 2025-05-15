#pragma once

#include <memory>

#include "core/domain/commands/Command.hpp"
#include "core/ports/output/MidiOutputPort.hpp"

/**
 * @brief Commande pour envoyer un message MIDI CC (Control Change)
 */
class SendMidiCCCommand : public ICommand {
public:
    /**
    * @brief Constructeur
    * @param midiOut Interface de sortie MIDI
    * @param channel Canal MIDI (0-15)
    * @param cc Numéro de contrôleur (0-127)
    * @param value Valeur (0-127)
    * @param source ID de la source (encodeur, bouton, etc.)
    */
    SendMidiCCCommand(MidiOutputPort& midiOut, uint8_t channel, uint8_t cc, uint8_t value, uint8_t source = 0);

    /**
     * @brief Constructeur par défaut pour le pool d'objets
     */
    SendMidiCCCommand() = default;

    /**
     * @brief Réinitialise la commande avec de nouveaux paramètres
     * @param midiOut Interface de sortie MIDI
     * @param channel Canal MIDI (0-15)
     * @param cc Numéro de contrôleur (0-127)
     * @param value Valeur (0-127)
     * @param source ID de la source (encodeur, bouton, etc.)
     */
    void reset(MidiOutputPort& midiOut, uint8_t channel, uint8_t cc, uint8_t value, uint8_t source = 0);

    /**
     * @brief Exécute la commande : envoie le message MIDI CC
     */
    void execute() override;

    /**
     * @brief Annule la commande (si possible)
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

private:
    MidiOutputPort* midiOut_ = nullptr;
    uint8_t channel_ = 0;
    uint8_t cc_ = 0;
    uint8_t value_ = 0;
    uint8_t source_ = 0; // ID de la source (encodeur, bouton, etc.)
    uint8_t previousValue_ = 0;  // Pour l'annulation
    bool hasExecuted_ = false;
};