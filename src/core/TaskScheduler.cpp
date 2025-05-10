#include "TaskScheduler.hpp"
#include <algorithm>

// Instantiation du singleton global
TaskScheduler scheduler;

TaskScheduler::TaskScheduler() 
    : cycleStartTime(0), totalExecutionTime(0), cpuUsage(0),
      overruns(0), cycleCount(0) {
#ifdef DEBUG
    DEBUG_SCHEDULER("TaskScheduler initialise - Niveau de debug: %d", DEBUG_TASK_SCHEDULER_LEVEL);
#endif
}

int TaskScheduler::addTask(TaskFunction func, uint32_t intervalMicros, uint8_t priority, const char* name) {
    tasks.emplace_back(func, intervalMicros, priority, name);
    
    sortTasksByPriority();
    return tasks.size() - 1;
}

void TaskScheduler::update(uint32_t maxMicros) {
    uint32_t startTime = micros();
    uint32_t elapsedTime = 0;
    
    // Si c'est le premier cycle ou si on a bouclé sur micros()
    if (cycleStartTime == 0 || startTime < cycleStartTime) {
        cycleStartTime = startTime;
    }
    
    // Calcul du temps entre cycles (pour le calcul de CPU usage)
    uint32_t cyclePeriod = startTime - cycleStartTime;
    cycleStartTime = startTime;
    
    cycleCount++;
    
    // Exécute les tâches dues, en respectant le budget CPU
    for (size_t i = 0; i < tasks.size(); i++) {
        Task& task = tasks[i];
        
        if (!task.enabled) {
            continue;
        }
        
        uint32_t now = micros();
        uint32_t elapsed = now - task.lastRun;
        
        // Si la tâche est due (en tenant compte du débordement micros possible)
        if (elapsed >= task.interval || now < task.lastRun) {
            // Vérifie si on a assez de temps dans le budget
            if (elapsedTime > maxMicros) {
                overruns++;
#ifdef DEBUG
                DEBUG_SCHEDULER_VERBOSE("Budget CPU dépassé, tâche '%s' reportée", task.name);
#endif
                break;
            }
            
            // Exécute la tâche et mesure son temps
            uint32_t taskTime = executeTask(i);
            elapsedTime += taskTime;
            
            // Mise à jour des statistiques
            totalExecutionTime = totalExecutionTime * 0.95 + elapsedTime * 0.05; // Moyenne glissante
        }
    }
    
    // Calcul de l'utilisation CPU (moyenne glissante)
    if (cyclePeriod > 0) {
        uint32_t usage = (elapsedTime * 10000) / cyclePeriod; // *100 pour pourcentage, *100 pour 2 décimales
        cpuUsage = cpuUsage * 0.95 + usage * 0.05;
    }
    
    // Affichage des statistiques de debug (toutes les 10 secondes)
    // Cycle count est incrémenté environ 1000 fois par seconde, donc 10000 = 10 secondes
#ifdef DEBUG
    if (DEBUG_TASK_SCHEDULER_LEVEL >= 1 && cycleCount % 10000 == 0) {
        printDebugStats();
    }
#endif
}

void TaskScheduler::enableTask(int taskIndex, bool enabled) {
    if (taskIndex >= 0 && taskIndex < static_cast<int>(tasks.size())) {
        tasks[taskIndex].enabled = enabled;
    }
}

void TaskScheduler::setTaskInterval(int taskIndex, uint32_t intervalMicros) {
    if (taskIndex >= 0 && taskIndex < static_cast<int>(tasks.size())) {
        tasks[taskIndex].interval = intervalMicros;
    }
}

void TaskScheduler::runTaskNow(int taskIndex) {
    if (taskIndex >= 0 && taskIndex < static_cast<int>(tasks.size())) {
        executeTask(taskIndex);
    }
}

float TaskScheduler::getCpuUsage() const {
    return cpuUsage / 100.0f; // Conversion en pourcentage avec 2 décimales
}

size_t TaskScheduler::getTaskCount() const {
    return tasks.size();
}

