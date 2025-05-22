#pragma once

#include <vector>

#include "adapters/secondary/hardware/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/encoders/EncoderConfig.hpp"

/**
 * @brief Configuration matérielle des boutons et encodeurs
 *
 * Cette classe centralise toutes les configurations matérielles du contrôleur MIDI.
 */
class HardwareConfiguration {
public:
    HardwareConfiguration();

    /**
     * @brief Obtient la configuration des boutons de contrôle
     * @return Un vecteur constant de configurations de boutons
     */
    const std::vector<ButtonConfig>& getControlButtonConfigurations() const;

    /**
     * @brief Obtient la configuration des encodeurs
     * @return Un vecteur constant de configurations d'encodeurs
     */
    const std::vector<EncoderConfig>& getEncoderConfigurations() const;

private:
    std::vector<ButtonConfig> controlButtonConfigurations;
    std::vector<EncoderConfig> encoderConfigurations;

    // Méthodes d'initialisation
    void initializeControlButtonConfigurations();
    void initializeEncoderConfigurations();
};
