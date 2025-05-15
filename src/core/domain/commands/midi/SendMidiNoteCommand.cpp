#include "core/domain/commands/midi/SendMidiNoteCommand.hpp"

#include <Arduino.h>

SendMidiNoteCommand::SendMidiNoteCommand(MidiOutputPort& midiOut, uint8_t channel, uint8_t note,
                                         uint8_t velocity, unsigned long duration)
    : midiOut_(&midiOut),
      channel_(channel),
      note_(note),
      velocity_(velocity),
      duration_(duration),
      startTime_(0),
      noteActive_(false),
      hasExecuted_(false) {}

void SendMidiNoteCommand::reset(MidiOutputPort& midiOut, uint8_t channel, uint8_t note,
                               uint8_t velocity, unsigned long duration) {
    midiOut_ = &midiOut;
    channel_ = channel;
    note_ = note;
    velocity_ = velocity;
    duration_ = duration;
    startTime_ = 0;
    noteActive_ = false;
    hasExecuted_ = false;
}

void SendMidiNoteCommand::execute() {
    if (!midiOut_) return; // Vérification de sécurité
    
    if (velocity_ > 0) {
        // Note On
        midiOut_->sendNoteOn(channel_, note_, velocity_);
        noteActive_ = true;

        // Si une durée est spécifiée, enregistrer le temps de début
        if (duration_ > 0) {
            startTime_ = millis();
        }
    } else {
        // Note Off
        midiOut_->sendNoteOff(channel_, note_, 0);
        noteActive_ = false;
    }

    hasExecuted_ = true;
}

bool SendMidiNoteCommand::undo() {
    if (!hasExecuted_ || !midiOut_) {
        return false;
    }

    if (noteActive_) {
        // Si la note est active, l'annuler en envoyant Note Off
        sendNoteOff();
        return true;
    } else if (velocity_ > 0) {
        // Si c'était une Note On qui est déjà terminée, on pourrait
        // théoriquement la rejouer, mais cela n'a généralement pas de sens musical
        return false;
    } else {
        // Si c'était une Note Off, on pourrait théoriquement envoyer une Note On,
        // mais sans connaître la vélocité d'origine, cela n'a pas vraiment de sens
        return false;
    }
}

bool SendMidiNoteCommand::isUndoable() const {
    // Une commande Note est annulable seulement si la note est encore active
    return noteActive_ && midiOut_ != nullptr;
}

const char* SendMidiNoteCommand::getDescription() const {
    static char buffer[64];
    if (velocity_ > 0) {
        snprintf(buffer,
                 sizeof(buffer),
                 "Note On: ch=%d note=%d vel=%d",
                 channel_ + 1,
                 note_,
                 velocity_);
    } else {
        snprintf(buffer, sizeof(buffer), "Note Off: ch=%d note=%d", channel_ + 1, note_);
    }
    return buffer;
}

void SendMidiNoteCommand::update() {
    if (!midiOut_) return; // Vérification de sécurité
    
    // Si la note est active et une durée est spécifiée
    if (noteActive_ && duration_ > 0) {
        unsigned long currentTime = millis();

        // Vérifier si la durée est écoulée
        if (currentTime - startTime_ >= duration_) {
            sendNoteOff();
        }
    }
}

bool SendMidiNoteCommand::isNoteActive() const {
    return noteActive_;
}

void SendMidiNoteCommand::sendNoteOff() {
    if (!midiOut_) return; // Vérification de sécurité
    
    midiOut_->sendNoteOff(channel_, note_, 0);
    noteActive_ = false;
}