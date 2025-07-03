#pragma once

#include "core/utils/Result.hpp"
#include "config/ApplicationConfiguration.hpp"
#include <memory>

/**
 * @brief Interface for loading and processing configurations
 * 
 * This interface defines the contract for loading configurations
 * from various sources and processing them for the system.
 * Extracted from ConfigurationSubsystem to follow SRP.
 */
class IConfigurationLoader {
public:
    virtual ~IConfigurationLoader() = default;
    
    /**
     * @brief Load unified configurations from the application configuration
     * @param appConfig The application configuration instance
     * @return Result<bool> Success or error message
     */
    virtual Result<bool> loadUnifiedConfigurations(
        std::shared_ptr<ApplicationConfiguration> appConfig) = 0;
    
    /**
     * @brief Validate all loaded configurations
     * @return Result<bool> Success if all configurations are valid
     */
    virtual Result<bool> validateConfigurations() = 0;
};

/**
 * @brief Default implementation of configuration loading
 * 
 * This class handles the actual loading and validation of configurations
 * from ApplicationConfiguration instances.
 */
class ConfigurationLoader : public IConfigurationLoader {
public:
    ConfigurationLoader() = default;
    ~ConfigurationLoader() override = default;
    
    /**
     * @brief Load unified configurations from the application configuration
     * @param appConfig The application configuration instance
     * @return Result<bool> Success or error message
     */
    Result<bool> loadUnifiedConfigurations(
        std::shared_ptr<ApplicationConfiguration> appConfig) override;
    
    /**
     * @brief Validate all loaded configurations
     * @return Result<bool> Success if all configurations are valid
     */
    Result<bool> validateConfigurations() override;

private:
    std::shared_ptr<ApplicationConfiguration> appConfig_;
    bool loaded_ = false;
};