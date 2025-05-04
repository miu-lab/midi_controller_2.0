#ifndef UNIT_TEST  // <- Ne compile pas cette partie pendant les tests

#ifdef UNIT_TEST_NATIVE
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include "app/ControllerApp.hpp"

ControllerApp app;

void setup() {
    Serial.begin(115200);
    app.init();  // méthode à créer (même vide)
}

void loop() {
    app.update();  // méthode à créer (même vide)
    delay(4);     // éviter de saturer le CPU
}
#endif