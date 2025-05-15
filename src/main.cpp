#include <Arduino.h>
#include <Wire.h>

#include <memory>

#include "app/InitializationScript.hpp"
#include "app/MidiControllerApp.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "tools/ErrorUtils.hpp"

// Variables globales
ApplicationConfiguration appConfig;
std::shared_ptr<MidiControllerApp> app;
std::shared_ptr<DependencyContainer> container;

void setup() {
    Serial.println(F("=== MIDI Controller V2 ==="));

    // Initialisation du système
    container = std::make_shared<DependencyContainer>();
    auto initResult = InitializationScript::initializeContainer(container, appConfig);
    if (initResult.isError()) {
        ErrorUtils::printError(initResult, "Erreur système");
    }

    // Initialisation de l'application
    app = std::make_shared<MidiControllerApp>(container);
    auto appResult = app->init();
    if (appResult.isError()) {
        ErrorUtils::printError(appResult, "Erreur application");
    }

    Serial.println(F("Système prêt"));
}

void loop() {
    unsigned long cycleStartTime = micros();

    if (app) app->update();

    // Céder du temps CPU si possible (pour éviter la gigue MIDI)
    if (micros() - cycleStartTime < 8000) yield();
}
