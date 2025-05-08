#pragma once
#include <cstdint>
#include <cstddef>


struct MidiMessage {
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
};

class MidiMessageBuffer {
public:
    static constexpr size_t Capacity = 16;

    bool push(const MidiMessage& msg);
    bool pop(MidiMessage& msg_out);
    bool isEmpty() const;
    bool isFull() const;
    void clear();

private:
    MidiMessage buffer_[Capacity];
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t count_ = 0;
};
