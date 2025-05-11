#pragma once

#include <unity.h>
#include "app/MidiControllerApp.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"
#include "../mocks/MockConfigurationSubsystem.h"
#include "../mocks/MockInputSubsystem.h"
#include "../mocks/MockMidiSubsystem.h"
#include "../mocks/MockUISubsystem.h"

// Tests d'intégration pour MidiControllerApp
void test_midi_controller_app2_full_integration() {
    // Créer une instance de ApplicationConfiguration
    ApplicationConfiguration config;
    
    // Créer une instance de DependencyContainer
    auto container = std::make_shared<DependencyContainer>();
    
    // Créer les mocks
    auto mockConfig = std::make_shared<MockConfigurationSubsystem>();
    auto mockInput = std::make_shared<MockInputSubsystem>();
    auto mockMidi = std::make_shared<MockMidiSubsystem>();
    auto mockUI = std::make_shared<MockUISubsystem>();
    
    // Enregistrer les mocks dans le conteneur comme interfaces
    container->registerDependency<ApplicationConfiguration>(std::make_shared<ApplicationConfiguration>(config));
    container->registerDependency<IConfiguration>(mockConfig);
    container->registerDependency<IInputSystem>(mockInput);
    container->registerDependency<IMidiSystem>(mockMidi);
    container->registerDependency<IUISystem>(mockUI);
    
    // Créer l'application avec la configuration ET le conteneur externe
    MidiControllerApp app(config, container);
    
    // Initialiser l'application
    app.init();
    
    // Vérifier que les sous-systèmes ont été initialisés
    TEST_ASSERT_TRUE(mockConfig->initCalled);
    TEST_ASSERT_TRUE(mockInput->initCalled);
    TEST_ASSERT_TRUE(mockMidi->initCalled);
    TEST_ASSERT_TRUE(mockUI->initCalled);
    
    // Mettre à jour l'application
    app.update();
    
    // Vérifier que les sous-systèmes ont été mis à jour
    TEST_ASSERT_TRUE(mockInput->updateCalled);
    TEST_ASSERT_TRUE(mockMidi->updateCalled);
    TEST_ASSERT_TRUE(mockUI->updateCalled);
}

void test_midi_controller_app2_subsystem_interactions() {
    // Créer une instance de ApplicationConfiguration
    ApplicationConfiguration config;
    
    // Créer l'application avec la configuration
    MidiControllerApp app(config);
    
    // Initialiser l'application
    app.init();
    
    // Tester les interactions entre les sous-systèmes
    // Exemple: Définir un contrôle pour la navigation et vérifier qu'il est correctement défini
    ControlId testId = 5;
    app.setControlForNavigation(testId, true);
    TEST_ASSERT_TRUE(app.isNavigationControl(testId));
    
    // Réinitialiser
    app.setControlForNavigation(testId, false);
    TEST_ASSERT_FALSE(app.isNavigationControl(testId));
}
