#pragma once

#include <memory>
#include <vector>
#include <optional>

#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/input/InputManager.hpp"
#include "core/factories/ControllerFactory.hpp"
#include "core/utils/Result.hpp"

class InputController;

/**
 * @brief Sous-système de gestion des entrées
 *
 * Cette classe implémente l'interface IInputSystem et délègue
 * la gestion des entrées à InputManager et ControllerFactory.
 * Respecte le principe de responsabilité unique.
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

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<IConfiguration> configuration_;
    
    // Composants délégués
    std::unique_ptr<InputManager> inputManager_;
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
