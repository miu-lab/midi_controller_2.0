#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "app/di/DependencyContainer.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Sous-système de configuration
 * 
 * Responsable de charger et gérer toutes les configurations du système.
 * Interface unifiée basée sur ControlDefinition.
 */
class ConfigurationSubsystem : public IConfiguration {
public:
    explicit ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container);
    ~ConfigurationSubsystem() = default;

    /**
     * @brief Initialise le sous-système de configuration
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> init() override;

    const std::vector<ControlDefinition>& getAllControlDefinitions() const override;
    std::vector<ControlDefinition> getControlDefinitionsByType(InputType type) const override;
    std::optional<ControlDefinition> getControlDefinitionById(InputId id) const override;
    std::vector<ControlDefinition> getControlDefinitionsByGroup(const std::string& group) const override;

    bool isNavigationControl(InputId id) const override;
    void setControlForNavigation(InputId id, bool isNavigation = true) override;

    bool isDebugEnabled() const override;
    int midiChannel() const override;
    bool isHardwareInitEnabled() const override;
    
    bool validateAllConfigurations() const override;
    std::vector<std::string> getAvailableGroups() const override;
    size_t getInputCountByType(InputType type) const override;

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<ApplicationConfiguration> config_;
    std::shared_ptr<NavigationConfigService> navService_;

    Result<bool, std::string> loadUnifiedConfigurations();
};
