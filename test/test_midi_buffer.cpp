#include <unity.h>
#include "utils/MidiMessageBuffer.hpp"
#include "interface_adapters/midi/BufferedMidiOut.hpp"

void test_midi_message_buffer_basic() {
    MidiMessageBuffer buf;
    MidiMessage msg1 = {0x90, 60, 100};
    MidiMessage msg2;

    TEST_ASSERT_TRUE(buf.push(msg1));
    TEST_ASSERT_FALSE(buf.isEmpty());
    TEST_ASSERT_TRUE(buf.pop(msg2));
    TEST_ASSERT_EQUAL_HEX8(msg1.status, msg2.status);
    TEST_ASSERT_EQUAL(msg1.data1, msg2.data1);
    TEST_ASSERT_EQUAL(msg1.data2, msg2.data2);
    TEST_ASSERT_TRUE(buf.isEmpty());
}

void test_buffered_midi_out() {
    BufferedMidiOut out;
    out.sendCc(1, 7, 100);         // ch1, CC7, value 100
    out.sendNoteOn(2, 64, 127);    // ch2, note 64, vel 127
    out.sendNoteOff(2, 64, 0);     // ch2, note 64, vel 0

    MidiMessage m;
    TEST_ASSERT_TRUE(out.pop(m));
    TEST_ASSERT_EQUAL_HEX8(0xB1, m.status);
    TEST_ASSERT_EQUAL(7, m.data1);
    TEST_ASSERT_EQUAL(100, m.data2);

    TEST_ASSERT_TRUE(out.pop(m));
    TEST_ASSERT_EQUAL_HEX8(0x92, m.status);
    TEST_ASSERT_EQUAL(64, m.data1);
    TEST_ASSERT_EQUAL(127, m.data2);

    TEST_ASSERT_TRUE(out.pop(m));
    TEST_ASSERT_EQUAL_HEX8(0x82, m.status);
    TEST_ASSERT_EQUAL(64, m.data1);
    TEST_ASSERT_EQUAL(0, m.data2);
}

void run_midi_buffer_tests() {
    RUN_TEST(test_midi_message_buffer_basic);
    RUN_TEST(test_buffered_midi_out);
}
