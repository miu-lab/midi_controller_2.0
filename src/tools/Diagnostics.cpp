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
    DEBUG_LOG(DEBUG_LEVEL_INFO, "Module de diagnostics initialisé (mode événementiel)");
    
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
    DEBUG_LOG(DEBUG_LEVEL_INFO, "===================================================");
    DEBUG_LOG(DEBUG_LEVEL_INFO, "DIAGNOSTICS - ÉVÉNEMENT: %s", eventName);
    DEBUG_LOG(DEBUG_LEVEL_INFO, "===================================================");
    
    // Affiche les statistiques
    printSchedulerStats(showDetails);
    
    // En mode détaillé, affiche également les stats mémoire
    if (showDetails) {
        printMemoryStats();
    }
    
    DEBUG_LOG(DEBUG_LEVEL_INFO, "===================================================");
#endif
}

void DiagnosticsManager::printStats(bool showDetails) {
#ifdef DEBUG
    if (!_scheduler) {
        DEBUG_ERROR("Diagnostics non initialisé!");
        return;
    }
    
    printSchedulerStats(showDetails);
    printMemoryStats();
#endif
}

void DiagnosticsManager::enableRegularStats(bool enable) {
    _regularStatsEnabled = enable;
    
#ifdef DEBUG
    DEBUG_LOG(DEBUG_LEVEL_INFO, "Statistiques régulières %s", enable ? "activées" : "désactivées");
#endif
}

void DiagnosticsManager::enableEventDiagnostics(bool enable) {
    _eventDiagnosticsEnabled = enable;
#ifdef DEBUG
    DEBUG_LOG(DEBUG_LEVEL_INFO, "Diagnostics événementiels %s", enable ? "activés" : "désactivés");
#endif
}

void DiagnosticsManager::setStatsInterval(uint16_t seconds) {
    if (seconds < 1) seconds = 1;  // Minimum 1 seconde
    
    _statsInterval = seconds;
#ifdef DEBUG
    DEBUG_LOG(DEBUG_LEVEL_INFO, "Intervalle d'affichage des statistiques: %us", seconds);
#endif
}

bool DiagnosticsManager::handleCommand(const String& command) {
#ifdef DEBUG
    if (command == "stats") {
        DEBUG_LOG(DEBUG_LEVEL_INFO, "===== STATISTIQUES =====");
        printStats(true);
        DEBUG_LOG(DEBUG_LEVEL_INFO, "=======================");
        return true;
    }
    else if (command == "stats detailed") {
        DEBUG_LOG(DEBUG_LEVEL_INFO, "===== STATISTIQUES DÉTAILLÉES =====");
        printStats(true);
        DEBUG_LOG(DEBUG_LEVEL_INFO, "================================");
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
            DEBUG_ERROR("Intervalle invalide");
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
    DEBUG_LOG(DEBUG_LEVEL_INFO, "========== STATISTIQUES SCHEDULER ===========");
    DEBUG_LOG(DEBUG_LEVEL_INFO, "CPU: %.2f%% | Cycles: %u | Overruns: %u", 
              _scheduler->getCpuUsage(), _scheduler->getCycleCount(), _scheduler->getOverruns());
    
    // Détails des tâches si demandé
    if (showDetails) {
        // Utilise l'API publique pour récupérer les infos sur les tâches
        size_t taskCount = _scheduler->getTaskCount();
        
        DEBUG_LOG(DEBUG_LEVEL_INFO, "Nombre total de tâches: %d", taskCount);
        
        // Nous avons besoin d'accéder aux détails des tâches
        // Mais comme nous ne voulons pas modifier TaskScheduler, nous ne pouvons
        // afficher que les informations globales
        
        DEBUG_LOG(DEBUG_LEVEL_INFO, "Note: Pour les détails des tâches, utilisez la commande 'stats detailed'");
        DEBUG_LOG(DEBUG_LEVEL_INFO, "(Nécessite une modification de TaskScheduler pour être implémenté)");
    }
    
    DEBUG_LOG(DEBUG_LEVEL_INFO, "============================================");
#endif
}

void DiagnosticsManager::printMemoryStats() {
#if defined(DEBUG) && (defined(ARDUINO_TEENSY36) || defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41))
    // Statistiques de mémoire spécifiques à Teensy
    DEBUG_LOG(DEBUG_LEVEL_INFO, "========== STATISTIQUES MEMOIRE ============");
    
    // Obtenir les informations de mémoire disponible
    extern unsigned long _heap_start;
    extern unsigned long _heap_end;
    extern char *__brkval;
    
    int heapSize = (char *)&_heap_end - (char *)&_heap_start;
    int heapUsed = __brkval ? (char *)__brkval - (char *)&_heap_start : 0;
    int heapFree = heapSize - heapUsed;
    
    DEBUG_LOG(DEBUG_LEVEL_INFO, "RAM Totale: %d octets", heapSize);
    DEBUG_LOG(DEBUG_LEVEL_INFO, "RAM Utilisée: %d octets (%.1f%%)", 
              heapUsed, (heapUsed * 100.0) / heapSize);
    DEBUG_LOG(DEBUG_LEVEL_INFO, "RAM Libre: %d octets (%.1f%%)", 
              heapFree, (heapFree * 100.0) / heapSize);
    
    DEBUG_LOG(DEBUG_LEVEL_INFO, "============================================");
#elif defined(DEBUG)
    // Pour les autres plateformes, utiliser freeMemory() si disponible
    DEBUG_LOG(DEBUG_LEVEL_INFO, "Information mémoire non disponible sur cette plateforme");
#endif
}