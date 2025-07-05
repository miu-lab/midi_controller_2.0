#include "ConfigurationLoader.hpp"
#include "core/utils/Error.hpp"

Result<bool> ConfigurationLoader::loadUnifiedConfigurations(
    std::shared_ptr<ApplicationConfiguration> appConfig) {
    
    if (!appConfig) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, "ApplicationConfiguration not available"});
    }
    
    // Store the configuration reference
    appConfig_ = appConfig;
    
    // The actual loading is currently minimal, but this is where
    // more complex loading logic would go in the future
    loaded_ = true;
    
    return Result<bool>::success(true);
}

Result<bool> ConfigurationLoader::validateConfigurations() {
    if (!loaded_ || !appConfig_) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, "Configurations not loaded"});
    }
    
    // Access the unified configuration for validation
    const auto& unifiedConfig = appConfig_->getUnifiedConfiguration();
    
    // Validate using the existing validation method
    auto validationResult = unifiedConfig.validate();
    if (validationResult.isError()) {
        return Result<bool>::error(
            {ErrorCode::ConfigError, "Configuration validation failed"});
    }
    
    return Result<bool>::success(true);
}