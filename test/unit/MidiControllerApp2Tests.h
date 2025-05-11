#ifndef MIDI_CONTROLLER_APP2_TESTS_H
#define MIDI_CONTROLLER_APP2_TESTS_H

#include <unity.h>

#include "app/MidiControllerApp2.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "mocks/MockConfiguration.hpp"
#include "mocks/MockInput.h"
#include "mocks/MockMidi.h"
#include "mocks/MockUI.h"

/**
 * @brief Pour tester l'initialisation de MidiControllerApp2
 */
void test_midi_controller_app2_initialization() {
    // Créer une configuration pour les tests
    ApplicationConfiguration config;
    
    // Créer une instance de l'application
    MidiControllerApp2 app(config);
    
    // Vérifier que l'initialisation fonctionne
    auto result = app.init();
    TEST_ASSERT_TRUE(result.isSuccess());
    
    // Vérifier que tous les sous-systèmes ont été initialisés
    auto configSystem = app.getConfigSystem();
    auto inputSystem = app.getInputSystem();
    auto midiSystem = app.getMidiSystem();
    auto uiSystem = app.getUISystem();
    
    TEST_ASSERT_NOT_NULL(configSystem.get());
    TEST_ASSERT_NOT_NULL(inputSystem.get());
    TEST_ASSERT_NOT_NULL(midiSystem.get());
    TEST_ASSERT_NOT_NULL(uiSystem.get());
}

/**
 * @brief Pour tester la mise à jour des sous-systèmes
 */
void test_midi_controller_app2_update() {
    // Créer une configuration pour les tests
    ApplicationConfiguration config;
    
    // Créer une instance de l'application
    MidiControllerApp2 app(config);
    
    // Initialiser
    app.init();
    
    // Appeler update
    app.update();
    
    // Vérifier que update a été appelé pour tous les sous-systèmes
    // Note: Nous ne pouvons pas vérifier directement car nous n'avons pas de mocks ici,
    // mais les sous-systèmes réels doivent être appelés sans erreur
}

/**
 * @brief Pour tester l'API de navigation
 */
void test_midi_controller_app2_navigation_api() {
    // Créer une configuration pour les tests
    ApplicationConfiguration config;
    
    // Créer une instance de l'application
    MidiControllerApp2 app(config);
    
    // Initialiser
    app.init();
    
    // Tester setControlForNavigation et isNavigationControl
    ControlId testControlId(42);
    
    // Par défaut, aucun contrôle n'est pour la navigation
    TEST_ASSERT_FALSE(app.isNavigationControl(testControlId));
    
    // Définir un contrôle pour la navigation
    app.setControlForNavigation(testControlId, true);
    TEST_ASSERT_TRUE(app.isNavigationControl(testControlId));
    
    // Réinitialiser ce contrôle
    app.setControlForNavigation(testControlId, false);
    TEST_ASSERT_FALSE(app.isNavigationControl(testControlId));
}

/**
 * @brief Pour tester la gestion des erreurs lors de l'initialisation
 * Note: Ce test nécessiterait des mocks qui peuvent échouer à l'initialisation
 */
void test_midi_controller_app2_error_handling() {
    // Dans une implémentation future avec des mocks complets, nous pourrions
    // tester comment l'application gère les erreurs d'initialisation des sous-systèmes
}

#endif // MIDI_CONTROLLER_APP2_TESTS_H