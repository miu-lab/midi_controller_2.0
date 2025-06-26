#pragma once

#include <memory>

#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/utils/Result.hpp"

// Déclarations anticipées pour réduire les inclusions
class NavigationConfigService;
class CommandManager;
class InputController;
class InputSubsystem;
class MidiSubsystem;
class UISubsystem;
class MenuController;
class UIController;

/**
 * @brief Script d'initialisation du système
 */
class InitializationScript {
public:
    /**
     * @brief Initialise le conteneur de dépendances avec tous les composants requis
     */
    static Result<bool, std::string> initializeContainer(
        std::shared_ptr<DependencyContainer> container, const ApplicationConfiguration& config);

private:
    // Structure de données pour les initialisations des sous-systèmes
    struct SubsystemInfo {
        const char* name;
        std::shared_ptr<void> instance;
        std::function<Result<bool, std::string>()> initFn;
    };

    // Étapes d'initialisation ordonnées
    static void registerBaseServices(std::shared_ptr<DependencyContainer> container,
                                     const ApplicationConfiguration& config);

    static Result<bool, std::string> setupHardwareAdapters(
        std::shared_ptr<DependencyContainer> container);

    static Result<bool, std::string> initializeSubsystems(
        std::shared_ptr<DependencyContainer> container);

    static bool setupControllers(std::shared_ptr<DependencyContainer> container);

    // Configuration des écouteurs MIDI prioritaires
    static void setupMidiEventListeners(std::shared_ptr<DependencyContainer> container);
};
