#include "Diagnostics.hpp"
#include "config/debug/DebugMacros.hpp"

// Initialisation des variables statiques
TaskScheduler* DiagnosticsManager::_scheduler = nullptr;
unsigned long DiagnosticsManager::_lastStatsTime = 0;
uint16_t DiagnosticsManager::_statsInterval = 10;  // 10 secondes par défaut
bool DiagnosticsManager::_regularStatsEnabled = false;  // Désactivé par défaut
bool DiagnosticsManager::_eventDiagnosticsEnabled = true;   // Activé par défaut

void DiagnosticsManager::init(TaskScheduler& scheduler, uint16_t interval) {
    _scheduler = &scheduler;
    _statsInterval = interval;
    _lastStatsTime = millis();
    _regularStatsEnabled = false;  // Désactivé par défaut
    _eventDiagnosticsEnabled = true;  // Activé par défaut
    
#ifdef DEBUG
    Serial.printf("[DIAG] Module de diagnostics initialisé (mode événementiel)\n");
    
    // Affiche les statistiques initiales
    printStats(false);
#endif
}

void DiagnosticsManager::update() {
#ifdef DEBUG
    if (!_scheduler || !_regularStatsEnabled) {
        return;
    }
    
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - _lastStatsTime;
    
    // Conversion ms -> s et vérification de l'intervalle
    if (elapsedTime >= (_statsInterval * 1000)) {
        printSchedulerStats(DEBUG_TASK_SCHEDULER_LEVEL >= 2);
        _lastStatsTime = currentTime;
    }
#endif
}

void DiagnosticsManager::onEvent(const char* eventName, bool showDetails) {
#ifdef DEBUG
    if (!_scheduler || !_eventDiagnosticsEnabled) {
        return;
    }
    
    // Affiche un en-tête avec le nom de l'événement
    Serial.println();
    Serial.println("===================================================");
    Serial.printf("DIAGNOSTICS - ÉVÉNEMENT: %s\n", eventName);
    Serial.println("===================================================");
    
    // Affiche les statistiques
    printSchedulerStats(showDetails);
    
    // En mode détaillé, affiche également les stats mémoire
    if (showDetails) {
        printMemoryStats();
    }
    
    Serial.println("===================================================");
    Serial.println();
#endif
}

void DiagnosticsManager::printStats(bool showDetails) {
#ifdef DEBUG
    if (!_scheduler) {
        Serial.println("[DIAG] Erreur: Diagnostics non initialisé!");
        return;
    }
    
    printSchedulerStats(showDetails);
    printMemoryStats();
#endif
}

void DiagnosticsManager::enableRegularStats(bool enable) {
#ifdef DEBUG
    _regularStatsEnabled = enable;
    
    Serial.printf("[DIAG] Statistiques régulières %s\n", 
                 enable ? "activées" : "désactivées");
#endif
}

void DiagnosticsManager::enableEventDiagnostics(bool enable) {
#ifdef DEBUG
    _eventDiagnosticsEnabled = enable;
    Serial.printf("[DIAG] Diagnostics événementiels %s\n", 
                 enable ? "activés" : "désactivés");
#endif
}

void DiagnosticsManager::setStatsInterval(uint16_t seconds) {
#ifdef DEBUG
    if (seconds < 1) seconds = 1;  // Minimum 1 seconde
    
    _statsInterval = seconds;
    Serial.printf("[DIAG] Intervalle d'affichage des statistiques: %us\n", seconds);
#else
    _statsInterval = seconds;
#endif
}

bool DiagnosticsManager::handleCommand(const String& command) {
#ifdef DEBUG
    if (command == "stats") {
        Serial.println("\n===== STATISTIQUES =====");
        printStats(true);
        Serial.println("=======================\n");
        return true;
    }
    else if (command == "stats detailed") {
        Serial.println("\n===== STATISTIQUES DÉTAILLÉES =====");
        printStats(true);
        Serial.println("================================\n");
        return true;
    }
    else if (command == "stats off") {
        enableRegularStats(false);
        return true;
    }
    else if (command == "stats on") {
        enableRegularStats(true);
        return true;
    }
    else if (command == "event off") {
        enableEventDiagnostics(false);
        return true;
    }
    else if (command == "event on") {
        enableEventDiagnostics(true);
        return true;
    }
    else if (command.startsWith("stats interval ")) {
        // Extrait l'intervalle (ex: "stats interval 5" -> 5 secondes)
        String intervalStr = command.substring(15);
        int interval = intervalStr.toInt();
        
        if (interval > 0) {
            setStatsInterval(interval);
        } else {
            Serial.println("[DIAG] Erreur: intervalle invalide");
        }
        return true;
    }
    else if (command == "memory") {
        printMemoryStats();
        return true;
    }
#endif
    
    return false;  // Commande non reconnue
}

void DiagnosticsManager::printSchedulerStats(bool showDetails) {
#ifdef DEBUG
    // Affichage des statistiques du scheduler
    Serial.println("========== STATISTIQUES SCHEDULER ===========");
    Serial.printf("CPU: %.2f%% | Cycles: %u | Overruns: %u\n", 
                _scheduler->getCpuUsage(), _scheduler->getCycleCount(), _scheduler->getOverruns());
    
    // Détails des tâches si demandé
    if (showDetails) {
        // Utilise l'API publique pour récupérer les infos sur les tâches
        size_t taskCount = _scheduler->getTaskCount();
        
        Serial.println("Nombre total de tâches: " + String(taskCount));
        
        // Nous avons besoin d'accéder aux détails des tâches
        // Mais comme nous ne voulons pas modifier TaskScheduler, nous ne pouvons
        // afficher que les informations globales
        
        Serial.println("Note: Pour les détails des tâches, utilisez la commande 'stats detailed'");
        Serial.println("(Nécessite une modification de TaskScheduler pour être implémenté)");
    }
    
    Serial.println("============================================");
#endif
}

void DiagnosticsManager::printMemoryStats() {
#if defined(DEBUG) && (defined(ARDUINO_TEENSY36) || defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41))
    // Statistiques de mémoire spécifiques à Teensy
    Serial.println("========== STATISTIQUES MEMOIRE ============");
    
    // Obtenir les informations de mémoire disponible
    extern unsigned long _heap_start;
    extern unsigned long _heap_end;
    extern char *__brkval;
    
    int heapSize = (char *)&_heap_end - (char *)&_heap_start;
    int heapUsed = __brkval ? (char *)__brkval - (char *)&_heap_start : 0;
    int heapFree = heapSize - heapUsed;
    
    Serial.printf("RAM Totale: %d octets\n", heapSize);
    Serial.printf("RAM Utilisée: %d octets (%.1f%%)\n", 
                 heapUsed, (heapUsed * 100.0) / heapSize);
    Serial.printf("RAM Libre: %d octets (%.1f%%)\n", 
                 heapFree, (heapFree * 100.0) / heapSize);
    
    Serial.println("============================================");
#elif defined(DEBUG)
    // Pour les autres plateformes, utiliser freeMemory() si disponible
    Serial.println("Information mémoire non disponible sur cette plateforme");
#endif
}