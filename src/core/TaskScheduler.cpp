#include "TaskScheduler.hpp"
#include <algorithm>

// Instantiation du singleton global
TaskScheduler scheduler;

TaskScheduler::TaskScheduler() 
    : cycleStartTime(0), totalExecutionTime(0), cpuUsage(0),
      overruns(0), cycleCount(0) {
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
                DEBUG_SCHEDULER("Budget CPU dépassé, tâche '%s' reportée", task.name);
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
    
    // Affichage des statistiques de debug (toutes les 100 itérations)
    if (DEBUG_TASK_SCHEDULER && cycleCount % 100 == 0) {
        printDebugStats();
    }
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
    
    DEBUG_SCHEDULER("Tâche '%s' exécutée en %u µs", task.name, executionTime);
    
    return executionTime;
}

void TaskScheduler::printDebugStats() {
    DEBUG_SCHEDULER("Stats CPU: %.2f%% utilisation", getCpuUsage());
    DEBUG_SCHEDULER("Overruns: %u/%u cycles", overruns, cycleCount);
    
    DEBUG_SCHEDULER("Tâches:");
    for (size_t i = 0; i < tasks.size(); i++) {
        DEBUG_SCHEDULER("  [%u] %s: P%u, %u µs/cycle, interval %u µs, %s", 
            i, tasks[i].name, tasks[i].priority, tasks[i].executionTime, 
            tasks[i].interval, tasks[i].enabled ? "ON" : "OFF");
    }
}
