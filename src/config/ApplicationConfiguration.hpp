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

    /**
     * @brief Définit le facteur de sensibilité global pour les encodeurs
     * @param value Nouveau facteur de sensibilité (1.0 = normal, >1.0 = plus sensible, <1.0 = moins sensible)
     */
    void setEncoderSensitivity(float value);

    /**
     * @brief Obtient le facteur de sensibilité global pour les encodeurs
     * @return Facteur de sensibilité actuel
     */
    float getEncoderSensitivity() const;

private:
    HardwareConfiguration hardwareConfiguration;
    MappingConfiguration mappingConfiguration;
    
    // Paramètres globaux (anciennement dans GlobalSettings)
    float encoderSensitivity_;  // Facteur de sensibilité pour les encodeurs (1.0 = normal)
};
