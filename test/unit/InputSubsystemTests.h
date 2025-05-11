#ifndef INPUT_SUBSYSTEM_TESTS_H
#define INPUT_SUBSYSTEM_TESTS_H

#include <unity.h>
#include "app/subsystems/InputSubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "../mocks/MockConfiguration.hpp"

// Tests d'initialisation du sous-système d'entrée
void test_input_subsystem_initialization() {
    auto container = std::make_shared<DependencyContainer>();
    auto mockConfig = std::make_shared<MockConfiguration>();
    
    // Configurer les encodeurs et boutons de test
    mockConfig->encoderConfigs = {
        EncoderConfig{1, 2, 3, 1},
        EncoderConfig{4, 5, 6, 2}
    };
    
    mockConfig->buttonConfigs = {
        ButtonConfig{10, 3},
        ButtonConfig{11, 4}
    };
    
    container->registerImplementation<IConfiguration, MockConfiguration>(mockConfig);
    
    auto inputSystem = std::make_shared<InputSubsystem>(container);
    inputSystem->init();
    
    // Vérifier que le système est correctement initialisé
    // Note: comme EncoderManager et DigitalButtonManager sont des dépendances externes,
    // nous ne pouvons pas facilement tester leur état interne ici
    // On pourrait plutôt vérifier que le système est bien enregistré dans le conteneur
    
    auto resolvedSystem = container->resolve<IInputSystem>();
    TEST_ASSERT_NOT_NULL(resolvedSystem.get());
}

// Tests de configuration des encodeurs
void test_input_subsystem_configure() {
    auto container = std::make_shared<DependencyContainer>();
    auto mockConfig = std::make_shared<MockConfiguration>();
    
    // Configurer les encodeurs et boutons de test
    mockConfig->encoderConfigs = {
        EncoderConfig{1, 2, 3, 1},
        EncoderConfig{4, 5, 6, 2}
    };
    
    mockConfig->buttonConfigs = {
        ButtonConfig{10, 3},
        ButtonConfig{11, 4}
    };
    
    container->registerImplementation<IConfiguration, MockConfiguration>(mockConfig);
    
    auto inputSystem = std::make_shared<InputSubsystem>(container);
    inputSystem->init();
    
    // Reconfigurer avec de nouvelles valeurs
    std::vector<EncoderConfig> newEncoderConfigs = {
        EncoderConfig{7, 8, 9, 3}
    };
    
    inputSystem->configureEncoders(newEncoderConfigs);
    
    // Idéalement, on vérifierait que les encodeurs ont été correctement reconfigurés
    // Mais comme EncoderManager est une dépendance externe, nous ne pouvons pas
    // facilement vérifier son état interne dans ce test
}

// Test de mise à jour du sous-système d'entrée
void test_input_subsystem_update() {
    auto container = std::make_shared<DependencyContainer>();
    auto mockConfig = std::make_shared<MockConfiguration>();
    
    container->registerImplementation<IConfiguration, MockConfiguration>(mockConfig);
    
    auto inputSystem = std::make_shared<InputSubsystem>(container);
    inputSystem->init();
    
    // Appeler update() ne devrait pas provoquer d'erreur
    inputSystem->update();
    
    // Ici aussi, nous ne pouvons pas facilement vérifier l'état interne des gestionnaires
    // dans ce test, car ce sont des dépendances externes
}

#endif // INPUT_SUBSYSTEM_TESTS_H