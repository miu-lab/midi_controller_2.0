#ifndef UI_SUBSYSTEM_TESTS_H
#define UI_SUBSYSTEM_TESTS_H

#include <unity.h>

#include "../mocks/MockConfiguration.hpp"
#include "../mocks/MockDisplay.hpp"
#include "app/di/DependencyContainer.hpp"
#include "app/subsystems/UISubsystem.hpp"

// Tests d'initialisation du sous-système UI
void test_ui_subsystem_initialization() {
    auto container = std::make_shared<DependencyContainer>();
    auto mockConfig = std::make_shared<MockConfiguration>();
    auto mockDisplay = std::make_shared<MockDisplay>();

    // Désactiver l'initialisation matérielle pour les tests
    mockConfig->hardwareInitEnabled = false;

    container->registerImplementation<IConfiguration, MockConfiguration>(mockConfig);
    container->registerImplementation<IDisplay, MockDisplay>(mockDisplay);

    auto uiSystem = std::make_shared<UISubsystem>(container);
    uiSystem->init(false);  // UI simplifiée pour les tests

    // Vérifier que le système est correctement enregistré dans le conteneur
    auto resolvedSystem = container->resolve<IUISystem>();
    TEST_ASSERT_NOT_NULL(resolvedSystem.get());
}

// Tests des méthodes d'affichage
void test_ui_subsystem_display_methods() {
    auto container = std::make_shared<DependencyContainer>();
    auto mockConfig = std::make_shared<MockConfiguration>();
    auto mockDisplay = std::make_shared<MockDisplay>();

    // Désactiver l'initialisation matérielle pour les tests
    mockConfig->hardwareInitEnabled = false;

    container->registerImplementation<IConfiguration, MockConfiguration>(mockConfig);
    container->registerImplementation<IDisplay, MockDisplay>(mockDisplay);

    auto uiSystem = std::make_shared<UISubsystem>(container);
    uiSystem->init(false);  // UI simplifiée pour les tests

    // Garder une référence au mock pour les tests
    auto testDisplay = mockDisplay;

    // Tester les méthodes d'affichage
    uiSystem->showMessage("Test Message");
    TEST_ASSERT_EQUAL_STRING("Test Message", testDisplay->lastMessage().c_str());
    TEST_ASSERT_EQUAL(true, testDisplay->wasUpdateCalled());

    uiSystem->clearDisplay();
    TEST_ASSERT_EQUAL(true, testDisplay->wasCleared());
    TEST_ASSERT_EQUAL(true, testDisplay->wasUpdateCalled());
}

// Tests avec UI complète
void test_ui_subsystem_full_ui() {
    auto container = std::make_shared<DependencyContainer>();
    auto mockConfig = std::make_shared<MockConfiguration>();
    auto mockDisplay = std::make_shared<MockDisplay>();

    // Désactiver l'initialisation matérielle pour les tests
    mockConfig->hardwareInitEnabled = false;

    container->registerImplementation<IConfiguration, MockConfiguration>(mockConfig);
    container->registerImplementation<IDisplay, MockDisplay>(mockDisplay);

    auto uiSystem = std::make_shared<UISubsystem>(container);
    uiSystem->init(true);  // UI complète

    // Garder une référence au mock pour les tests
    auto testDisplay = mockDisplay;

    // Appeler update ne devrait pas provoquer d'erreur
    uiSystem->update();

    // Les messages devraient toujours fonctionner
    uiSystem->showMessage("Full UI Message");
    TEST_ASSERT_EQUAL_STRING("Full UI Message", testDisplay->lastMessage().c_str());
}

#endif  // UI_SUBSYSTEM_TESTS_H