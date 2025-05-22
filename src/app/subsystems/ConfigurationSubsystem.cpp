#include "ConfigurationSubsystem.hpp"

#include <Arduino.h>
#include <algorithm>
#include <set>

#include "config/ConfigDefaults.hpp"
#include "config/HardwareConfiguration.hpp"

ConfigurationSubsystem::ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    navService_ = std::make_shared<NavigationConfigService>();
    hardwareConfig_ = std::make_shared<HardwareConfiguration>();
}

Result<bool, std::string> ConfigurationSubsystem::init() {
    // Récupérer la configuration de l'application depuis le conteneur
    config_ = container_->resolve<ApplicationConfiguration>();
    if (!config_) {
        // Si aucune configuration n'est trouvée, créer une configuration par défaut
        config_ = std::make_shared<ApplicationConfiguration>();
        container_->registerDependency<ApplicationConfiguration>(config_);
    }

    // Charger les configurations unifiées
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

// === NOUVELLE INTERFACE UNIFIÉE ===

const std::vector<InputConfig>& ConfigurationSubsystem::getAllInputConfigurations() const {
    return hardwareConfig_->getAllInputConfigurations();
}

std::vector<InputConfig> ConfigurationSubsystem::getInputConfigurationsByType(InputType type) const {
    return hardwareConfig_->getInputConfigurationsByType(type);
}

std::optional<InputConfig> ConfigurationSubsystem::getInputConfigurationById(InputId id) const {
    return hardwareConfig_->getInputConfigurationById(id);
}

std::vector<InputConfig> ConfigurationSubsystem::getInputConfigurationsByGroup(const std::string& group) const {
    const auto& allInputs = getAllInputConfigurations();
    std::vector<InputConfig> filtered;
    
    for (const auto& input : allInputs) {
        if (input.group == group) {
            filtered.push_back(input);
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

// === MÉTHODES DE CONFIGURATION GÉNÉRALE ===

bool ConfigurationSubsystem::isDebugEnabled() const {
    // TODO: Accéder à la configuration de debug via la structure appropriée
    // Pour l'instant, renvoyer une valeur par défaut
    return ConfigDefaults::SHOW_DEBUG_INFO;
}

int ConfigurationSubsystem::midiChannel() const {
    // TODO: Accéder au canal MIDI via la structure appropriée
    // Pour l'instant, renvoyer le canal par défaut
    return ConfigDefaults::DEFAULT_MIDI_CHANNEL;
}

bool ConfigurationSubsystem::isHardwareInitEnabled() const {
    // TODO: Vérifier si l'initialisation matérielle est activée
    // Pour l'instant, renvoyer true par défaut
    return true;
}

// === MÉTHODES UTILITAIRES ===

bool ConfigurationSubsystem::validateAllConfigurations() const {
    return hardwareConfig_->validateAllConfigurations();
}

std::vector<std::string> ConfigurationSubsystem::getAvailableGroups() const {
    const auto& allInputs = getAllInputConfigurations();
    std::set<std::string> uniqueGroups;
    
    for (const auto& input : allInputs) {
        uniqueGroups.insert(input.group);
    }
    
    return std::vector<std::string>(uniqueGroups.begin(), uniqueGroups.end());
}

size_t ConfigurationSubsystem::getInputCountByType(InputType type) const {
    const auto& allInputs = getAllInputConfigurations();
    return std::count_if(allInputs.begin(), allInputs.end(), 
                        [type](const InputConfig& input) {
                            return input.type == type;
                        });
}

// === MÉTHODES PRIVÉES ===

Result<bool, std::string> ConfigurationSubsystem::loadUnifiedConfigurations() {
    Serial.println(F("ConfigurationSubsystem: Loading unified input configurations"));
    
    // Les configurations sont déjà chargées dans HardwareConfiguration
    // On valide juste qu'elles sont correctes
    if (!validateAllConfigurations()) {
        return Result<bool, std::string>::error("Some input configurations are invalid");
    }
    
    const auto& allInputs = getAllInputConfigurations();
    Serial.print(allInputs.size());
    Serial.println(F(" input configurations loaded"));
    
    // Afficher un résumé des groupes
    auto groups = getAvailableGroups();
    Serial.print(F("Groups found: "));
    for (size_t i = 0; i < groups.size(); ++i) {
        Serial.print(groups[i].c_str());
        if (i < groups.size() - 1) Serial.print(F(", "));
    }
    Serial.println();
    
    // Afficher le nombre par type
    Serial.print(F("Buttons: "));
    Serial.print(getInputCountByType(InputType::BUTTON));
    Serial.print(F(", Encoders: "));
    Serial.println(getInputCountByType(InputType::ENCODER));
    
    return Result<bool, std::string>::success(true);
}
