#include <unity.h>
#include "use_cases/ProcessEncoders.hpp"
#include "utils/EventBus.hpp"
#include "input/InputEvent.hpp"


// Après :
struct MockEncoder : public IEncoder {
    EncoderId id;
    int8_t   delta;
    bool     pressed;

    int8_t    readDelta() override        { return delta; }
    bool      isPressed()  const override { return pressed; }
    EncoderId getId()      const override { return id; }
    uint16_t  getPpr()     const override { return 0; }  // <— valeur factice
};
static int turnedSum = 0;
static bool lastButtonPressed = false;

void onTurned(const EncoderTurnedEvent& e) {
    turnedSum += e.delta;
}

void onPressed(const EncoderButtonEvent& e) {
    lastButtonPressed = e.pressed;
}

void test_process_encoders_event_bus() {
    MockEncoder enc1; enc1.id = 1;
    MockEncoder enc2; enc2.id = 2;

    std::vector<IEncoder*> encoders = { &enc1, &enc2 };
    ProcessEncoders processor(encoders);

    EventBus<EncoderTurnedEvent>::subscribe(onTurned);
    EventBus<EncoderButtonEvent>::subscribe(onPressed);

    enc1.delta = 3;
    enc2.pressed = true;

    turnedSum = 0;
    lastButtonPressed = false;

    processor.update();

    TEST_ASSERT_EQUAL(3, turnedSum);
    TEST_ASSERT_TRUE(lastButtonPressed);
}

void run_process_encoders_tests() {
    RUN_TEST(test_process_encoders_event_bus);
}
