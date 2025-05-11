#pragma once

#include <unity.h>
#include "app/subsystems/UISubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "mocks/MockConfigurationSubsystem.h"

// Tests pour UISubsystem
void test_ui_subsystem_initialization() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto mockConfig = std::make_shared<MockConfigurationSubsystem>();
    container->registerDependency<IConfiguration>(mockConfig);
    
    // Créer le sous-système
    UISubsystem uiSystem(container);
    
    // Initialiser le sous-système
    auto result = uiSystem.init();
    
    // Vérifier que l'initialisation a réussi
    TEST_ASSERT_TRUE(result.isSuccess());
}

void test_ui_subsystem_display_methods() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto mockConfig = std::make_shared<MockConfigurationSubsystem>();
    container->registerDependency<IConfiguration>(mockConfig);
    
    // Créer le sous-système
    UISubsystem uiSystem(container);
    
    // Initialiser le sous-système
    uiSystem.init();
    
    // Tester les méthodes d'affichage
    auto resultShow = uiSystem.showMessage("Test Message");
    auto resultClear = uiSystem.clearDisplay();
    
    // Vérifier que les opérations ont réussi
    TEST_ASSERT_TRUE(resultShow.isSuccess());
    TEST_ASSERT_TRUE(resultClear.isSuccess());
}

void test_ui_subsystem_full_ui() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto mockConfig = std::make_shared<MockConfigurationSubsystem>();
    container->registerDependency<IConfiguration>(mockConfig);
    
    // Créer le sous-système
    UISubsystem uiSystem(container);
    
    // Initialiser le sous-système avec l'UI complète
    auto result = uiSystem.init(true);
    
    // Vérifier que l'initialisation a réussi
    TEST_ASSERT_TRUE(result.isSuccess());
    
    // Mettre à jour le sous-système plusieurs fois
    for (int i = 0; i < 5; i++) {
        uiSystem.update();
    }
    
    // Pas de test spécifique, juste vérifier que les mises à jour ne provoquent pas d'erreurs
    TEST_ASSERT_TRUE(true);
}
