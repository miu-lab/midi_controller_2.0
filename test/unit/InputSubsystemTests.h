#pragma once

#include <unity.h>
#include "app/subsystems/InputSubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "mocks/MockConfigurationSubsystem.h"

// Tests pour InputSubsystem
void test_input_subsystem_initialization() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto mockConfig = std::make_shared<MockConfigurationSubsystem>();
    container->registerDependency<IConfiguration>(mockConfig);
    
    // Créer le sous-système
    InputSubsystem inputSystem(container);
    
    // Initialiser le sous-système
    auto result = inputSystem.init();
    
    // Vérifier que l'initialisation a réussi
    TEST_ASSERT_TRUE(result.isSuccess());
}

void test_input_subsystem_configure() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto mockConfig = std::make_shared<MockConfigurationSubsystem>();
    container->registerDependency<IConfiguration>(mockConfig);
    
    // Créer le sous-système
    InputSubsystem inputSystem(container);
    
    // Initialiser le sous-système
    inputSystem.init();
    
    // Créer des configurations d'encodeur et de bouton
    std::vector<EncoderConfig> encoderConfigs = {
        {0, 1, 2, 600, true, 4, true}  // Paramètres fictifs: id, pinA, pinB, ppr, hasButton, pinButton, activeLowButton
    };
    
    std::vector<ButtonConfig> buttonConfigs = {
        {0, 1, false}  // Paramètres fictifs
    };
    
    // Configurer les encodeurs et les boutons
    auto resultEncoders = inputSystem.configureEncoders(encoderConfigs);
    auto resultButtons = inputSystem.configureButtons(buttonConfigs);
    
    // Vérifier que la configuration a réussi
    TEST_ASSERT_TRUE(resultEncoders.isSuccess());
    TEST_ASSERT_TRUE(resultButtons.isSuccess());
}

void test_input_subsystem_update() {
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Enregistrer les dépendances nécessaires
    auto mockConfig = std::make_shared<MockConfigurationSubsystem>();
    container->registerDependency<IConfiguration>(mockConfig);
    
    // Créer le sous-système
    InputSubsystem inputSystem(container);
    
    // Initialiser le sous-système
    inputSystem.init();
    
    // Mettre à jour le sous-système plusieurs fois
    for (int i = 0; i < 5; i++) {
        inputSystem.update();
    }
    
    // Pas de test spécifique, juste vérifier que les mises à jour ne provoquent pas d'erreurs
    TEST_ASSERT_TRUE(true);
}
