#pragma once

#include <memory>

#include "core/domain/IMidiOut.hpp"
#include "core/domain/commands/Command.hpp"

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
     */
    SendMidiCCCommand(IMidiOut& midiOut, uint8_t channel, uint8_t cc, uint8_t value);

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
    IMidiOut& midiOut_;
    uint8_t channel_;
    uint8_t cc_;
    uint8_t value_;
    uint8_t previousValue_;  // Pour l'annulation
    bool hasExecuted_ = false;
};
