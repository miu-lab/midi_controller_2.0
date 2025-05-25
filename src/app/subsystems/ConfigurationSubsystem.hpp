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

// Forward declarations
// class HardwareConfiguration;  // SUPPRIMÉ - Plus nécessaire avec le système unifié

/**
 * @brief Sous-système de configuration
 * 
 * Responsable de charger et gérer toutes les configurations du système.
 * Interface unifiée basée sur InputConfig.
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

    // === INTERFACE UNIFIÉE ===
    
    const std::vector<InputConfig>& getAllInputConfigurations() const override;
    std::vector<InputConfig> getInputConfigurationsByType(InputType type) const override;
    std::optional<InputConfig> getInputConfigurationById(InputId id) const override;
    std::vector<InputConfig> getInputConfigurationsByGroup(const std::string& group) const override;

    // === MÉTHODES DE NAVIGATION ===
    
    bool isNavigationControl(InputId id) const override;
    void setControlForNavigation(InputId id, bool isNavigation = true) override;

    // === MÉTHODES DE CONFIGURATION GÉNÉRALE ===
    
    bool isDebugEnabled() const override;
    int midiChannel() const override;
    bool isHardwareInitEnabled() const override;
    
    // === MÉTHODES UTILITAIRES ===
    
    bool validateAllConfigurations() const override;
    std::vector<std::string> getAvailableGroups() const override;
    size_t getInputCountByType(InputType type) const override;

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<ApplicationConfiguration> config_;
    std::shared_ptr<NavigationConfigService> navService_;
    // std::shared_ptr<HardwareConfiguration> hardwareConfig_;  // SUPPRIMÉ - Migration vers système unifié

    // Méthodes privées d'initialisation
    Result<bool, std::string> loadUnifiedConfigurations();
};
