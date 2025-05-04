#include "BufferedMidiOut.hpp"

void BufferedMidiOut::sendCc(MidiChannel ch, MidiCC cc, uint8_t value) {
    buffer_.push({ static_cast<uint8_t>(0xB0 | (ch & 0x0F)), cc, value });
}

void BufferedMidiOut::sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) {
    buffer_.push({ static_cast<uint8_t>(0x90 | (ch & 0x0F)), note, velocity });
}

void BufferedMidiOut::sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) {
    buffer_.push({ static_cast<uint8_t>(0x80 | (ch & 0x0F)), note, velocity });
}

bool BufferedMidiOut::pop(MidiMessage& msg_out) {
    return buffer_.pop(msg_out);
}
