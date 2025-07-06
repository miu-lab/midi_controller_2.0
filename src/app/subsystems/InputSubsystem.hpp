#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <set>

#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/INavigationService.hpp"
#include "app/services/InputManagerService.hpp"
#include "core/domain/interfaces/IInputManager.hpp"
#include "core/factories/ControllerFactory.hpp"
#include "core/utils/Result.hpp"

class InputController;

/**
 * @brief Sous-système de gestion des entrées
 *
 * REFACTOR: Responsabilité étendue pour inclure la gestion des contrôles de navigation.
 * Cette classe implémente l'interface IInputSystem et délègue
 * la gestion des entrées à InputManager et ControllerFactory.
 * 
 * Nouvelles responsabilités:
 * - Configuration des contrôles de navigation
 * - Synchronisation avec INavigationService
 * - Séparation logique Input vs Navigation
 * 
 * Respecte le principe de responsabilité unique étendu pour les inputs.
 */
class InputSubsystem : public IInputSystem {
public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param container Conteneur de dépendances
     */
    explicit InputSubsystem(std::shared_ptr<DependencyContainer> container);

    /**
     * @brief Destructeur par défaut
     */
    ~InputSubsystem() override = default;

    /**
     * @brief Initialise le sous-système d'entrée
     * @return Result<bool> Succès ou message d'erreur
     */
    Result<bool> init() override;

    /**
     * @brief Met à jour l'état des entrées
     */
    void update() override;

    Result<bool> configureInputs(const std::vector<ControlDefinition>& controlDefinitions) override;
    std::vector<ControlDefinition> getAllActiveControlDefinitions() const override;
    std::optional<ControlDefinition> getControlDefinitionById(InputId id) const override;
    size_t getActiveInputCountByType(InputType type) const override;
    Result<void> validateInputsStatus() const override;

    /**
     * @brief Configure les contrôles de navigation 
     * 
     * REFACTOR: Nouvelle méthode pour gérer les contrôles de navigation
     * directement dans InputSubsystem au lieu de MidiSubsystem.
     * 
     * @param controlDefinitions Toutes les définitions de contrôles
     * @return Result<bool> Succès ou erreur de configuration
     */
    Result<bool> configureNavigationControls(const std::vector<ControlDefinition>& controlDefinitions);

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<IConfiguration> configuration_;
    std::shared_ptr<INavigationService> navigationService_;
    
    // Composants délégués
    std::shared_ptr<InputManagerService> inputManager_;
    std::unique_ptr<ControllerFactory> controllerFactory_;
    std::shared_ptr<InputController> inputController_;

    bool initialized_ = false;
    
    /**
     * @brief Initialise les composants délégués
     * @return Result<bool> Succès ou erreur
     */
    Result<bool> initializeDelegatedComponents();

    /**
     * @brief Configure les entrées via InputManager
     * @param controlDefinitions Définitions des contrôles
     * @return Result<bool> Succès ou erreur
     */
    Result<bool> setupInputManager(const std::vector<ControlDefinition>& controlDefinitions);
};
