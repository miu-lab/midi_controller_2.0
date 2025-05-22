#pragma once

#include <vector>

#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderConfig.hpp"
#include "core/domain/types.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Interface pour la gestion des configurations
 *
 * Cette interface définit les méthodes que tous les systèmes
 * de configuration doivent implémenter.
 */
class IConfiguration {
public:
    virtual ~IConfiguration() = default;

    /**
     * @brief Initialise la configuration
     * @return Résultat de l'initialisation
     */
    virtual Result<bool, std::string> init() = 0;

    // Méthodes de NavigationConfigService
    virtual bool isNavigationControl(InputId id) const = 0;
    virtual void setControlForNavigation(InputId id, bool isNavigation = true) = 0;

    // Méthodes de ConfigurationService
    virtual const std::vector<EncoderConfig>& encoderConfigs() const = 0;
    virtual const std::vector<ButtonConfig>& buttonConfigs() const = 0;

    // Méthodes liées à ApplicationConfiguration
    virtual bool isDebugEnabled() const = 0;
    virtual int midiChannel() const = 0;
    virtual bool isHardwareInitEnabled() const = 0;
};