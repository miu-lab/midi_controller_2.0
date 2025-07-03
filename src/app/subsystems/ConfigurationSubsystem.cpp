#include "ConfigurationSubsystem.hpp"

#include <Arduino.h>

#include "config/ConfigDefaults.hpp"
#include "core/configuration/ConfigurationLoader.hpp"
#include "core/configuration/ConfigurationService.hpp"

ConfigurationSubsystem::ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    navService_ = std::make_shared<NavigationConfigService>();
    configLoader_ = std::make_unique<ConfigurationLoader>();
    configRegistry_ = std::make_unique<ConfigurationRegistry>(container);
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

    // Utiliser ConfigurationRegistry pour enregistrer les dépendances
    if (configRegistry_) {
        auto thisPtr = std::shared_ptr<ConfigurationSubsystem>(this, [](ConfigurationSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        });
        
        configRegistry_->registerConfigurationSubsystem(
            std::static_pointer_cast<void>(thisPtr));
        
        if (config_) {
            configRegistry_->registerUnifiedConfiguration(config_);
        }
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