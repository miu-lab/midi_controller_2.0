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
    
    // Définir un temps limite pour l'initialisation de Serial
    constexpr unsigned long SERIAL_TIMEOUT_MS = 50; // Limite de 50ms seulement
    unsigned long startTime = millis();
    
    // Attendre Serial mais avec un timeout court
    while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS)) {
        // Attente limitée
    }
    
    // Message de débogage 
    DEBUG_PRINTLN_FLASH("[INIT] MidiController - Démarrage...");

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