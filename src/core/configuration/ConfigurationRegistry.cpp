#include "ConfigurationRegistry.hpp"
#include "config/unified/UnifiedConfiguration.hpp"

ConfigurationRegistry::ConfigurationRegistry(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
}

bool ConfigurationRegistry::registerConfigurationSubsystem(std::shared_ptr<void> configSubsystem) {
    if (!container_ || !configSubsystem) {
        return false;
    }
    
    // Enregistrer directement comme IConfiguration
    // Le cast est sûr car nous savons que c'est un ConfigurationSubsystem
    container_->registerDependency<IConfiguration>(
        std::static_pointer_cast<IConfiguration>(configSubsystem));
    
    return true;
}

bool ConfigurationRegistry::registerUnifiedConfiguration(std::shared_ptr<ApplicationConfiguration> appConfig) {
    if (!container_ || !appConfig) {
        return false;
    }
    
    // Enregistrer UnifiedConfiguration pour l'accès direct
    auto unifiedConfigPtr = std::shared_ptr<UnifiedConfiguration>(
        const_cast<UnifiedConfiguration*>(&appConfig->getUnifiedConfiguration()),
        [](UnifiedConfiguration*) {
            // Custom deleter qui ne fait rien car on ne possède pas l'objet
        });
    
    container_->registerDependency<UnifiedConfiguration>(unifiedConfigPtr);
    
    return true;
}