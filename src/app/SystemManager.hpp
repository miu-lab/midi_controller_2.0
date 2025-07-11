#pragma once

#include <Arduino.h>

#include <memory>

#include "app/MidiControllerApp.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "config/SystemConstants.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Gestionnaire système pour le contrôleur MIDI
 *
 * Cette classe centralise toute la logique de gestion d'erreurs,
 * de récupération et d'états du système, gardant main.cpp simple.
 */
class SystemManager {
public:
    /**
     * @brief États possibles du système
     */
    enum class State { UNINITIALIZED, INITIALIZING, RUNNING, ERROR_STATE, RECOVERY_MODE };

    /**
     * @brief Constructeur
     */
    SystemManager();

    /**
     * @brief Destructeur
     */
    ~SystemManager() = default;

    /**
     * @brief Initialise le système complet
     * @return Result<void> Succès ou erreur détaillée
     */
    Result<void> initialize();

    /**
     * @brief Met à jour le système (appelé dans loop)
     */
    void update();

    /**
     * @brief Obtient l'état actuel du système
     * @return State État actuel
     */
    State getCurrentState() const {
        return currentState_;
    }

    /**
     * @brief Force l'arrêt du système
     */
    void shutdown();

private:
    // Configuration et composants principaux
    ApplicationConfiguration appConfig_;
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<MidiControllerApp> app_;

    // Gestion d'états et récupération
    State currentState_;
    unsigned long lastErrorTime_;
    static constexpr unsigned long ERROR_RECOVERY_DELAY = 5000;  // 5 secondes


    /**
     * @brief Initialise le système avec gestion d'erreurs robuste
     * @return Result<void> Succès ou erreur détaillée
     */
    Result<void> performInitialization();

    /**
     * @brief Entre en mode de récupération d'erreur
     */
    void enterRecoveryMode();

    /**
     * @brief Tente une récupération du système
     * @return Result<void> Succès ou échec de la récupération
     */
    Result<void> attemptRecovery();

    /**
     * @brief Met à jour le système en état RUNNING
     */
    void updateRunningState();

    /**
     * @brief Met à jour le système en mode RECOVERY
     */
    void updateRecoveryMode();

    /**
     * @brief Affiche une erreur de manière détaillée
     * @param context Contexte de l'erreur
     * @param error Erreur à afficher
     */
    void logError(const char* context, const Error& error);

    /**
     * @brief Cleanup des ressources
     */
    void cleanup();
};