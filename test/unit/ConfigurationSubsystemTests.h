#ifndef CONFIGURATION_SUBSYSTEM_TESTS_H
#define CONFIGURATION_SUBSYSTEM_TESTS_H

#include <unity.h>
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"

// Configuration par défaut pour les tests
void test_configuration_subsystem_initialization() {
    auto container = std::make_shared<DependencyContainer>();
    auto appConfig = std::make_shared<ApplicationConfiguration>();
    container->registerDependency<ApplicationConfiguration>(appConfig);
    
    ConfigurationSubsystem configSystem(container);
    configSystem.init();
    
    // Vérifier que les configurations par défaut sont chargées
    TEST_ASSERT_EQUAL(false, configSystem.isNavigationControl(ControlId(1)));
    TEST_ASSERT_GREATER_THAN(0, configSystem.getEncoderConfigs().size());
    TEST_ASSERT_GREATER_THAN(0, configSystem.getButtonConfigs().size());
}

// Test d'ajout et de vérification des contrôles de navigation
void test_configuration_navigation_controls() {
    auto container = std::make_shared<DependencyContainer>();
    auto appConfig = std::make_shared<ApplicationConfiguration>();
    container->registerDependency<ApplicationConfiguration>(appConfig);
    
    ConfigurationSubsystem configSystem(container);
    configSystem.init();
    
    // Par défaut, aucun contrôle n'est pour la navigation
    TEST_ASSERT_EQUAL(false, configSystem.isNavigationControl(ControlId(1)));
    
    // Définir un contrôle pour la navigation
    configSystem.setControlForNavigation(ControlId(1), true);
    TEST_ASSERT_EQUAL(true, configSystem.isNavigationControl(ControlId(1)));
    
    // Réinitialiser ce contrôle
    configSystem.setControlForNavigation(ControlId(1), false);
    TEST_ASSERT_EQUAL(false, configSystem.isNavigationControl(ControlId(1)));
}

// Test des accesseurs de configuration
void test_configuration_accessors() {
    auto container = std::make_shared<DependencyContainer>();
    auto appConfig = std::make_shared<ApplicationConfiguration>();
    
    // Comme les méthodes de configuration n'existent pas dans ApplicationConfiguration,
    // nous allons tester les valeurs par défaut retournées par ConfigurationSubsystem
    container->registerDependency<ApplicationConfiguration>(appConfig);
    
    ConfigurationSubsystem configSystem(container);
    configSystem.init();
    
    // Vérifier les valeurs par défaut
    TEST_ASSERT_EQUAL(true, configSystem.isDebugEnabled());    // Valeur par défaut = true
    TEST_ASSERT_EQUAL(1, configSystem.getMidiChannel());       // Valeur par défaut = 1
    TEST_ASSERT_EQUAL(true, configSystem.isHardwareInitEnabled()); // Valeur par défaut = true
}

#endif // CONFIGURATION_SUBSYSTEM_TESTS_H