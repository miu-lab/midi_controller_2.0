#include "core/domain/commands/midi/SendMidiCCCommand.hpp"

#include <cstdio>  // Pour snprintf


SendMidiCCCommand::SendMidiCCCommand(MidiOutputPort& midiOut, uint8_t channel, uint8_t cc,
                                     uint8_t value, uint8_t source)
    : midiOut_(&midiOut),
      channel_(channel),
      cc_(cc),
      value_(value),
      source_(source),
      previousValue_(0),  // Sera mis à jour lors de l'exécution
      hasExecuted_(false) {
    // TODO DEBUG MSG
}

void SendMidiCCCommand::reset(MidiOutputPort& midiOut, uint8_t channel, uint8_t cc, uint8_t value,
                              uint8_t source) {
    midiOut_ = &midiOut;
    channel_ = channel;
    cc_ = cc;
    value_ = value;
    source_ = source;
    previousValue_ = 0;
    hasExecuted_ = false;
}

void SendMidiCCCommand::execute() {
    if (!midiOut_) return;  // Vérification de sécurité

    // Si c'est la première exécution, essayer de lire la valeur actuelle
    // Cela permet d'avoir un état pour l'annulation
    if (!hasExecuted_) {
        // Idéalement, on devrait pouvoir lire la valeur actuelle du CC
        // Mais comme MidiOutputPort ne nous permet pas de le faire directement,
        // on pourrait avoir besoin d'un autre service pour cela

        // Pour l'instant, on suppose que previousValue_ = 0 si on ne peut pas le lire
        hasExecuted_ = true;
    }

    // Vérifier si le port supporte les événements via la méthode virtuelle
    if (midiOut_->supportsEvents()) {
        // Utiliser l'interface étendue avec l'ID de source
        midiOut_->sendCc(channel_, cc_, value_, source_);
    } else {
        // Utiliser l'interface standard sans l'ID de source
        midiOut_->sendControlChange(channel_, cc_, value_);
    }
}

bool SendMidiCCCommand::undo() {
    if (!hasExecuted_ || !midiOut_) {
        return false;
    }

    // Restaurer la valeur précédente
    midiOut_->sendControlChange(channel_, cc_, previousValue_);
    return true;
}

bool SendMidiCCCommand::isUndoable() const {
    // Cette commande est théoriquement annulable, mais cela dépend
    // de notre capacité à connaître la valeur précédente
    return hasExecuted_ && midiOut_ != nullptr;
}

const char* SendMidiCCCommand::getDescription() const {
    static char buffer[80];
    snprintf(buffer,
             sizeof(buffer),
             "Send MIDI CC: source=%d ch=%d cc=%d val=%d",
             source_,
             channel_ + 1,
             cc_,
             value_);
    return buffer;
}