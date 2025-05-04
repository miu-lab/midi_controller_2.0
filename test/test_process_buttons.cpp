#include <unity.h>
#include "use_cases/ProcessButtons.hpp"
#include "utils/EventBus.hpp"
#include "input/InputEvent.hpp"

#include "domain/IButton.hpp"

struct MockButton : public IButton {
    ControlId id;
    bool simulatedState = false;

    void setPressed(bool state) { simulatedState = state; }

    ControlId getId() const override { return id; }

    bool isPressed() const override { return simulatedState; }

    void update() override {}  // Pas utile pour le test
};



static ButtonId lastEventId = 0;
static bool lastPressed = false;

void onPressed(const ButtonPressed& e) {
    lastEventId = e.id;
    lastPressed = true;
}

void onReleased(const ButtonReleased& e) {
    lastEventId = e.id;
    lastPressed = false;
}

void test_process_buttons() {
    MockButton b1; b1.id = 1;
    MockButton b2; b2.id = 2;
    std::vector<IButton*> buttons = { &b1, &b2 };

    EventBus<ButtonPressed>::subscribe(onPressed);
    EventBus<ButtonReleased>::subscribe(onReleased);
    ProcessButtons handler(buttons);

    b1.setPressed(true);
    handler.update();

    TEST_ASSERT_EQUAL(1, lastEventId);
    TEST_ASSERT_TRUE(lastPressed);

    b1.setPressed(false);
    handler.update();

    TEST_ASSERT_EQUAL(1, lastEventId);
    TEST_ASSERT_FALSE(lastPressed);
}

void run_process_buttons_tests() {
    RUN_TEST(test_process_buttons);
}
