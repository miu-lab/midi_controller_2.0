#include <unity.h>
#include "input/InputRouter.hpp"
#include "utils/EventBus.hpp"
#include "input/InputEvent.hpp"
#include "utils/MidiMessageBuffer.hpp"
#include <vector>
#include <optional>

struct MockMidiOut : public IMidiOut {
    std::vector<MidiMessage> sent;

    void sendCc(MidiChannel ch, MidiCC cc, uint8_t val) override {
        sent.push_back({ static_cast<uint8_t>(0xB0 | (ch & 0x0F)), cc, val });
    }

    void sendNoteOn(MidiChannel ch, MidiNote note, uint8_t vel) override {
        sent.push_back({ static_cast<uint8_t>(0x90 | (ch & 0x0F)), note, vel });
    }

    void sendNoteOff(MidiChannel ch, MidiNote note, uint8_t vel) override {
        sent.push_back({ static_cast<uint8_t>(0x80 | (ch & 0x0F)), note, vel });
    }
};

struct MockProfileManager : public IProfileManager {
    std::optional<MidiControl> getBinding(ControlId id) const override {
        if (id == 1) return MidiControl{1, 10, true};   // pour EncoderTurnedEvent
        if (id == 2) return MidiControl{2, 60, false};  // pour ButtonPressed & Released
        return std::nullopt;
    }
    

    void setBinding(ControlId, MidiControl) override {}
};

void test_input_router_basic() {
    MockMidiOut midi;
    MockProfileManager profiles;

    InputRouter router(midi, profiles);
    router.init();

    EventBus<EncoderTurnedEvent>::publish({ .id = 1, .delta = 3 });
    EventBus<ButtonPressed>::publish({ .id = 2 });
    EventBus<ButtonReleased>::publish({ .id = 2 });

    TEST_ASSERT_EQUAL(3, midi.sent.size());
    TEST_ASSERT_EQUAL_HEX8(0xB1, midi.sent[0].status);  // CC on channel 1
    TEST_ASSERT_EQUAL(10, midi.sent[0].data1);
    TEST_ASSERT_EQUAL(6, midi.sent[0].data2);  // 3 * 2 (relative)

    TEST_ASSERT_EQUAL_HEX8(0x92, midi.sent[1].status);  // Note On on channel 2
    TEST_ASSERT_EQUAL(60, midi.sent[1].data1);
    TEST_ASSERT_EQUAL(127, midi.sent[1].data2);

    TEST_ASSERT_EQUAL_HEX8(0x82, midi.sent[2].status);  // Note Off on channel 2
    TEST_ASSERT_EQUAL(60, midi.sent[2].data1);
    TEST_ASSERT_EQUAL(0, midi.sent[2].data2);
}

void run_input_router_tests() {
    RUN_TEST(test_input_router_basic);
}
