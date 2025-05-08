#include "adapters/secondary/midi/MidiMessageBuffer.hpp"

bool MidiMessageBuffer::push(const MidiMessage& msg) {
    if (isFull()) return false;
    buffer_[head_] = msg;
    head_ = (head_ + 1) % Capacity;
    count_++;
    return true;
}

bool MidiMessageBuffer::pop(MidiMessage& msg_out) {
    if (isEmpty()) return false;
    msg_out = buffer_[tail_];
    tail_ = (tail_ + 1) % Capacity;
    count_--;
    return true;
}

bool MidiMessageBuffer::isEmpty() const {
    return count_ == 0;
}

bool MidiMessageBuffer::isFull() const {
    return count_ == Capacity;
}

void MidiMessageBuffer::clear() {
    head_ = tail_ = count_ = 0;
}
