#include "ConfigurationService.hpp"
#include <algorithm>
#include <set>

ConfigurationService::ConfigurationService(std::shared_ptr<ApplicationConfiguration> appConfig)
    : appConfig_(appConfig) {
}

void ConfigurationService::setApplicationConfiguration(std::shared_ptr<ApplicationConfiguration> appConfig) {
    appConfig_ = appConfig;
}

const std::vector<ControlDefinition>& ConfigurationService::getAllControlDefinitions() const {
    if (appConfig_) {
        const auto& unifiedConfig = appConfig_->getUnifiedConfiguration();
        return unifiedConfig.getAllControls();
    }
    
    static std::vector<ControlDefinition> emptyControls;
    return emptyControls;
}

std::vector<ControlDefinition> ConfigurationService::getControlDefinitionsByType(InputType type) const {
    const auto& allControls = getAllControlDefinitions();
    std::vector<ControlDefinition> filtered;
    
    for (const auto& control : allControls) {
        if (control.hardware.type == type) {
            filtered.push_back(control);
        }
    }
    
    return filtered;
}

std::optional<ControlDefinition> ConfigurationService::getControlDefinitionById(InputId id) const {
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

std::vector<ControlDefinition> ConfigurationService::getControlDefinitionsByGroup(const std::string& group) const {
    const auto& allControls = getAllControlDefinitions();
    std::vector<ControlDefinition> filtered;
    
    for (const auto& control : allControls) {
        if (control.group == group) {
            filtered.push_back(control);
        }
    }
    
    return filtered;
}

std::vector<std::string> ConfigurationService::getAvailableGroups() const {
    const auto& allControls = getAllControlDefinitions();
    std::set<std::string> uniqueGroups;
    
    for (const auto& control : allControls) {
        uniqueGroups.insert(control.group);
    }
    
    return std::vector<std::string>(uniqueGroups.begin(), uniqueGroups.end());
}

size_t ConfigurationService::getInputCountByType(InputType type) const {
    const auto& allControls = getAllControlDefinitions();
    return std::count_if(allControls.begin(), allControls.end(), 
                        [type](const ControlDefinition& control) {
                            return control.hardware.type == type;
                        });
}

bool ConfigurationService::validateAllConfigurations() const {
    if (appConfig_) {
        const auto& unifiedConfig = appConfig_->getUnifiedConfiguration();
        return unifiedConfig.validate();
    }
    return false;
}