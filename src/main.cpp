#ifndef UNIT_TEST  // <- Ne compile pas cette partie pendant les tests

#ifdef UNIT_TEST_NATIVE
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include "app/MidiControllerApp.hpp"
#include "config/ApplicationConfiguration.hpp"

// Création de la configuration de l'application
ApplicationConfiguration appConfig;

// Création de l'application avec la configuration
MidiControllerApp app(appConfig);

void setup() {
    Serial.begin(115200);  // Initialiser le série, mais ne pas attendre qu'il soit prêt
    // Décommenter si vous avez besoin de debug via Serial, mais avec un timeout
    // unsigned long startTime = millis();
    // while (!Serial && (millis() - startTime < 1000)) { /* attend max 1s */ }

    app.init();  // initialiser l'application
}

unsigned long lastUpdateTime = 0;
const unsigned long minUpdateInterval = 1; // 1ms (1000Hz) au lieu de 4ms (250Hz)

void loop() {
    unsigned long currentTime = micros() / 1000; // Temps en millisecondes
    
    // Limiter la fréquence maximale des mises à jour
    if (currentTime - lastUpdateTime >= minUpdateInterval) {
        app.update();
        lastUpdateTime = currentTime;
    }
    
    // Utiliser yield() au lieu de delay pour permettre le multitâche
    yield();
}
#endif