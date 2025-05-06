// config/debug/DebugConfig.hpp
#pragma once

// Configuration de débogage
// 1. Débogage des contrôles physiques de bas niveau (encodeurs, boutons)
// 0 = Désactivé, 1 = Léger (seulement les mouvements), 2 = Complet (tous les appels)
#define DEBUG_RAW_CONTROLS 0

// 2. Débogage du bus d'événements (événements publiés/reçus)
// 0 = Désactivé, 1 = Activé
#define DEBUG_EVENT_BUS 0
