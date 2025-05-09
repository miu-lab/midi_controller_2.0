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
