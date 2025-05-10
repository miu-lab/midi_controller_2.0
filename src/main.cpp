#include <Arduino.h>

#include "app/MidiControllerApp.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "config/debug/SerialBuffer.hpp"
#include "core/TaskScheduler.hpp"
#include "tools/Diagnostics.hpp"

// Variables globales
ApplicationConfiguration appConfig;
MidiControllerApp app(appConfig);
int appUpdateTaskId = -1;

// Callback pour la mise à jour de l'application
void appUpdateCallback() {
    app.update();
}

// Gestion des commandes du moniteur série
void handleSerialCommands() {
    if (Serial.available() <= 0) {
        return;
    }

    String command = Serial.readStringUntil('\n');
    command.trim();

    // Priorité aux commandes de diagnostics
    if (DiagnosticsManager::handleCommand(command)) {
        return;
    }

    // Commandes standard
    if (command == "clear") {
        SerialBuffer::clear();
        Serial.write(27);     // ESC
        Serial.print("[2J");  // Efface l'écran
        Serial.write(27);     // ESC
        Serial.print("[H");   // Curseur en haut à gauche
        Serial.println("Moniteur effacé");
    } else if (command == "dump") {
        Serial.println("\n===== CONTENU DU TAMPON =====\n");
        SerialBuffer::flush();
        Serial.println("\n============================\n");
    } else if (command == "help") {
        Serial.println("\nCommandes disponibles:");
        Serial.println("  clear - Efface le moniteur et le tampon");
        Serial.println("  dump  - Affiche tout le contenu du tampon");
        Serial.println("  help  - Affiche cette aide");

        // Commandes de diagnostics
        Serial.println("\nCommandes de diagnostics:");
        Serial.println("  stats            - Affiche les statistiques du système");
        Serial.println("  stats detailed   - Affiche les statistiques détaillées");
        Serial.println("  stats on/off     - Active/désactive l'affichage régulier");
        Serial.println("  event on/off     - Active/désactive le diagnostic sur événements");
        Serial.println("  stats interval N - Définit l'intervalle à N secondes");
        Serial.println("  memory           - Affiche les statistiques de mémoire");
    } else if (command.length() > 0) {
        Serial.println("Commande non reconnue. Tapez 'help' pour voir les commandes disponibles.");
    }
}

void setup() {
    // Initialisation du port série
    Serial.begin(9600);
    constexpr unsigned long SERIAL_TIMEOUT_MS = 50;
    unsigned long startTime = millis();
    while (!Serial && (millis() - startTime < SERIAL_TIMEOUT_MS)) {
        // Attente avec timeout
    }

    // Initialisation du tampon série et de l'application
    SerialBuffer::init(300);
    DEBUG_PRINTLN_FLASH("[INIT] MidiController - Démarrage...");
    app.init();

    // Configuration de l'ordonnanceur
    appUpdateTaskId = scheduler.addTask(appUpdateCallback, 1000, 1, "AppUpdate");
    DEBUG_PRINTLN_FLASH("[INIT] Configuration de l'ordonnanceur terminée");

    // Initialisation du module de diagnostics (0.1 seconde d'intervalle)
    INIT_DIAGNOSTICS(scheduler, .1);
}

void loop() {
    // Mise à jour de l'ordonnanceur (budget CPU de 4000µs)
    scheduler.update(4000);
    
    // Gestion des commandes série
    handleSerialCommands();
    
    // Mise à jour des diagnostics (mode événementiel)
    UPDATE_DIAGNOSTICS();
    
    // Permet aux autres tâches système de s'exécuter
    yield();
}
