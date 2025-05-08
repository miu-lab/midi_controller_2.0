#pragma once

#include "config/HardwareConfiguration.hpp"
#include "config/MappingConfiguration.hpp"

/**
 * @brief Configuration centrale de l'application
 *
 * Cette classe sert de point d'entrée unique pour toutes les configurations
 * de l'application. Elle regroupe toutes les configurations spécifiques.
 */
class ApplicationConfiguration {
public:
    /**
     * @brief Constructeur par défaut
     */
    ApplicationConfiguration();

    /**
     * @brief Obtient la configuration matérielle
     * @return Une référence à la configuration matérielle
     */
    const HardwareConfiguration& getHardwareConfiguration() const;

    /**
     * @brief Obtient la configuration des mappings
     * @return Une référence à la configuration des mappings
     */
    const MappingConfiguration& getMappingConfiguration() const;

private:
    HardwareConfiguration hardwareConfiguration;
    MappingConfiguration mappingConfiguration;
};
