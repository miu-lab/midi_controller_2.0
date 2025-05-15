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
}

void loop() {
    // Mesurer le temps de début du cycle
    unsigned long cycleStartTime = micros();

    // Mise à jour de l'application avec les systèmes critiques
    if (app) {
        app->update();
    }

    // Calculer le temps restant dans ce cycle (si on vise 1ms par cycle)
    unsigned long elapsedTime = micros() - cycleStartTime;
    static const unsigned long TARGET_CYCLE_TIME_US = 4000;  // 1ms

    // N'appeler yield() que si on a du temps restant
    // Cela évite d'interrompre le traitement MIDI avec yield() quand le temps est critique
    if (elapsedTime < TARGET_CYCLE_TIME_US) {
        yield();
    }
}