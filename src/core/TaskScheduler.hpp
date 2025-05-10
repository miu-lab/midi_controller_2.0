#ifndef TASK_SCHEDULER_HPP
#define TASK_SCHEDULER_HPP

#include <Arduino.h>
#include <vector>
#include <functional>
#include "config/debug/DebugMacros.hpp"
#include "config/debug/TaskSchedulerConfig.hpp"

/**
 * @brief Définition d'un type pour les fonctions de tâches
 */
using TaskFunction = std::function<void()>;

/**
 * @brief Structure représentant une tâche exécutable
 */
struct Task {
    TaskFunction function;     // Fonction à exécuter
    uint32_t interval;         // Intervalle en microsecondes
    uint32_t lastRun;          // Dernière exécution (micros)
    uint32_t executionTime;    // Temps d'exécution (moyenne)
    uint8_t priority;          // Priorité (0 = plus haute)
    bool enabled;              // Tâche activée ?
    const char* name;          // Nom de la tâche pour le débogage

    Task(TaskFunction func, uint32_t inter, uint8_t prio, const char* taskName)
        : function(func), interval(inter), lastRun(0), executionTime(0), 
          priority(prio), enabled(true), name(taskName) {}
};

/**
 * @brief Gestionnaire de tâches avec ordonnancement par priorité et budget CPU
 */
class TaskScheduler {
public:
    TaskScheduler();
    
    /**
     * @brief Ajoute une tâche à l'ordonnanceur
     * @param func Fonction à exécuter
     * @param intervalMicros Intervalle en microsecondes
     * @param priority Priorité (0 = plus haute)
     * @param name Nom de la tâche pour le débogage
     * @return Indice de la tâche
     */
    int addTask(TaskFunction func, uint32_t intervalMicros, uint8_t priority, const char* name);
    
    /**
     * @brief Exécute les tâches planifiées en respectant le budget CPU
     * @param maxMicros Budget CPU maximum par cycle
     */
    void update(uint32_t maxMicros = 2000);
    
    /**
     * @brief Active ou désactive une tâche
     * @param taskIndex Indice de la tâche
     * @param enabled État d'activation
     */
    void enableTask(int taskIndex, bool enabled);
    
    /**
     * @brief Modifie l'intervalle d'une tâche
     * @param taskIndex Indice de la tâche
     * @param intervalMicros Nouvel intervalle en microsecondes
     */
    void setTaskInterval(int taskIndex, uint32_t intervalMicros);
    
    /**
     * @brief Exécute immédiatement une tâche, indépendamment de son intervalle
     * @param taskIndex Indice de la tâche
     */
    void runTaskNow(int taskIndex);
    
    /**
     * @brief Renvoie des statistiques sur l'utilisation du CPU
     * @return Pourcentage d'utilisation CPU (0-100)
     */
    float getCpuUsage() const;
    
    /**
     * @brief Nombre de tâches gérées par l'ordonnanceur
     * @return Nombre de tâches
     */
    size_t getTaskCount() const;
    
    /**
     * @brief Renvoie le nombre de cycles d'exécution
     * @return Nombre total de cycles
     */
    uint32_t getCycleCount() const { return cycleCount; }
    
    /**
     * @brief Renvoie le nombre de dépassements de budget CPU
     * @return Nombre de dépassements
     */
    uint32_t getOverruns() const { return overruns; }

    /**
     * @brief Affiche les statistiques de performance en mode debug
     */
    void printDebugStats();
    
    /**
     * @brief Affiche les statistiques sur le port série
     * @param showDetailedStats Afficher les détails des tâches
     */
    void printStats(bool showDetailedStats = false);

private:
    std::vector<Task> tasks;
    uint32_t cycleStartTime;
    uint32_t totalExecutionTime;
    uint32_t cpuUsage;         // en pourcentage * 100 (pour précision)
    
    // Métriques pour le diagnostic
    uint32_t overruns;         // Nombre de cycles ayant dépassé le budget
    uint32_t cycleCount;       // Nombre total de cycles
    
    /**
     * @brief Trie les tâches par priorité
     */
    void sortTasksByPriority();
    
    /**
     * @brief Exécute une tâche et mesure son temps d'exécution
     * @param taskIndex Indice de la tâche
     * @return Temps d'exécution en microsecondes
     */
    uint32_t executeTask(int taskIndex);
};

// Singleton global pour faciliter l'accès
extern TaskScheduler scheduler;

#endif // TASK_SCHEDULER_HPP