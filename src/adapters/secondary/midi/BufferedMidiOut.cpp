#include "adapters/secondary/midi/BufferedMidiOut.hpp"

BufferedMidiOut::BufferedMidiOut(IMidiOut& output, uint16_t bufferSize)
    : output_(output),
      bufferSize_(bufferSize > 0 ? bufferSize : DEFAULT_BUFFER_SIZE),
      nextIndex_(0) {
    buffer_ = new MidiMessage[bufferSize_];
    clear();
}

void BufferedMidiOut::sendCc(MidiChannel ch, MidiCC cc, uint8_t value) {
    uint16_t index = findOrCreateMessage(MessageType::CC, ch, cc);
    buffer_[index].value = value;
    buffer_[index].sent = false;
}

void BufferedMidiOut::sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) {
    uint16_t index = findOrCreateMessage(MessageType::NOTE_ON, ch, note);
    buffer_[index].value = velocity;
    buffer_[index].sent = false;
}

void BufferedMidiOut::sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) {
    uint16_t index = findOrCreateMessage(MessageType::NOTE_OFF, ch, note);
    buffer_[index].value = velocity;
    buffer_[index].sent = false;
}

void BufferedMidiOut::flush() {
    for (uint16_t i = 0; i < bufferSize_; i++) {
        if (!buffer_[i].sent) {
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
            buffer_[i].sent = true;
        }
    }
}

void BufferedMidiOut::clear() {
    for (uint16_t i = 0; i < bufferSize_; i++) {
        buffer_[i].type = MessageType::CC;
        buffer_[i].channel = 0;
        buffer_[i].control = 0;
        buffer_[i].value = 0;
        buffer_[i].sent = true;
    }
    nextIndex_ = 0;
}

uint16_t BufferedMidiOut::findOrCreateMessage(MessageType type, MidiChannel ch, uint8_t control) {
    // Chercher si ce message existe déjà
    for (uint16_t i = 0; i < bufferSize_; i++) {
        if (buffer_[i].type == type && buffer_[i].channel == ch && buffer_[i].control == control) {
            return i;
        }
    }

    // Chercher un emplacement libre
    for (uint16_t i = 0; i < bufferSize_; i++) {
        if (buffer_[i].sent) {
            buffer_[i].type = type;
            buffer_[i].channel = ch;
            buffer_[i].control = control;
            return i;
        }
    }

    // Si tout est occupé, utiliser l'index suivant (stratégie circulaire)
    uint16_t index = nextIndex_;
    nextIndex_ = (nextIndex_ + 1) % bufferSize_;

    buffer_[index].type = type;
    buffer_[index].channel = ch;
    buffer_[index].control = control;

    return index;
}

void BufferedMidiOut::update() {
    flush();
}
