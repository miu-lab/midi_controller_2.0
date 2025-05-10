// config/debug/DebugConfig.hpp
#pragma once

//---------------------------------------------------------------------------
// Définitions des niveaux de débogage (ne pas modifier)
//---------------------------------------------------------------------------
#define DEBUG_LEVEL_NONE 0     // Aucun message de débogage
#define DEBUG_LEVEL_ERROR 1    // Erreurs seulement
#define DEBUG_LEVEL_WARNING 2  // Erreurs + avertissements
#define DEBUG_LEVEL_INFO 3     // Niveau informatif général
#define DEBUG_LEVEL_DEBUG 4    // Informations détaillées pour le développement
#define DEBUG_LEVEL_TRACE 5    // Suivi d'exécution très détaillé

//---------------------------------------------------------------------------
// Configuration des environnements de débogage (via platformio.ini)
//---------------------------------------------------------------------------

// PRODUCTION: Débogage minimal (défini dans platformio.ini comme CONFIG_PRODUCTION)
#if defined(CONFIG_PRODUCTION)
#undef DEBUG  // Assurer que DEBUG n'est pas défini en production
#define DEBUG_LEVEL DEBUG_LEVEL_ERROR
#define DEBUG_RAW_CONTROLS 0
#define DEBUG_EVENT_BUS_ENABLED 0
#define DEBUG_TASK_SCHEDULER_LEVEL 0  // Désactiver toutes les statistiques
#define DEBUG_SCHEDULER_LEVEL DEBUG_LEVEL_NONE  // Définir à NONE en production

// DEVELOPMENT: Débogage standard (défini dans platformio.ini comme CONFIG_DEVELOPMENT)
#elif defined(CONFIG_DEVELOPMENT)
#ifndef DEBUG
#define DEBUG  // Activer DEBUG s'il n'est pas déjà défini
#endif
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#define DEBUG_RAW_CONTROLS 1  // Débogage léger des contrôles
#define DEBUG_EVENT_BUS_ENABLED 1
#define DEBUG_TASK_SCHEDULER_LEVEL 1  // Stats CPU basiques seulement
#define DEBUG_SCHEDULER_LEVEL DEBUG_LEVEL_INFO  // Définir le niveau pour l'ordonnanceur

// VERBOSE: Débogage maximal (défini dans platformio.ini comme CONFIG_VERBOSE)
#elif defined(CONFIG_VERBOSE)
#ifndef DEBUG
#define DEBUG  // Activer DEBUG s'il n'est pas déjà défini
#endif
#define DEBUG_LEVEL DEBUG_LEVEL_DEBUG
#define DEBUG_RAW_CONTROLS 2  // Débogage complet des contrôles
#define DEBUG_EVENT_BUS_ENABLED 1
#ifndef DEBUG_TASK_SCHEDULER_LEVEL
#define DEBUG_TASK_SCHEDULER_LEVEL 2  // Stats CPU complètes (basiques + détaillées)
#endif
#define DEBUG_SCHEDULER_LEVEL DEBUG_LEVEL_DEBUG  // Définir le niveau pour l'ordonnanceur

// Configuration par défaut (si aucun environnement n'est spécifié)
#else
// Activer DEBUG uniquement s'il n'est pas déjà défini ou explicitement désactivé
#if !defined(DEBUG) && !defined(NDEBUG)
#define DEBUG
#endif

// Définir les valeurs par défaut seulement si elles ne sont pas déjà définies
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#endif

#ifndef DEBUG_RAW_CONTROLS
#define DEBUG_RAW_CONTROLS 0
#endif

#ifndef DEBUG_EVENT_BUS_ENABLED
#define DEBUG_EVENT_BUS_ENABLED 0
#endif

// Niveau de logs pour l'ordonnanceur (0=désactivé, 1=stats CPU basiques, 2=stats détaillées)
#ifndef DEBUG_TASK_SCHEDULER_LEVEL
#define DEBUG_TASK_SCHEDULER_LEVEL 1
#endif

// Niveau pour les messages du scheduler
#ifndef DEBUG_SCHEDULER_LEVEL
#define DEBUG_SCHEDULER_LEVEL DEBUG_LEVEL_INFO
#endif
#endif  // Ferme le bloc #if/#elif/#else

//---------------------------------------------------------------------------
// Inclusion des macros de débogage
//---------------------------------------------------------------------------
// Les macros de débogage sont dans DebugMacros.hpp qui inclut ce fichier
