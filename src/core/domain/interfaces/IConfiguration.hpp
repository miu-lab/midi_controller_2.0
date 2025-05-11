#pragma once

#include <vector>
#include "adapters/secondary/hardware/buttons/DigitalButtonConfig.hpp"
#include "adapters/secondary/hardware/encoders/EncoderConfig.hpp"
#include "core/domain/types.hpp"

/**
 * @brief Interface pour la gestion des configurations
 * 
 * Cette interface définit les méthodes que tous les systèmes
 * de configuration doivent implémenter.
 */
class IConfiguration {
public:
    virtual ~IConfiguration() = default;
    
    // Méthodes de NavigationConfigService
    virtual bool isNavigationControl(ControlId id) const = 0;
    virtual void setControlForNavigation(ControlId id, bool isNavigation = true) = 0;
    
    // Méthodes de ConfigurationService
    virtual const std::vector<EncoderConfig>& getEncoderConfigs() const = 0;
    virtual const std::vector<ButtonConfig>& getButtonConfigs() const = 0;
    
    // Méthodes liées à ApplicationConfiguration
    virtual bool isDebugEnabled() const = 0;
    virtual int getMidiChannel() const = 0;
    virtual bool isHardwareInitEnabled() const = 0;
};