#pragma once

#include <unity.h>
#include "mocks/MockConfigurationSubsystem.h"
#include "mocks/MockInputSubsystem.h"
#include "mocks/MockMidiSubsystem.h"
#include "mocks/MockUISubsystem.h"

// Tests pour les mocks
void test_mock_initialization() {
    // Tester l'initialisation des mocks
    MockConfigurationSubsystem mockConfig;
    MockInputSubsystem mockInput;
    MockMidiSubsystem mockMidi;
    MockUISubsystem mockUI;
    
    // Vérifier l'état initial
    TEST_ASSERT_FALSE(mockConfig.initCalled);
    TEST_ASSERT_FALSE(mockInput.initCalled);
    TEST_ASSERT_FALSE(mockMidi.initCalled);
    TEST_ASSERT_FALSE(mockUI.initCalled);
    
    // Initialiser les mocks
    mockInput.init();
    mockMidi.init();
    mockUI.init();
    
    // Vérifier qu'ils ont été initialisés
    TEST_ASSERT_TRUE(mockInput.initCalled);
    TEST_ASSERT_TRUE(mockMidi.initCalled);
    TEST_ASSERT_TRUE(mockUI.initCalled);
}

void test_mock_updates() {
    // Tester les mises à jour des mocks
    MockInputSubsystem mockInput;
    MockMidiSubsystem mockMidi;
    MockUISubsystem mockUI;
    
    // Vérifier l'état initial
    TEST_ASSERT_FALSE(mockInput.updateCalled);
    TEST_ASSERT_FALSE(mockMidi.updateCalled);
    TEST_ASSERT_FALSE(mockUI.updateCalled);
    
    // Mettre à jour les mocks
    mockInput.update();
    mockMidi.update();
    mockUI.update();
    
    // Vérifier qu'ils ont été mis à jour
    TEST_ASSERT_TRUE(mockInput.updateCalled);
    TEST_ASSERT_TRUE(mockMidi.updateCalled);
    TEST_ASSERT_TRUE(mockUI.updateCalled);
}

void test_midi_messages() {
    // Tester l'envoi de messages MIDI
    MockMidiSubsystem mockMidi;
    
    // Envoyer des messages MIDI
    mockMidi.sendNoteOn(1, 60, 100);
    mockMidi.sendNoteOff(1, 60);
    mockMidi.sendControlChange(1, 7, 127);
    
    // Vérifier que les messages ont été envoyés
    TEST_ASSERT_EQUAL(3, mockMidi.sentMessages.size());
    
    // Vérifier les messages
    TEST_ASSERT_TRUE(mockMidi.hasMessage(MockMidiSubsystem::MidiMessage::Type::NoteOn, 1, 60, 100));
    TEST_ASSERT_TRUE(mockMidi.hasMessage(MockMidiSubsystem::MidiMessage::Type::NoteOff, 1, 60));
    TEST_ASSERT_TRUE(mockMidi.hasMessage(MockMidiSubsystem::MidiMessage::Type::ControlChange, 1, 7, 127));
}

void test_ui_messages() {
    // Tester l'affichage de messages UI
    MockUISubsystem mockUI;
    
    // Afficher des messages
    mockUI.showMessage("Test Message 1");
    mockUI.showMessage("Test Message 2");
    
    // Vérifier que les messages ont été affichés
    TEST_ASSERT_EQUAL(2, mockUI.displayedMessages.size());
    TEST_ASSERT_TRUE(mockUI.hasDisplayedMessage("Test Message 1"));
    TEST_ASSERT_TRUE(mockUI.hasDisplayedMessage("Test Message 2"));
    
    // Effacer l'affichage
    mockUI.clearDisplay();
    
    // Vérifier que l'affichage a été effacé
    TEST_ASSERT_TRUE(mockUI.displayCleared);
    TEST_ASSERT_EQUAL(0, mockUI.displayedMessages.size());
}
