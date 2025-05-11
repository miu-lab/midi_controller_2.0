#ifndef MOCK_TESTS_H
#define MOCK_TESTS_H

#include <unity.h>
#include "mocks/MockInput.h"
#include "mocks/MockMidi.h"
#include "mocks/MockUI.h"

// Tests pour les mocks
void test_mock_initialization() {
    MockInput input;
    MockMidi midi;
    MockUI ui;
    
    // Initialiser les mocks et vérifier les résultats
    auto inputResult = input.init();
    auto midiResult = midi.init();
    auto uiResult = ui.init(true);
    
    // Vérifier que l'initialisation a fonctionné avec Unity
    TEST_ASSERT_TRUE(inputResult.isSuccess());
    TEST_ASSERT_TRUE(midiResult.isSuccess());
    TEST_ASSERT_TRUE(uiResult.isSuccess());
    
    TEST_ASSERT_TRUE(input.initialized);
    TEST_ASSERT_TRUE(midi.initialized);
    TEST_ASSERT_TRUE(ui.initialized);
    TEST_ASSERT_TRUE(ui.fullUIEnabled);
}

void test_mock_updates() {
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

void test_midi_messages() {
    MockMidi midi;
    
    // Envoyer quelques messages MIDI et vérifier les résultats
    auto noteOnResult = midi.sendNoteOn(0, 60, 100);
    auto noteOffResult = midi.sendNoteOff(0, 60);
    auto ccResult = midi.sendControlChange(0, 7, 127);
    
    // Vérifier que les opérations ont réussi
    TEST_ASSERT_TRUE(noteOnResult.isSuccess());
    TEST_ASSERT_TRUE(noteOffResult.isSuccess());
    TEST_ASSERT_TRUE(ccResult.isSuccess());
    
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

void test_ui_messages() {
    MockUI ui;
    
    // Envoyer des messages à l'UI et vérifier les résultats
    auto msg1Result = ui.showMessage("Test message 1");
    auto msg2Result = ui.showMessage("Test message 2");
    auto clearResult = ui.clearDisplay();
    
    // Vérifier que les opérations ont réussi
    TEST_ASSERT_TRUE(msg1Result.isSuccess());
    TEST_ASSERT_TRUE(msg2Result.isSuccess());
    TEST_ASSERT_TRUE(clearResult.isSuccess());
    
    // Vérifier que les messages ont été enregistrés
    TEST_ASSERT_EQUAL_INT(2, ui.messages.size());
    TEST_ASSERT_EQUAL_STRING("Test message 1", ui.messages[0].c_str());
    TEST_ASSERT_EQUAL_STRING("Test message 2", ui.messages[1].c_str());
    TEST_ASSERT_TRUE(ui.displayCleared);
}

#endif // MOCK_TESTS_H
