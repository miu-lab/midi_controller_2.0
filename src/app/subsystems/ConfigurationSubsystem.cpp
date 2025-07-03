#include "ConfigurationSubsystem.hpp"

#include <Arduino.h>

#include <algorithm>
#include <set>

#include "config/ConfigDefaults.hpp"

#include "config/unified/UnifiedConfiguration.hpp"
#include "core/configuration/ConfigurationLoader.hpp"
#include "core/configuration/ConfigurationService.hpp"
#include "core/utils/Error.hpp"

ConfigurationSubsystem::ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    navService_ = std::make_shared<NavigationConfigService>();
    configLoader_ = std::make_unique<ConfigurationLoader>();
    // configService_ will be initialized after config_ is set
}

Result<bool> ConfigurationSubsystem::init() {
    // Récupérer la configuration de l'application depuis le conteneur
    config_ = container_->resolve<ApplicationConfiguration>();
    if (!config_) {
        // Si aucune configuration n'est trouvée, créer une configuration par défaut
        config_ = std::make_shared<ApplicationConfiguration>();
        container_->registerDependency<ApplicationConfiguration>(config_);
    }

    // Charger les configurations unifiées depuis ApplicationConfiguration
    auto unifiedResult = loadUnifiedConfigurations();
    if (unifiedResult.isError()) {
        return unifiedResult;
    }

    // Initialiser le service de configuration avec la config chargée
    configService_ = std::make_unique<ConfigurationService>(config_);

    // Enregistrer ce sous-système comme implémentation de IConfiguration
    container_->registerImplementation<IConfiguration, ConfigurationSubsystem>(
        std::shared_ptr<ConfigurationSubsystem>(this, [](ConfigurationSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        }));

    // Enregistrer UnifiedConfiguration pour l'accès direct
    if (config_) {
        auto unifiedConfigPtr = std::shared_ptr<UnifiedConfiguration>(
            const_cast<UnifiedConfiguration*>(&config_->getUnifiedConfiguration()),
            [](UnifiedConfiguration*) {
                // Custom deleter qui ne fait rien car on ne possède pas l'objet
            });
        container_->registerDependency<UnifiedConfiguration>(unifiedConfigPtr);
        // TODO DEBUG MSG
    }

    return Result<bool>::success(true);
}

const std::vector<ControlDefinition>& ConfigurationSubsystem::getAllControlDefinitions() const {
    if (configService_) {
        return configService_->getAllControlDefinitions();
    }
    
    static std::vector<ControlDefinition> emptyControls;
    return emptyControls;
}

std::vector<ControlDefinition> ConfigurationSubsystem::getControlDefinitionsByType(InputType type) const {
    if (configService_) {
        return configService_->getControlDefinitionsByType(type);
    }
    
    return std::vector<ControlDefinition>();
}

std::optional<ControlDefinition> ConfigurationSubsystem::getControlDefinitionById(InputId id) const {
    if (configService_) {
        return configService_->getControlDefinitionById(id);
    }
    
    return std::nullopt;
}

std::vector<ControlDefinition> ConfigurationSubsystem::getControlDefinitionsByGroup(const std::string& group) const {
    if (configService_) {
        return configService_->getControlDefinitionsByGroup(group);
    }
    
    return std::vector<ControlDefinition>();
}

// === MÉTHODES DE NAVIGATION ===

bool ConfigurationSubsystem::isNavigationControl(InputId id) const {
    return navService_->isNavigationControl(id);
}

void ConfigurationSubsystem::setControlForNavigation(InputId id, bool isNavigation) {
    navService_->setControlForNavigation(id, isNavigation);
}

bool ConfigurationSubsystem::isDebugEnabled() const {
    return ConfigDefaults::SHOW_DEBUG_INFO;
}

int ConfigurationSubsystem::midiChannel() const {
    return ConfigDefaults::DEFAULT_MIDI_CHANNEL;
}

bool ConfigurationSubsystem::isHardwareInitEnabled() const {
    return true;
}

bool ConfigurationSubsystem::validateAllConfigurations() const {
    if (configService_) {
        return configService_->validateAllConfigurations();
    }
    
    return false;
}

std::vector<std::string> ConfigurationSubsystem::getAvailableGroups() const {
    if (configService_) {
        return configService_->getAvailableGroups();
    }
    
    return std::vector<std::string>();
}

size_t ConfigurationSubsystem::getInputCountByType(InputType type) const {
    if (configService_) {
        return configService_->getInputCountByType(type);
    }
    
    return 0;
}

Result<bool> ConfigurationSubsystem::loadUnifiedConfigurations() {
    // Delegate to ConfigurationLoader
    return configLoader_->loadUnifiedConfigurations(config_);
}