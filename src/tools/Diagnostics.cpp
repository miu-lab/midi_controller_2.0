#include "Diagnostics.hpp"

#include "core/utils/Error.hpp"

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
    // TODO DEBUG MSG
    
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
    // TODO DEBUG MSG
    // TODO DEBUG MSG
    // TODO DEBUG MSG
    
    // Affiche les statistiques
    printSchedulerStats(showDetails);
    
    // En mode détaillé, affiche également les stats mémoire
    if (showDetails) {
        printMemoryStats();
    }
    
    // TODO DEBUG MSG
#endif
}

void DiagnosticsManager::printStats(bool showDetails) {
#ifdef DEBUG
    if (!_scheduler) {
        return;
    }
    
    printSchedulerStats(showDetails);
    printMemoryStats();
#endif
}

void DiagnosticsManager::enableRegularStats(bool enable) {
    _regularStatsEnabled = enable;
    
#ifdef DEBUG
    // TODO DEBUG MSG
#endif
}

void DiagnosticsManager::enableEventDiagnostics(bool enable) {
    _eventDiagnosticsEnabled = enable;
#ifdef DEBUG
    // TODO DEBUG MSG
#endif
}

void DiagnosticsManager::setStatsInterval(uint16_t seconds) {
    if (seconds < 1) seconds = 1;  // Minimum 1 seconde
    
    _statsInterval = seconds;
#ifdef DEBUG
    // TODO DEBUG MSG
#endif
}

bool DiagnosticsManager::handleCommand(const String& command) {
#ifdef DEBUG
    if (command == "stats") {
        // TODO DEBUG MSG
        printStats(true);
        // TODO DEBUG MSG
        return true;
    }
    else if (command == "stats detailed") {
        // TODO DEBUG MSG
        printStats(true);
        // TODO DEBUG MSG
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
            // TODO DEBUG MSG
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

void DiagnosticsManager::printError(const Result<bool>& result, const char* prefix) {
    if (result.isError()) {
        // TODO DEBUG MSG
    }
}

void DiagnosticsManager::printSchedulerStats(bool showDetails) {
    // TODO DEBUG MSG
    return;
}

void DiagnosticsManager::printMemoryStats() {
    // TODO DEBUG MSG
}