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

#include "core/utils/Error.hpp"

ApplicationConfiguration appConfig;
std::shared_ptr<MidiControllerApp> app;
std::shared_ptr<DependencyContainer> container;

void setup() {
    Serial.begin(115200);                 // pas obligatoire sur Teensy, mais explicite
    const uint32_t timeout = 5000;        // 5 s max ; ajustez selon vos besoins
    uint32_t t0 = millis();
    while (!Serial && millis() - t0 < timeout) {}

    Serial.println("=== Init OK ===");
    Serial.println("Creating DependencyContainer...");
    container = std::make_shared<DependencyContainer>();

    Serial.println("Initializing container...");
    auto initResult = InitializationScript::initializeContainer(container, appConfig);
    if (initResult.isError()) {
        Serial.print("ERREUR InitializationScript: ");
        Serial.println(static_cast<int>(initResult.error().value().code));
        Serial.println(initResult.error().value().message);
        return;
    }

    Serial.println("Creating MidiControllerApp...");
    app = std::make_shared<MidiControllerApp>(container);
    auto appInitResult = app->init();
    if (appInitResult.isError()) {
        Serial.print("ERREUR MidiControllerApp init: ");
        Serial.println(static_cast<int>(appInitResult.error().value().code));
        Serial.println(appInitResult.error().value().message);
        return;
    }
    
    Serial.println("=== Initialization Complete ===");
}

void loop() {
    if (app) {
        app->update();
    }
}