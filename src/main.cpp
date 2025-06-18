#include <Arduino.h>
#include <Wire.h>

#include <memory>

#include "app/InitializationScript.hpp"
#include "app/MidiControllerApp.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "config/ConfigDefaults.hpp"
#include "tools/ErrorUtils.hpp"
#include "adapters/secondary/hardware/output/display/Ili9341LvglDisplay.hpp"

ApplicationConfiguration appConfig;
std::shared_ptr<MidiControllerApp> app;
std::shared_ptr<DependencyContainer> container;

void setup() {
    Serial.println(F("=== MIDI Controller 2.0 - Phase 1 Hardware Test ==="));
    Serial.println(F("Initializing..."));
    
    container = std::make_shared<DependencyContainer>();
    InitializationScript::initializeContainer(container, appConfig);
    app = std::make_shared<MidiControllerApp>(container);
    app->init();
    
    Serial.println(F("Setup complete. Send 'T' to run hardware tests."));
}

void loop() {
    // Phase 1: Check for hardware test command
    if (Serial.available()) {
        char command = Serial.read();
        if (command == 'T' || command == 't') {
            Serial.println(F("Hardware test requested - accessing display..."));
            
            // Récupérer le display depuis le container
            auto display = container->resolve<Ili9341LvglDisplay>();
            if (display) {
                display->runFullHardwareTestSuite();
            } else {
                Serial.println(F("ERROR: Could not access display for testing"));
            }
        }
    }
    
    if (app) app->update();
}