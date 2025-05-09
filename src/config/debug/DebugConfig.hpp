// config/debug/DebugConfig.hpp
#pragma once

// Active le débogage global de l'application
#define DEBUG

// Configuration de débogage
// 1. Débogage des contrôles physiques de bas niveau (encodeurs, boutons)
// 0 = Désactivé, 1 = Léger (seulement les mouvements), 2 = Complet (tous les appels)
#define DEBUG_RAW_CONTROLS 0

// 2. Débogage du bus d'événements (événements publiés/reçus)
// 0 = Désactivé, 1 = Activé
#define DEBUG_EVENT_BUS_ENABLED 0

// 3. Débogage de l'ordonnanceur de tâches (statistiques de tâches)
// 0 = Désactivé, 1 = Activé
#define DEBUG_TASK_SCHEDULER 0

// 4. Niveau de débogage global
// Valeurs possibles : DEBUG_LEVEL_NONE, DEBUG_LEVEL_ERROR, DEBUG_LEVEL_WARNING, 
//                  DEBUG_LEVEL_INFO, DEBUG_LEVEL_DEBUG, DEBUG_LEVEL_TRACE
// Par défaut : DEBUG_LEVEL_INFO
#define DEBUG_LEVEL DEBUG_LEVEL_INFO

// Inclure les macros optimisées
#include "config/debug/DebugMacros.hpp"

