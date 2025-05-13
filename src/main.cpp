#include <Arduino.h>

#include <memory>

// Configurations
#include "config/ApplicationConfiguration.hpp"
#include "config/debug/DebugConfig.hpp"

// Container et initialisation
#include "app/InitializationScript.hpp"
#include "app/di/DependencyContainer.hpp"

// Application
#include "app/MidiControllerApp.hpp"

// Bibliothèque Wire pour I2C
#include <Wire.h>

// Variables globales
ApplicationConfiguration appConfig;
std::shared_ptr<MidiControllerApp> app;
std::shared_ptr<DependencyContainer> container;

void setup() {
    // Création du conteneur d'injection de dépendances
    container = std::make_shared<DependencyContainer>();

    InitializationScript::initializeContainer(container, appConfig);

    // Création de l'application avec le conteneur de dépendances
    app = std::make_shared<MidiControllerApp>(container);

    auto result = app->init();

    // Afficher un message de bienvenue
    auto uiSystem = container->resolve<IUISystem>();
    // Pour le débogage, dessiner un motif de test
    auto display = container->resolve<DisplayPort>();
}

void loop() {
    // Mise à jour de l'application
    if (app) {
        app->update();
    }

    // Permet aux autres tâches système de s'exécuter
    yield();
}