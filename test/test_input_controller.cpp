#include <Arduino.h>
#include <unity.h>
#include "core/controllers/InputController.hpp"
#include "app/services/NavigationConfigService.hpp"

// Variables pour suivre les appels de callback
int navigationEncoderCalls = 0;
int midiEncoderCalls = 0;
int navigationButtonCalls = 0;
int midiButtonCalls = 0;

// Variables pour stocker les dernières valeurs
EncoderId lastEncoderId = 0;
int32_t lastAbsolutePosition = 0;
int8_t lastRelativeChange = 0;
ButtonId lastButtonId = 0;
bool lastButtonState = false;

// Fonction pour réinitialiser les compteurs
void resetCounters() {
    navigationEncoderCalls = 0;
    midiEncoderCalls = 0;
    navigationButtonCalls = 0;
    midiButtonCalls = 0;
}

void setUp() {
    resetCounters();
}

void tearDown() {
    // Rien à faire
}

void test_input_controller_callbacks() {
    // Créer les services nécessaires
    NavigationConfigService navConfig;
    
    // Configurer les contrôles de navigation
    const EncoderId NAV_ENCODER = 1;
    const ButtonId NAV_BUTTON = 10;
    navConfig.setControlForNavigation(NAV_ENCODER, true);
    navConfig.setControlForNavigation(NAV_BUTTON, true);
    
    // Créer le contrôleur d'entrée
    InputController controller(navConfig);
    
    // Configurer les callbacks
    controller.setNavigationEncoderCallback([](EncoderId id, int32_t absPos, int8_t relChange) {
        navigationEncoderCalls++;
        lastEncoderId = id;
        lastAbsolutePosition = absPos;
        lastRelativeChange = relChange;
    });
    
    controller.setMidiEncoderCallback([](EncoderId id, int32_t absPos, int8_t relChange) {
        midiEncoderCalls++;
        lastEncoderId = id;
        lastAbsolutePosition = absPos;
        lastRelativeChange = relChange;
    });
    
    controller.setNavigationButtonCallback([](ButtonId id, bool pressed) {
        navigationButtonCalls++;
        lastButtonId = id;
        lastButtonState = pressed;
    });
    
    controller.setMidiButtonCallback([](ButtonId id, bool pressed) {
        midiButtonCalls++;
        lastButtonId = id;
        lastButtonState = pressed;
    });
    
    // Test des encodeurs de navigation
    controller.processEncoderTurn(NAV_ENCODER, 10, 1);
    TEST_ASSERT_EQUAL(1, navigationEncoderCalls);
    TEST_ASSERT_EQUAL(0, midiEncoderCalls);
    TEST_ASSERT_EQUAL(NAV_ENCODER, lastEncoderId);
    TEST_ASSERT_EQUAL(10, lastAbsolutePosition);
    TEST_ASSERT_EQUAL(1, lastRelativeChange);
    
    resetCounters();
    
    // Test des encodeurs MIDI
    controller.processEncoderTurn(2, 20, 2);
    TEST_ASSERT_EQUAL(0, navigationEncoderCalls);
    TEST_ASSERT_EQUAL(1, midiEncoderCalls);
    TEST_ASSERT_EQUAL(2, lastEncoderId);
    TEST_ASSERT_EQUAL(20, lastAbsolutePosition);
    TEST_ASSERT_EQUAL(2, lastRelativeChange);
    
    resetCounters();
    
    // Test des boutons de navigation
    controller.processButtonPress(NAV_BUTTON, true);
    TEST_ASSERT_EQUAL(1, navigationButtonCalls);
    TEST_ASSERT_EQUAL(0, midiButtonCalls);
    TEST_ASSERT_EQUAL(NAV_BUTTON, lastButtonId);
    TEST_ASSERT_EQUAL(true, lastButtonState);
    
    resetCounters();
    
    // Test des boutons MIDI
    controller.processButtonPress(20, false);
    TEST_ASSERT_EQUAL(0, navigationButtonCalls);
    TEST_ASSERT_EQUAL(1, midiButtonCalls);
    TEST_ASSERT_EQUAL(20, lastButtonId);
    TEST_ASSERT_EQUAL(false, lastButtonState);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_input_controller_callbacks);
    return UNITY_END();
}
