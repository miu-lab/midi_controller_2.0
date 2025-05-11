#pragma once

#include <memory>
#include <vector>
#include "core/domain/interfaces/IInputSystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "adapters/secondary/hardware/encoders/EncoderConfig.hpp"
#include "adapters/secondary/hardware/buttons/DigitalButtonConfig.hpp"
#include "core/use_cases/ProcessEncoders.hpp"
#include "core/use_cases/ProcessButtons.hpp"
#include "core/utils/Result.hpp"

class EncoderManager;
class DigitalButtonManager;
class InputController;

/**
 * @brief Sous-système de gestion des entrées
 * 
 * Cette classe implémente l'interface IInputSystem et gère toutes
 * les entrées utilisateur (encodeurs, boutons).
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
     * 
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> init();
    
    /**
     * @brief Met à jour l'état des entrées
     */
    void update() override;
    
    /**
     * @brief Configure les encodeurs avec les paramètres fournis
     * @param encoderConfigs Configurations des encodeurs
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> configureEncoders(const std::vector<EncoderConfig>& encoderConfigs);
    
    /**
     * @brief Configure les boutons avec les paramètres fournis
     * @param buttonConfigs Configurations des boutons
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> configureButtons(const std::vector<ButtonConfig>& buttonConfigs);
    
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
};