void TaskScheduler::sortTasksByPriority() {
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.priority < b.priority;
    });
}

uint32_t TaskScheduler::executeTask(int taskIndex) {
    if (taskIndex < 0 || taskIndex >= static_cast<int>(tasks.size())) {
        return 0;
    }
    
    Task& task = tasks[taskIndex];
    
    uint32_t start = micros();
    task.function();
    uint32_t end = micros();
    
    uint32_t executionTime = end - start;
    task.lastRun = end;
    
    // Moyenne glissante pour le temps d'exécution
    if (task.executionTime == 0) {
        task.executionTime = executionTime;
    } else {
        task.executionTime = task.executionTime * 0.9 + executionTime * 0.1;
    }
    
#ifdef DEBUG
    DEBUG_SCHEDULER_VERBOSE("Tâche '%s' exécutée en %u µs", task.name, executionTime);
#endif
    
    return executionTime;
}

void TaskScheduler::printDebugStats() {
#ifdef DEBUG
    // Ajout d'un séparateur pour améliorer la lisibilité
    DEBUG_SCHEDULER("========== STATISTIQUES SCHEDULER ===========");
    
    // Informations de base (toujours disponibles si DEBUG_TASK_SCHEDULER est activé)
    DEBUG_SCHEDULER("CPU: %.2f%% | Cycles: %u | Overruns: %u", 
                  getCpuUsage(), cycleCount, overruns);
                  
    // Pour le mode verbose, afficher les détails des tâches
    if (DEBUG_TASK_SCHEDULER_LEVEL >= 2) {
        DEBUG_SCHEDULER_VERBOSE("Détails des tâches actives:");
        for (size_t i = 0; i < tasks.size(); i++) {
            if (tasks[i].enabled) {
                DEBUG_SCHEDULER_VERBOSE("  [%u] %s: P%u, %u µs/cycle, interval %u µs", 
                    i, tasks[i].name, tasks[i].priority, tasks[i].executionTime, 
                    tasks[i].interval);
            }
        }
        
        // Afficher les tâches désactivées séparément
        bool hasDisabledTasks = false;
        for (size_t i = 0; i < tasks.size(); i++) {
            if (!tasks[i].enabled) {
                if (!hasDisabledTasks) {
                    DEBUG_SCHEDULER_VERBOSE("Tâches désactivées:");
                    hasDisabledTasks = true;
                }
                DEBUG_SCHEDULER_VERBOSE("  [%u] %s", i, tasks[i].name);
            }
        }
    }
    
    // Terminateur
    DEBUG_SCHEDULER("============================================");
#endif
}

void TaskScheduler::printStats(bool showDetailedStats) {
    // Affiche les statistiques CPU de base sur le port série (fonctionne même en mode non-DEBUG)
    Serial.println("========== STATISTIQUES SCHEDULER ===========");
    Serial.printf("CPU: %.2f%% | Cycles: %u | Overruns: %u\n", 
                getCpuUsage(), cycleCount, overruns);
    
    // Pour le mode verbose, afficher les détails des tâches
    if (showDetailedStats || (DEBUG_TASK_SCHEDULER_LEVEL >= 2)) {
        Serial.println("Détails des tâches actives:");
        for (size_t i = 0; i < tasks.size(); i++) {
            if (tasks[i].enabled) {
                Serial.printf("  [%u] %s: P%u, %u µs/cycle, interval %u µs\n", 
                    i, tasks[i].name, tasks[i].priority, tasks[i].executionTime, 
                    tasks[i].interval);
            }
        }
        
        // Afficher les tâches désactivées séparément
        bool hasDisabledTasks = false;
        for (size_t i = 0; i < tasks.size(); i++) {
            if (!tasks[i].enabled) {
                if (!hasDisabledTasks) {
                    Serial.println("Tâches désactivées:");
                    hasDisabledTasks = true;
                }
                Serial.printf("  [%u] %s\n", i, tasks[i].name);
            }
        }
    }
    
    Serial.println("============================================");
}
