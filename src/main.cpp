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
#include "config/debug/DebugMacros.hpp"
#include "core/utils/Error.hpp"

ApplicationConfiguration appConfig;
std::shared_ptr<MidiControllerApp> app;
std::shared_ptr<DependencyContainer> container;

void setup() {
    Serial.begin(115200); // Assurer que la série est initialisée
    while (!Serial && millis() < 5000) { // Attendre jusqu'à 5 secondes pour la connexion série
        // Attendre la connexion série
    }

    DEBUG_LOG(DEBUG_LEVEL_INFO, "Démarrage de l'application MIDI Controller...");
    container = std::make_shared<DependencyContainer>();

    DEBUG_LOG(DEBUG_LEVEL_INFO, "Initialisation du conteneur...");
    auto initResult = InitializationScript::initializeContainer(container, appConfig);
    if (initResult.isError()) {
        DEBUG_ERROR("ERREUR d'initialisation: %s", initResult.error().value_or(Errors::Unknown).message);
        return;
    }
    DEBUG_LOG(DEBUG_LEVEL_INFO, "Conteneur initialisé avec succès");

    app = std::make_shared<MidiControllerApp>(container);
    auto appInitResult = app->init();
    if (appInitResult.isError()) {
        DEBUG_ERROR("ERREUR d'initialisation de l'app: %s", appInitResult.error().value_or(Errors::Unknown).message);
        return;
    }
    DEBUG_LOG(DEBUG_LEVEL_INFO, "Démarrage terminé");
}

void loop() {
    if (app) {
        app->update();
    }
}