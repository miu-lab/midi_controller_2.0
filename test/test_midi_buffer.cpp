#include <unity.h>
#include "adapters/secondary/midi/MidiMessageBuffer.hpp"
#include "adapters/secondary/midi/BufferedMidiOut.hpp"
#include "core/domain/IMidiOut.hpp"

// Classe qui capture les messages MIDI pour les tests
class TestMidiOut : public IMidiOut
{
public:
    struct CapturedMessage
    {
        uint8_t type; // 0xB0 pour CC, 0x90 pour Note On, 0x80 pour Note Off
        uint8_t channel;
        uint8_t control; // CC ou note
        uint8_t value;   // Valeur ou vélocité
    };

    std::vector<CapturedMessage> messages;

    void sendCc(MidiChannel ch, MidiCC cc, uint8_t value) override
    {
        messages.push_back({0xB0, ch, cc, value});
    }

    void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t velocity) override
    {
        messages.push_back({0x90, ch, note, velocity});
    }

    void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t velocity) override
    {
        messages.push_back({0x80, ch, note, velocity});
    }

    void clear()
    {
        messages.clear();
    }
};

void test_midi_message_buffer_basic()
{
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

void test_buffered_midi_out()
{
    TestMidiOut mockOutput;
    BufferedMidiOut out(mockOutput);

    // Envoyer quelques messages MIDI
    out.sendCc(1, 7, 100);      // ch1, CC7, value 100
    out.sendNoteOn(2, 64, 127); // ch2, note 64, vel 127
    out.sendNoteOff(2, 64, 0);  // ch2, note 64, vel 0

    // Ils ne sont pas encore envoyés car ils sont bufferisés
    TEST_ASSERT_EQUAL(0, mockOutput.messages.size());

    // Flush force l'envoi des messages
    out.flush();

    // Maintenant les messages devraient être envoyés
    TEST_ASSERT_EQUAL(3, mockOutput.messages.size());

    // Vérifier le message CC
    TEST_ASSERT_EQUAL_HEX8(0xB0, mockOutput.messages[0].type);
    TEST_ASSERT_EQUAL(1, mockOutput.messages[0].channel);
    TEST_ASSERT_EQUAL(7, mockOutput.messages[0].control);
    TEST_ASSERT_EQUAL(100, mockOutput.messages[0].value);

    // Vérifier le message Note On
    TEST_ASSERT_EQUAL_HEX8(0x90, mockOutput.messages[1].type);
    TEST_ASSERT_EQUAL(2, mockOutput.messages[1].channel);
    TEST_ASSERT_EQUAL(64, mockOutput.messages[1].control);
    TEST_ASSERT_EQUAL(127, mockOutput.messages[1].value);

    // Vérifier le message Note Off
    TEST_ASSERT_EQUAL_HEX8(0x80, mockOutput.messages[2].type);
    TEST_ASSERT_EQUAL(2, mockOutput.messages[2].channel);
    TEST_ASSERT_EQUAL(64, mockOutput.messages[2].control);
    TEST_ASSERT_EQUAL(0, mockOutput.messages[2].value);
}

void run_midi_buffer_tests()
{
    RUN_TEST(test_midi_message_buffer_basic);
    RUN_TEST(test_buffered_midi_out);
}
