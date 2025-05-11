#include <unity.h>
#include "mocks/MockInput.h"
#include "mocks/MockMidi.h"
#include "mocks/MockUI.h"

// Fonction appelée avant chaque test
void setUp(void) {
    // Initialisation si nécessaire
}

// Fonction appelée après chaque test
void tearDown(void) {
    // Nettoyage si nécessaire
}

// Test de l'initialisation des mocks
void test_mock_initialization(void) {
    MockInput input;
    MockMidi midi;
    MockUI ui;
    
    // Initialiser les mocks
    input.init();
    midi.init();
    ui.init(true);
    
    // Vérifier que l'initialisation a fonctionné avec Unity
    TEST_ASSERT_TRUE(input.initialized);
    TEST_ASSERT_TRUE(midi.initialized);
    TEST_ASSERT_TRUE(ui.initialized);
    TEST_ASSERT_TRUE(ui.fullUIEnabled);
}

// Test des mises à jour
void test_mock_updates(void) {
    MockInput input;
    MockMidi midi;
    MockUI ui;
    
    // Faire une mise à jour
    input.update();
    midi.update();
    ui.update();
    
    // Vérifier que les compteurs ont été incrémentés
    TEST_ASSERT_EQUAL_INT(1, input.updateCount);
    TEST_ASSERT_EQUAL_INT(1, midi.updateCount);
    TEST_ASSERT_EQUAL_INT(1, ui.updateCount);
}

// Test des fonctionnalités MIDI
void test_midi_messages(void) {
    MockMidi midi;
    
    // Envoyer quelques messages MIDI
    midi.sendNoteOn(0, 60, 100);
    midi.sendNoteOff(0, 60);
    midi.sendControlChange(0, 7, 127);
    
    // Vérifier que les messages ont été enregistrés
    TEST_ASSERT_EQUAL_INT(1, midi.noteOnMessages.size());
    TEST_ASSERT_EQUAL_INT(1, midi.noteOffMessages.size());
    TEST_ASSERT_EQUAL_INT(1, midi.ccMessages.size());
    
    // Vérifier le contenu des messages
    TEST_ASSERT_EQUAL_INT(0, midi.noteOnMessages[0].channel);
    TEST_ASSERT_EQUAL_INT(60, midi.noteOnMessages[0].note);
    TEST_ASSERT_EQUAL_INT(100, midi.noteOnMessages[0].velocity);
    
    TEST_ASSERT_EQUAL_INT(0, midi.noteOffMessages[0].channel);
    TEST_ASSERT_EQUAL_INT(60, midi.noteOffMessages[0].note);
    
    TEST_ASSERT_EQUAL_INT(0, midi.ccMessages[0].channel);
    TEST_ASSERT_EQUAL_INT(7, midi.ccMessages[0].controller);
    TEST_ASSERT_EQUAL_INT(127, midi.ccMessages[0].value);
}

// Test des fonctionnalités UI
void test_ui_messages(void) {
    MockUI ui;
    
    // Envoyer des messages à l'UI
    ui.showMessage("Test message 1");
    ui.showMessage("Test message 2");
    ui.clearDisplay();
    
    // Vérifier que les messages ont été enregistrés
    TEST_ASSERT_EQUAL_INT(2, ui.messages.size());
    TEST_ASSERT_EQUAL_STRING("Test message 1", ui.messages[0].c_str());
    TEST_ASSERT_EQUAL_STRING("Test message 2", ui.messages[1].c_str());
    TEST_ASSERT_TRUE(ui.displayCleared);
}

// Fonction principale pour les tests
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_mock_initialization);
    RUN_TEST(test_mock_updates);
    RUN_TEST(test_midi_messages);
    RUN_TEST(test_ui_messages);
    
    return UNITY_END();
}
