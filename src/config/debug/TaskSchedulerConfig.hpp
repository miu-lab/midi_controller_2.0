#pragma once

// Ne pas redéfinir DEBUG_TASK_SCHEDULER - il est déjà défini dans DebugConfig.hpp
// #define DEBUG_TASK_SCHEDULER 1

// Intervalle pour l'affichage des statistiques (en nombre de cycles)
#define TASK_SCHEDULER_STATS_INTERVAL 500

// Budget CPU par défaut par cycle (en microsecondes)
#define DEFAULT_CPU_BUDGET_MICROS 5000

// Nombre initial de tâches à allouer
#define INITIAL_TASK_COUNT 8

// Priorités prédéfinies pour le TaskScheduler
#define TASK_PRIORITY_CRITICAL    0   // Plus haute priorité
#define TASK_PRIORITY_HIGH        1
#define TASK_PRIORITY_NORMAL      5
#define TASK_PRIORITY_LOW        10
#define TASK_PRIORITY_BACKGROUND 20   // Plus basse priorité

// Budget temporel pour les différents sous-systèmes (en microsecondes)
namespace TaskTiming {
    static constexpr unsigned long MAX_INPUT_TIME_US = 1000;  // 1ms pour l'input (critique)
    static constexpr unsigned long MAX_MIDI_TIME_US = 2000;   // 2ms pour MIDI (critique)
    static constexpr unsigned long MAX_UI_TIME_US = 10000;    // 10ms pour l'UI (moins critique)
    
    // Période minimale d'exécution pour certaines tâches
    static constexpr unsigned long UI_MIN_PERIOD_MS = 20;     // 50Hz max pour l'UI
}
