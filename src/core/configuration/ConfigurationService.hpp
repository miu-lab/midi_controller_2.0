#pragma once

#include "config/unified/ControlDefinition.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/domain/types.hpp"
#include "core/utils/Result.hpp"
#include <memory>
#include <vector>
#include <optional>
#include <string>

/**
 * @brief Interface for configuration business logic operations
 * 
 * This interface defines the contract for configuration query and
 * business logic operations, extracted from ConfigurationSubsystem
 * to follow SRP (Single Responsibility Principle).
 */
class IConfigurationService {
public:
    virtual ~IConfigurationService() = default;
    
    /**
     * @brief Get all control definitions
     * @return Vector of all control definitions
     */
    virtual const std::vector<ControlDefinition>& getAllControlDefinitions() const = 0;
    
    /**
     * @brief Get control definitions filtered by type
     * @param type The input type to filter by
     * @return Vector of control definitions of the specified type
     */
    virtual std::vector<ControlDefinition> getControlDefinitionsByType(InputType type) const = 0;
    
    /**
     * @brief Get a control definition by its ID
     * @param id The input ID to search for
     * @return Optional control definition if found
     */
    virtual std::optional<ControlDefinition> getControlDefinitionById(InputId id) const = 0;
    
    /**
     * @brief Get control definitions filtered by group
     * @param group The group name to filter by
     * @return Vector of control definitions in the specified group
     */
    virtual std::vector<ControlDefinition> getControlDefinitionsByGroup(const std::string& group) const = 0;
    
    /**
     * @brief Get all available groups
     * @return Vector of unique group names
     */
    virtual std::vector<std::string> getAvailableGroups() const = 0;
    
    /**
     * @brief Get count of inputs by type
     * @param type The input type to count
     * @return Number of inputs of the specified type
     */
    virtual size_t getInputCountByType(InputType type) const = 0;
    
    /**
     * @brief Validate all configurations
     * @return true if all configurations are valid
     */
    virtual bool validateAllConfigurations() const = 0;
};

/**
 * @brief Default implementation of configuration business logic service
 * 
 * This class handles the business logic operations on configurations,
 * such as querying, filtering, and validation.
 */
class ConfigurationService : public IConfigurationService {
public:
    /**
     * @brief Constructor with application configuration
     * @param appConfig The application configuration instance
     */
    explicit ConfigurationService(std::shared_ptr<ApplicationConfiguration> appConfig);
    
    ~ConfigurationService() override = default;
    
    /**
     * @brief Update the application configuration reference
     * @param appConfig New application configuration instance
     */
    void setApplicationConfiguration(std::shared_ptr<ApplicationConfiguration> appConfig);
    
    // IConfigurationService implementation
    const std::vector<ControlDefinition>& getAllControlDefinitions() const override;
    std::vector<ControlDefinition> getControlDefinitionsByType(InputType type) const override;
    std::optional<ControlDefinition> getControlDefinitionById(InputId id) const override;
    std::vector<ControlDefinition> getControlDefinitionsByGroup(const std::string& group) const override;
    std::vector<std::string> getAvailableGroups() const override;
    size_t getInputCountByType(InputType type) const override;
    bool validateAllConfigurations() const override;

private:
    std::shared_ptr<ApplicationConfiguration> appConfig_;
};