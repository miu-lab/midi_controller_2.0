#pragma once

#include <memory>
#include "core/domain/interfaces/IConfiguration.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/di/DependencyContainer.hpp"

/**
 * @brief Sous-système de gestion des configurations
 * 
 * Cette classe implémente l'interface IConfiguration et centralise toutes
 * les configurations de l'application.
 */
class ConfigurationSubsystem : public IConfiguration {
public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param container Conteneur de dépendances
     */
    explicit ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container);
    
    /**
     * @brief Destructeur par défaut
     */
    ~ConfigurationSubsystem() override = default;
    
    /**
     * @brief Initialise le sous-système de configuration
     */
    void init();
    
    // Implémentation de IConfiguration
    bool isNavigationControl(ControlId id) const override;
    void setControlForNavigation(ControlId id, bool isNavigation = true) override;
    const std::vector<EncoderConfig>& getEncoderConfigs() const override;
    const std::vector<ButtonConfig>& getButtonConfigs() const override;
    bool isDebugEnabled() const override;
    int getMidiChannel() const override;
    bool isHardwareInitEnabled() const override;
    
private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<ApplicationConfiguration> config_;
    std::shared_ptr<NavigationConfigService> navService_;
    std::vector<EncoderConfig> encoderConfigs_;
    std::vector<ButtonConfig> buttonConfigs_;
    
    /**
     * @brief Charge les configurations des encodeurs depuis ApplicationConfiguration
     */
    void loadEncoderConfigs();
    
    /**
     * @brief Charge les configurations des boutons depuis ApplicationConfiguration
     */
    void loadButtonConfigs();
};