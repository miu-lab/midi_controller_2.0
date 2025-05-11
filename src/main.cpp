#include <Arduino.h>
#include <memory>

// Configurations
#include "config/ApplicationConfiguration.hpp"

// Container et initialisation
#include "app/di/DependencyContainer.hpp"
#include "app/InitializationScript.hpp"

// Application
#include "app/MidiControllerApp.hpp"

// Variables globales
ApplicationConfiguration appConfig;
std::shared_ptr<MidiControllerApp> app;
std::shared_ptr<DependencyContainer> container;

void setup() {
    // Création du conteneur d'injection de dépendances
    container = std::make_shared<DependencyContainer>();
    
    // Initialisation du conteneur avec tous les composants nécessaires
    bool initSuccess = InitializationScript::initializeContainer(container, appConfig);
    if (!initSuccess) {
        return;
    }
    
    // Création de l'application avec le conteneur de dépendances
    app = std::make_shared<MidiControllerApp>(container);
    
    // Initialisation de l'application
    auto result = app->init();
    if (result.isError()) {
        return;
    }
}

void loop() {
    // Mise à jour de l'application
    if (app) {
        app->update();
    }
    
    // Permet aux autres tâches système de s'exécuter
    yield();
}
