/**
 * @file main.cpp
 * @brief Point d'entrée principal - Interface minimale
 *
 * Toute la logique complexe est déléguée à SystemManager
 * pour garder ce fichier simple et lisible.
 */

#include <Arduino.h>
#include "app/SystemManager.hpp"

// Instance unique du gestionnaire système
SystemManager systemManager;

void setup() {
    systemManager.initialize();
}

void loop() {
    systemManager.update();
}