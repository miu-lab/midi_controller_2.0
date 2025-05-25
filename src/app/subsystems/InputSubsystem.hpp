#pragma once

#include <memory>
#include <vector>
#include <optional>

#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderConfig.hpp"
#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/use_cases/ProcessButtons.hpp"
#include "core/use_cases/ProcessEncoders.hpp"
#include "core/utils/Result.hpp"

class EncoderManager;
class DigitalButtonManager;
class InputController;

/**
 * @brief Sous-système de gestion des entrées
 *
 * Cette classe implémente l'interface IInputSystem et gère toutes
 * les entrées utilisateur (encodeurs, boutons).
 * Interface unifiée basée sur ControlDefinition.
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
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> init() override;

    /**
     * @brief Met à jour l'état des entrées
     */
    void update() override;

    Result<bool, std::string> configureInputs(const std::vector<ControlDefinition>& controlDefinitions) override;
    std::vector<ControlDefinition> getAllActiveControlDefinitions() const override;
    std::optional<ControlDefinition> getControlDefinitionById(InputId id) const override;
    size_t getActiveInputCountByType(InputType type) const override;
    bool validateInputsStatus() const override;

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<IConfiguration> configuration_;
    std::shared_ptr<EncoderManager> encoderManager_;
    std::shared_ptr<DigitalButtonManager> buttonManager_;
    std::shared_ptr<InputController> inputController_;

    // Processeurs d'événements
    std::unique_ptr<ProcessEncoders> processEncoders_;
    std::unique_ptr<ProcessButtons> processButtons_;

    bool initialized_ = false;
    
    Result<bool, std::string> loadUnifiedConfigurations();
    std::vector<EncoderConfig> extractEncoderConfigs(const std::vector<ControlDefinition>& controlDefinitions) const;
    std::vector<ButtonConfig> extractButtonConfigs(const std::vector<ControlDefinition>& controlDefinitions) const;
    Result<bool, std::string> createManagers(const std::vector<EncoderConfig>& encoderConfigs,
                                            const std::vector<ButtonConfig>& buttonConfigs);
    Result<bool, std::string> initializeProcessors();
    Result<bool, std::string> connectInputController();
};
