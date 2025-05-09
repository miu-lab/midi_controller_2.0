#ifndef UNIT_TEST  // <- Ne compile pas cette partie pendant les tests

#include <Arduino.h>

#include "app/MidiControllerApp.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/TaskScheduler.hpp"

// Création de la configuration de l'application
ApplicationConfiguration appConfig;

// Création de l'application avec la configuration
MidiControllerApp app(appConfig);

// Les IDs des tâches pour référence (global)
int appUpdateTaskId = -1;
int ledsUpdateTaskId = -1;  // Pour une éventuelle gestion des LEDs
int diagnosticTaskId = -1;  // Tâche de diagnostic

// Fonctions de rappel pour les tâches
void appUpdateCallback() {
    app.update();
}

void diagnosticCallback() {
    // Tâche de diagnostic pour monitorer les performances

    // Afficher les statistiques de performance si débogage activé
#if defined(DEBUG) && defined(DEBUG_TASK_SCHEDULER)
    DEBUG_SCHEDULER("Charge CPU: %.2f%%", scheduler.getCpuUsage());
    DEBUG_SCHEDULER("Nombre de tâches: %u", scheduler.getTaskCount());
#endif
}

void setup() {
    Serial.begin(115200);  // Initialiser le série, mais ne pas attendre qu'il soit prêt

    // Définir un temps limite pour l'initialisation de Serial
    constexpr unsigned long SERIAL_TIMEOUT_MS = 50;  // Limite de 50ms seulement
    unsigned long startTime = millis();

    // Attendre Serial mais avec un timeout court
    while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS)) {
        // Attente limitée
    }

    // Message de débogage
    DEBUG_PRINTLN_FLASH("[INIT] MidiController - Démarrage...");

    // Initialiser l'application
    app.init();

    // Configurer l'ordonnanceur de tâches

    // Tâche principale de l'application - haute priorité (1)
    // Exécute app.update() toutes les 1ms (1000Hz)
    appUpdateTaskId = scheduler.addTask(appUpdateCallback, 1000, 1, "AppUpdate");

    // Tâche de diagnostic - basse priorité (10)
    // Exécute la fonction de diagnostic toutes les 3 secondes
    diagnosticTaskId = scheduler.addTask(diagnosticCallback, 3000000, 10, "Diagnostic");

    DEBUG_PRINTLN_FLASH("[INIT] Configuration de l'ordonnanceur terminée");
}

void loop() {
    // Exécuter l'ordonnanceur de tâches avec un budget CPU de 5ms par cycle
    // Il gère automatiquement l'exécution des tâches selon leur priorité
    scheduler.update(5000);

    // Donner du temps au système pour d'autres tâches (Watchdog, etc.)
    yield();
}
#endif