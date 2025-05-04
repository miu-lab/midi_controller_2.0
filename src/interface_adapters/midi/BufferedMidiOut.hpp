#pragma once
#include "domain/IMidiOut.hpp"
#include "utils/MidiMessageBuffer.hpp"

class BufferedMidiOut : public IMidiOut {
public:
    void sendCc(MidiChannel ch, MidiCC cc, uint8_t value) override;
    void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) override;
    void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) override;

    bool pop(MidiMessage& msg_out);  // pour tester ou transmettre

private:
    MidiMessageBuffer buffer_;
};
