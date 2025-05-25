#include <Arduino.h>
#include <Wire.h>

#include <memory>

#include "app/InitializationScript.hpp"
#include "app/MidiControllerApp.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "config/ConfigDefaults.hpp"
#include "tools/ErrorUtils.hpp"
// #include "tools/testing/ConfigurationValidator.hpp"  // Temporairement désactivé

// Variables globales
ApplicationConfiguration appConfig;
std::shared_ptr<MidiControllerApp> app;
std::shared_ptr<DependencyContainer> container;

void setup() {
    container = std::make_shared<DependencyContainer>();
    InitializationScript::initializeContainer(container, appConfig);
    app = std::make_shared<MidiControllerApp>(container);
    app->init();
}

void loop() {
    if (app) app->update();
}
