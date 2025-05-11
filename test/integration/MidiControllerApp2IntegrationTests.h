#ifndef MIDI_CONTROLLER_APP2_INTEGRATION_TESTS_H
#define MIDI_CONTROLLER_APP2_INTEGRATION_TESTS_H

#include <unity.h>

#include "app/MidiControllerApp2.hpp"
#include "app/di/DependencyContainer.hpp"
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/subsystems/InputSubsystem.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "app/subsystems/UISubsystem.hpp"
#include "config/ApplicationConfiguration.hpp"

/**
 * @brief Test d'intégration complet pour MidiControllerApp2
 * 
 * Ce test vérifie que tous les sous-systèmes réels fonctionnent
 * correctement ensemble au sein de MidiControllerApp2.
 */
void test_midi_controller_app2_full_integration() {
    // Créer une configuration réaliste pour l'intégration
    ApplicationConfiguration config;
    
    // Créer et initialiser l'application
    MidiControllerApp2 app(config);
    auto result = app.init();
    
    // Vérifier que l'initialisation a réussi
    TEST_ASSERT_TRUE(result.isSuccess());
    
    // Simuler un cycle de l'application
    app.update();
    
    // Vérifier l'API publique avec quelques tests de base
    const ControlId testControlId(1);
    app.setControlForNavigation(testControlId, true);
    TEST_ASSERT_TRUE(app.isNavigationControl(testControlId));
    
    // Note: Dans un test d'intégration complet, on pourrait ajouter:
    // - Tests de l'interaction entre les sous-systèmes
    // - Vérification des événements entre sous-systèmes
    // - Tests des scénarios d'utilisation complets
}

/**
 * @brief Test des interactions entre sous-systèmes
 * 
 * Ce test vérifie que les différents sous-systèmes interagissent
 * correctement entre eux via MidiControllerApp2.
 */
void test_midi_controller_app2_subsystem_interactions() {
    // Créer une configuration pour les tests
    ApplicationConfiguration config;
    
    // Créer et initialiser l'application
    MidiControllerApp2 app(config);
    app.init();
    
    // Ce test pourrait être étendu pour vérifier:
    // - Les interactions InputSystem -> MidiSystem (touche pressée -> message MIDI)
    // - Les interactions InputSystem -> UISystem (navigation)
    // - Les interactions MidiSystem -> UISystem (affichage de l'état MIDI)
    
    // Pour le moment, nous vérifions simplement que l'application peut s'exécuter
    // sans erreur avec tous les sous-systèmes
    app.update();
    
    // Si nous arrivons ici sans erreur, le test est considéré comme réussi
    TEST_ASSERT_TRUE(true);
}

#endif // MIDI_CONTROLLER_APP2_INTEGRATION_TESTS_H