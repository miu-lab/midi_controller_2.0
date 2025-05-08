#pragma once
#include "types.hpp"

class IMidiOut {
public:
    virtual ~IMidiOut() = default;
    virtual void sendCc(MidiChannel ch, MidiCC cc, uint8_t value) = 0;
    virtual void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) = 0;
    virtual void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) = 0;
};
