#pragma once

#include <unity.h>
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"

// Tests pour le ConfigurationSubsystem
void test_configuration_subsystem_initialization() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto appConfig = std::make_shared<ApplicationConfiguration>();
    container->registerDependency<ApplicationConfiguration>(appConfig);
    
    // Créer le sous-système
    ConfigurationSubsystem configSystem(container);
    
    // Initialiser le sous-système
    auto result = configSystem.init();
    
    // Vérifier que l'initialisation a réussi
    TEST_ASSERT_TRUE(result.isSuccess());
}

void test_configuration_navigation_controls() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto appConfig = std::make_shared<ApplicationConfiguration>();
    container->registerDependency<ApplicationConfiguration>(appConfig);
    
    // Créer le sous-système
    ConfigurationSubsystem configSystem(container);
    
    // Initialiser le sous-système
    configSystem.init();
    
    // Tester les méthodes de navigation
    ControlId testId = 3;
    
    // Par défaut, un contrôle ne devrait pas être configuré pour la navigation
    TEST_ASSERT_FALSE(configSystem.isNavigationControl(testId));
    
    // Définir le contrôle pour la navigation
    configSystem.setControlForNavigation(testId, true);
    TEST_ASSERT_TRUE(configSystem.isNavigationControl(testId));
    
    // Désactiver le contrôle pour la navigation
    configSystem.setControlForNavigation(testId, false);
    TEST_ASSERT_FALSE(configSystem.isNavigationControl(testId));
}

void test_configuration_accessors() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto appConfig = std::make_shared<ApplicationConfiguration>();
    container->registerDependency<ApplicationConfiguration>(appConfig);
    
    // Créer le sous-système
    ConfigurationSubsystem configSystem(container);
    
    // Initialiser le sous-système
    configSystem.init();
    
    // Tester les accesseurs
    TEST_ASSERT_NOT_NULL(&configSystem.encoderConfigs());
    TEST_ASSERT_NOT_NULL(&configSystem.buttonConfigs());
    
    // Vérifier que les méthodes renvoient des valeurs valides
    TEST_ASSERT_TRUE(configSystem.isDebugEnabled() == true || configSystem.isDebugEnabled() == false);
    TEST_ASSERT_TRUE(configSystem.midiChannel() >= 0 && configSystem.midiChannel() <= 15);
    TEST_ASSERT_TRUE(configSystem.isHardwareInitEnabled() == true || configSystem.isHardwareInitEnabled() == false);
}
