// config/debug/DebugConfig.hpp
#pragma once

//---------------------------------------------------------------------------
// DEBUG COMPLÈTEMENT DÉSACTIVÉ
//---------------------------------------------------------------------------

// Toujours désactiver DEBUG dans tous les environnements
#ifdef DEBUG
#undef DEBUG
#endif

// Définir NDEBUG pour désactiver les assertions
#ifndef NDEBUG
#define NDEBUG
#endif

// Niveaux de debug désactivés
#define DEBUG_LEVEL 0
#define DEBUG_LEVEL_INFO 0
#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_WARNING 0

// Niveaux scheduler désactivés
#define SCHEDULER_DEBUG_LEVEL 0
#define SCHEDULER_MSG_LEVEL 0

//---------------------------------------------------------------------------
// Inclusion des macros de débogage
//---------------------------------------------------------------------------
// Les macros de débogage sont dans DebugMacros.hpp