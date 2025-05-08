#include "adapters/secondary/midi/BufferedMidiOut.hpp"

BufferedMidiOut::BufferedMidiOut(IMidiOut& output, uint16_t bufferSize)
    : output_(output)
    , bufferSize_(bufferSize > 0 ? bufferSize : DEFAULT_BUFFER_SIZE)
    , nextIndex_(0)
{
    // Allouer le buffer
    buffer_ = new MidiMessage[bufferSize_];
    
    // Initialiser tous les messages comme non envoyés
    clear();
}

void BufferedMidiOut::sendCc(MidiChannel ch, MidiCC cc, uint8_t value) {
    uint16_t idx = findOrCreateMessage(MessageType::CC, ch, cc);
    buffer_[idx].value = value;
    buffer_[idx].sent = false;
}

void BufferedMidiOut::sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) {
    uint16_t idx = findOrCreateMessage(MessageType::NOTE_ON, ch, note);
    buffer_[idx].value = velocity;
    buffer_[idx].sent = false;
}

void BufferedMidiOut::sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) {
    uint16_t idx = findOrCreateMessage(MessageType::NOTE_OFF, ch, note);
    buffer_[idx].value = velocity;
    buffer_[idx].sent = false;
}

void BufferedMidiOut::flush() {
    // Envoyer tous les messages qui n'ont pas encore été envoyés
    for (uint16_t i = 0; i < bufferSize_; i++) {
        if (!buffer_[i].sent) {
            // Envoyer ce message
            switch (buffer_[i].type) {
                case MessageType::CC:
                    output_.sendCc(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                    break;
                case MessageType::NOTE_ON:
                    output_.sendNoteOn(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                    break;
                case MessageType::NOTE_OFF:
                    output_.sendNoteOff(buffer_[i].channel, buffer_[i].control, buffer_[i].value);
                    break;
            }
            
            // Marquer comme envoyé
            buffer_[i].sent = true;
        }
    }
}

void BufferedMidiOut::clear() {
    for (uint16_t i = 0; i < bufferSize_; i++) {
        buffer_[i].sent = true; // Marquer comme "déjà envoyé" = ne pas envoyer
    }
    nextIndex_ = 0;
}

uint16_t BufferedMidiOut::findOrCreateMessage(MessageType type, MidiChannel ch, uint8_t control) {
    // Créer un message temporaire pour la recherche
    MidiMessage temp = {
        .type = type,
        .channel = ch,
        .control = control,
        .value = 0,
        .sent = false
    };
    
    // Chercher un message existant avec le même type, canal et contrôle
    for (uint16_t i = 0; i < bufferSize_; i++) {
        if (buffer_[i] == temp) {
            return i;
        }
    }
    
    // Pas trouvé, créer un nouveau message
    if (nextIndex_ < bufferSize_) {
        uint16_t idx = nextIndex_++;
        buffer_[idx] = temp;
        return idx;
    }
    
    // Buffer plein, réutiliser le premier emplacement
    buffer_[0] = temp;
    return 0;
}
