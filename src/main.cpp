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
    // Initialisation de la communication série pour le débogage
    Serial.begin(115200);
    delay(1000);  // Attendre la stabilisation
    Serial.println("MIDI Controller starting...");
    
    // Configuration de Wire2 pour la communication I2C avec l'écran
    Wire2.begin();
    Wire2.setClock(400000);  // 400 kHz
    
    // Création du conteneur d'injection de dépendances
    container = std::make_shared<DependencyContainer>();
    
    // Initialisation du conteneur avec tous les composants nécessaires
    Serial.println("Initializing dependency container...");
    bool initSuccess = InitializationScript::initializeContainer(container, appConfig);
    if (!initSuccess) {
        Serial.println("Container initialization failed!");
        return;
    }
    Serial.println("Container initialization successful.");
    
    // Création de l'application avec le conteneur de dépendances
    app = std::make_shared<MidiControllerApp>(container);
    
    // Initialisation de l'application
    Serial.println("Initializing application...");
    auto result = app->init();
    if (result.isError()) {
        Serial.print("Application initialization failed: ");
        if (auto err = result.error()) {
            Serial.println(err->c_str());
        } else {
            Serial.println("Unknown error");
        }
        return;
    }
    Serial.println("Application started successfully.");
    
    // Afficher un message de bienvenue
    auto uiSystem = container->resolve<IUISystem>();
    if (uiSystem) {
        uiSystem->showMessage("MIDI Controller Ready");
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