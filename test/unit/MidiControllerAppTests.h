#pragma once

#include <unity.h>
#include "app/MidiControllerApp.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "../mocks/MockConfigurationSubsystem.h"
#include "../mocks/MockInputSubsystem.h"
#include "../mocks/MockMidiSubsystem.h"
#include "../mocks/MockUISubsystem.h"

// Tests unitaires pour MidiControllerApp
void test_midi_controller_app2_initialization() {
    // Créer une instance de ApplicationConfiguration
    ApplicationConfiguration config;
    
    // Créer l'application avec la configuration
    MidiControllerApp app(config);
    
    // Aucune assertion spécifique ici, on vérifie juste que la construction
    // ne lance pas d'exception ou ne provoque pas d'erreur
    TEST_ASSERT_TRUE(true);
}

void test_midi_controller_app2_update() {
    // Créer une instance de ApplicationConfiguration
    ApplicationConfiguration config;
    
    // Créer l'application avec la configuration
    MidiControllerApp app(config);
    
    // Initialiser l'application
    app.init();
    
    // Mettre à jour l'application plusieurs fois
    for (int i = 0; i < 5; i++) {
        app.update();
    }
    
    // Comme pour le test précédent, on vérifie simplement que l'exécution
    // ne provoque pas d'erreurs
    TEST_ASSERT_TRUE(true);
}

void test_midi_controller_app2_navigation_api() {
    // Créer une instance de ApplicationConfiguration
    ApplicationConfiguration config;
    
    // Créer l'application avec la configuration
    MidiControllerApp app(config);
    
    // Initialiser l'application
    app.init();
    
    // Tester les méthodes d'API de navigation
    ControlId testId = 3;
    
    // Par défaut, un contrôle ne devrait pas être configuré pour la navigation
    TEST_ASSERT_FALSE(app.isNavigationControl(testId));
    
    // Définir le contrôle pour la navigation
    app.setControlForNavigation(testId, true);
    TEST_ASSERT_TRUE(app.isNavigationControl(testId));
    
    // Désactiver le contrôle pour la navigation
    app.setControlForNavigation(testId, false);
    TEST_ASSERT_FALSE(app.isNavigationControl(testId));
}
