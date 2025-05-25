#include "ConfigurationSubsystem.hpp"

#include <Arduino.h>
#include <algorithm>
#include <set>

#include "config/ConfigDefaults.hpp"
// #include "config/HardwareConfiguration.hpp"  // SUPPRIMÉ - Migration vers système unifié

ConfigurationSubsystem::ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    navService_ = std::make_shared<NavigationConfigService>();
    // === MIGRATION VERS SYSTÈME UNIFIÉ ===
    // hardwareConfig_ = std::make_shared<HardwareConfiguration>();  // ANCIEN SYSTÈME SUPPRIMÉ
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

// === NOUVELLE INTERFACE UNIFIÉE ===

const std::vector<InputConfig>& ConfigurationSubsystem::getAllInputConfigurations() const {
    // === SYSTÈME UNIFIÉ MODERNE ===
    // Le système unifié est maintenant la seule option
    if (config_) {
        // const auto& unifiedConfig = config_->getUnifiedConfiguration();
        // Pour le moment, nous devons convertir depuis ControlDefinition vers InputConfig
        // Implementation temporaire - retourner une liste vide pour l'éviter les erreurs
        static std::vector<InputConfig> emptyConfigs;
        return emptyConfigs; // TODO: Implémenter la conversion depuis UnifiedConfiguration
    }
    
    static std::vector<InputConfig> emptyConfigs;
    return emptyConfigs;
}

std::vector<InputConfig> ConfigurationSubsystem::getInputConfigurationsByType(InputType type) const {
    // TODO: Implémenter avec le système unifié
    std::vector<InputConfig> emptyConfigs;
    return emptyConfigs;
}

std::optional<InputConfig> ConfigurationSubsystem::getInputConfigurationById(InputId id) const {
    // TODO: Implémenter avec le système unifié
    return std::nullopt;
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
    if (config_) {
        const auto& unifiedConfig = config_->getUnifiedConfiguration();
        return unifiedConfig.validate();
    }
    return false;
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
    
    // === SYSTÈME UNIFIÉ MODERNE ===
    // Vérifier que ApplicationConfiguration est disponible
    if (!config_) {
        return Result<bool, std::string>::error("ApplicationConfiguration not available");
    }
    
    // Les configurations sont maintenant chargées dans ApplicationConfiguration->UnifiedConfiguration
    // On valide juste qu'elles sont correctes
    if (!validateAllConfigurations()) {
        return Result<bool, std::string>::error("Some input configurations are invalid");
    }
    
    const auto& unifiedConfig = config_->getUnifiedConfiguration();
    auto stats = unifiedConfig.getStats();
    Serial.print(stats.totalControls);
    Serial.println(F(" input configurations loaded from unified system"));
    
    // Afficher le statut du système (toujours unifié maintenant)
    Serial.println(F("ConfigurationSubsystem: Using unified configuration system"));
    
    // Afficher un résumé des statistiques
    Serial.print(F("Stats - Encoders: "));
    Serial.print(stats.encoders);
    Serial.print(F(", Buttons: "));
    Serial.print(stats.buttons);
    Serial.print(F(", MIDI mappings: "));
    Serial.println(stats.midiMappings);
    
    return Result<bool, std::string>::success(true);
}
