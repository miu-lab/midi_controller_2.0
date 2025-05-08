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
static int eventCount = 0;  // Pour compter le nombre d'événements

void onPressed(const ButtonPressed& e) {
    lastEventId = e.id;
    lastPressed = true;
    eventCount++;
}

void onReleased(const ButtonReleased& e) {
    lastEventId = e.id;
    lastPressed = false;
    eventCount++;
}

void test_process_buttons() {
    MockButton b1; b1.id = 1;
    MockButton b2; b2.id = 2;
    std::vector<IButton*> buttons = { &b1, &b2 };

    EventBus<ButtonPressed>::subscribe(onPressed);
    EventBus<ButtonReleased>::subscribe(onReleased);
    
    // Réinitialiser les variables de test
    lastEventId = 0;
    lastPressed = false;
    eventCount = 0;
    
    ProcessButtons handler(buttons);
    
    // Commencer avec les boutons non pressés
    b1.setPressed(false);
    b2.setPressed(false);
    
    // Première mise à jour - initialisation des états
    handler.update();
    
    // Vérifier qu'aucun événement n'a été publié
    TEST_ASSERT_EQUAL(0, eventCount);
    
    // Presser le bouton 1
    b1.setPressed(true);
    handler.update();
    
    // Vérifier qu'un événement ButtonPressed a été publié
    TEST_ASSERT_EQUAL(1, eventCount);
    TEST_ASSERT_EQUAL(1, lastEventId);
    TEST_ASSERT_TRUE(lastPressed);
    
    // Relâcher le bouton 1
    b1.setPressed(false);
    handler.update();
    
    // Vérifier qu'un événement ButtonReleased a été publié
    TEST_ASSERT_EQUAL(2, eventCount);
    TEST_ASSERT_EQUAL(1, lastEventId);
    TEST_ASSERT_FALSE(lastPressed);
}

void run_process_buttons_tests() {
    RUN_TEST(test_process_buttons);
}
