/**
 * @file main.cpp
 * @brief Point d'entrée principal avec switch mode production/test
 *
 * Pour basculer en mode test : décommenter TEST_MODE_ENABLED
 * Pour le mode production : commenter TEST_MODE_ENABLED
 */

#include <Arduino.h>
#include <lvgl.h>

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "app/InitializationScript.hpp"
#include "app/MidiControllerApp.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"

ApplicationConfiguration appConfig;
std::shared_ptr<MidiControllerApp> app;
std::shared_ptr<DependencyContainer> container;

void setup() {
    delay(2000);
    Serial.println(F("Démarrage de l'application MIDI Controller..."));
    container = std::make_shared<DependencyContainer>();

    Serial.println(F("Initialisation du conteneur..."));
    auto initResult = InitializationScript::initializeContainer(container, appConfig);
    if (initResult.isError()) {
        Serial.print(F("ERREUR d'initialisation: "));
        Serial.println(initResult.error()->c_str());
        return;
    }
    Serial.println(F("Conteneur initialisé avec succès"));

    app = std::make_shared<MidiControllerApp>(container);
    auto appInitResult = app->init();
    if (appInitResult.isError()) {
        Serial.print(F("ERREUR d'initialisation de l'app: "));
        Serial.println(appInitResult.error()->c_str());
        return;
    }
    Serial.println(F("Démarrage terminé"));
}

void loop() {
    if (app) {
        app->update();
    }
}