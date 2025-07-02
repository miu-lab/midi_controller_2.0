#pragma once

#include <Arduino.h>
#include <string>

  // Pour avoir accès à PERFORMANCE_MODE
#include "core/TaskScheduler.hpp"
#include "core/utils/Result.hpp"  // Pour avoir accès à Result

/**
 * @brief Module de diagnostics pour le débogage et le monitoring
 * Ce module n'a pas d'impact sur les performances en mode production
 */
class DiagnosticsManager {
public:
    /**
     * @brief Initialise le gestionnaire de diagnostics
     * @param scheduler Référence vers l'ordonnanceur de tâches
     * @param interval Intervalle d'affichage en secondes
     */
    static void init(TaskScheduler& scheduler, uint16_t interval = 10);

    /**
     * @brief Doit être appelé régulièrement dans la boucle principale
     * Ne fait rien en mode production
     */
    static void update();

    /**
     * @brief Affiche les statistiques maintenant (même en mode production)
     * @param showDetails Affiche les détails des tâches
     */
    static void printStats(bool showDetails = false);

    /**
     * @brief Déclenche les diagnostics sur un événement spécifique
     * @param eventName Nom de l'événement qui a déclenché les diagnostics
     * @param showDetails Afficher les détails des tâches
     */
    static void onEvent(const char* eventName, bool showDetails = false);

    /**
     * @brief Active ou désactive l'affichage régulier des statistiques
     * Ne fait rien en mode production
     */
    static void enableRegularStats(bool enable);

    /**
     * @brief Définit l'intervalle d'affichage des statistiques
     * @param seconds Intervalle en secondes
     */
    static void setStatsInterval(uint16_t seconds);

    /**
     * @brief Traite les commandes de diagnostics venant du port série
     * Cette fonction doit être appelée depuis handleSerialCommands()
     * @param command Commande à traiter
     * @return true si la commande a été traitée, false sinon
     */
    static bool handleCommand(const String& command);

    /**
     * @brief Active ou désactive les diagnostics basés sur les événements
     * @param enable État d'activation
     */
    static void enableEventDiagnostics(bool enable);

    /**
     * @brief Affiche un message d'erreur provenant d'un objet Result
     * @param result L'objet Result contenant l'erreur potentielle
     * @param prefix Préfixe à afficher avant le message d'erreur
     */
    static void printError(const Result<bool>& result, const char* prefix);

private:
    static TaskScheduler* _scheduler;
    static unsigned long _lastStatsTime;
    static uint16_t _statsInterval;
    static bool _regularStatsEnabled;
    static bool _eventDiagnosticsEnabled;

    static void printSchedulerStats(bool showDetails);
    static void printMemoryStats();
};

/**
 * @brief Macro pour simplifier l'utilisation du diagnostics dans le code
 * @param scheduler Référence à l'ordonnanceur de tâches
 */
#if defined(DEBUG) && !defined(PERFORMANCE_MODE)
// Mode DEBUG normal (sans mode performance)
#define INIT_DIAGNOSTICS(scheduler, interval) DiagnosticsManager::init(scheduler, interval)
#define UPDATE_DIAGNOSTICS() DiagnosticsManager::update()
#define PRINT_DIAGNOSTICS(showDetails) DiagnosticsManager::printStats(showDetails)
#define DIAG_ON_EVENT(eventName) DiagnosticsManager::onEvent(eventName)
#define PRINT_ERROR(result, prefix) DiagnosticsManager::printError(result, prefix)
#else
// Mode production ou mode performance - pas de diagnostics
#define INIT_DIAGNOSTICS(scheduler, interval) \
    do {                                      \
    } while (0)
#define UPDATE_DIAGNOSTICS() \
    do {                     \
    } while (0)
#define PRINT_DIAGNOSTICS(showDetails) \
    do {                               \
    } while (0)
#define DIAG_ON_EVENT(eventName) \
    do {                         \
    } while (0)
#define PRINT_ERROR(result, prefix) \
    do {                            \
    } while (0)
#endif

// Fonction globale pour accéder aux diagnostics depuis n'importe où
inline void printDiagnostics(bool showDetails = false) {
#ifndef PERFORMANCE_MODE
    DiagnosticsManager::printStats(showDetails);
#endif
}

// Fonction globale pour afficher les erreurs
inline void printError(const Result<bool>& result, const char* prefix) {
#ifndef PERFORMANCE_MODE
    DiagnosticsManager::printError(result, prefix);
#endif
}