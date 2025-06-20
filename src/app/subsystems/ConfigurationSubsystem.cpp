#include "ConfigurationSubsystem.hpp"

#include <Arduino.h>
#include <algorithm>
#include <set>

#include "config/ConfigDefaults.hpp"

ConfigurationSubsystem::ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    navService_ = std::make_shared<NavigationConfigService>();
}

Result<bool, std::string> ConfigurationSubsystem::init() {
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

    // Enregistrer ce sous-système comme implémentation de IConfiguration
    container_->registerImplementation<IConfiguration, ConfigurationSubsystem>(
        std::shared_ptr<ConfigurationSubsystem>(this, [](ConfigurationSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        }));

    return Result<bool, std::string>::success(true);
}

const std::vector<ControlDefinition>& ConfigurationSubsystem::getAllControlDefinitions() const {
    if (config_) {
        const auto& unifiedConfig = config_->getUnifiedConfiguration();
        return unifiedConfig.getAllControls();
    }
    
    static std::vector<ControlDefinition> emptyControls;
    return emptyControls;
}

std::vector<ControlDefinition> ConfigurationSubsystem::getControlDefinitionsByType(InputType type) const {
    const auto& allControls = getAllControlDefinitions();
    std::vector<ControlDefinition> filtered;
    
    for (const auto& control : allControls) {
        if (control.hardware.type == type) {
            filtered.push_back(control);
        }
    }
    
    return filtered;
}

std::optional<ControlDefinition> ConfigurationSubsystem::getControlDefinitionById(InputId id) const {
    const auto& allControls = getAllControlDefinitions();
    
    auto it = std::find_if(allControls.begin(), allControls.end(),
        [id](const ControlDefinition& control) {
            return control.id == id;
        });
    
    if (it != allControls.end()) {
        return *it;
    }
    
    return std::nullopt;
}

std::vector<ControlDefinition> ConfigurationSubsystem::getControlDefinitionsByGroup(const std::string& group) const {
    const auto& allControls = getAllControlDefinitions();
    std::vector<ControlDefinition> filtered;
    
    for (const auto& control : allControls) {
        if (control.group == group) {
            filtered.push_back(control);
        }
    }
    
    return filtered;
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
    if (config_) {
        const auto& unifiedConfig = config_->getUnifiedConfiguration();
        return unifiedConfig.validate();
    }
    return false;
}

std::vector<std::string> ConfigurationSubsystem::getAvailableGroups() const {
    const auto& allControls = getAllControlDefinitions();
    std::set<std::string> uniqueGroups;
    
    for (const auto& control : allControls) {
        uniqueGroups.insert(control.group);
    }
    
    return std::vector<std::string>(uniqueGroups.begin(), uniqueGroups.end());
}

size_t ConfigurationSubsystem::getInputCountByType(InputType type) const {
    const auto& allControls = getAllControlDefinitions();
    return std::count_if(allControls.begin(), allControls.end(), 
                        [type](const ControlDefinition& control) {
                            return control.hardware.type == type;
                        });
}

Result<bool, std::string> ConfigurationSubsystem::loadUnifiedConfigurations() {
    Serial.println(F("ConfigurationSubsystem: Loading control definitions"));
    
    if (!config_) {
        return Result<bool, std::string>::error("ApplicationConfiguration not available");
    }
    
    if (!validateAllConfigurations()) {
        return Result<bool, std::string>::error("Some control definitions are invalid");
    }
    
    const auto& unifiedConfig = config_->getUnifiedConfiguration();
    auto stats = unifiedConfig.getStats();
    Serial.print(stats.totalControls);
    Serial.println(F(" control definitions loaded"));
    
    Serial.println(F("ConfigurationSubsystem: Using unified configuration system"));
    
    Serial.print(F("Stats - Encoders: "));
    Serial.print(stats.encoders);
    Serial.print(F(", Buttons: "));
    Serial.print(stats.buttons);
    Serial.print(F(", MIDI mappings: "));
    Serial.println(stats.midiMappings);
    
    return Result<bool, std::string>::success(true);
}
