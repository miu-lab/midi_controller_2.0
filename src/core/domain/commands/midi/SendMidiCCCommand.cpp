#include "core/domain/commands/midi/SendMidiCCCommand.hpp"

#include <cstdio>  // Pour snprintf

SendMidiCCCommand::SendMidiCCCommand(IMidiOut& midiOut, uint8_t channel, uint8_t cc, uint8_t value)
    : midiOut_(midiOut),
      channel_(channel),
      cc_(cc),
      value_(value),
      previousValue_(0)  // Sera mis à jour lors de l'exécution
{}

void SendMidiCCCommand::execute() {
    // Si c'est la première exécution, essayer de lire la valeur actuelle
    // Cela permet d'avoir un état pour l'annulation
    if (!hasExecuted_) {
        // Idéalement, on devrait pouvoir lire la valeur actuelle du CC
        // Mais comme IMidiOut ne nous permet pas de le faire directement,
        // on pourrait avoir besoin d'un autre service pour cela

        // Pour l'instant, on suppose que previousValue_ = 0 si on ne peut pas le lire
        hasExecuted_ = true;
    }

    // Envoyer le CC MIDI
    midiOut_.sendCc(channel_, cc_, value_);
}

bool SendMidiCCCommand::undo() {
    if (!hasExecuted_) {
        return false;
    }

    // Restaurer la valeur précédente
    midiOut_.sendCc(channel_, cc_, previousValue_);
    return true;
}

bool SendMidiCCCommand::isUndoable() const {
    // Cette commande est théoriquement annulable, mais cela dépend
    // de notre capacité à connaître la valeur précédente
    return hasExecuted_;
}

const char* SendMidiCCCommand::getDescription() const {
    static char buffer[64];
    snprintf(buffer, sizeof(buffer), "Send MIDI CC: ch=%d cc=%d val=%d", channel_ + 1, cc_, value_);
    return buffer;
}
