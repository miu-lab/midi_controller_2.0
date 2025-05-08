#ifndef UNIT_TEST  // <- Ne compile pas cette partie pendant les tests

#ifdef UNIT_TEST_NATIVE
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include "app/ControllerApp.hpp"

ControllerApp app;

void setup() {
    Serial.begin(115200);  // Initialiser le série, mais ne pas attendre qu'il soit prêt
    // Décommenter si vous avez besoin de debug via Serial, mais avec un timeout
    // unsigned long startTime = millis();
    // while (!Serial && (millis() - startTime < 1000)) { /* attend max 1s */ }
    
    app.init();  // initialiser l'application
}

void loop() {
    app.update();  // méthode à créer (même vide)
    delay(4);     // éviter de saturer le CPU
}
#endif