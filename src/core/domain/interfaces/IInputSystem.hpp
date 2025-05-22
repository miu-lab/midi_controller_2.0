#pragma once

#include <vector>

#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderConfig.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Interface pour le système d'entrée
 *
 * Cette interface définit les méthodes que tous les systèmes d'entrée doivent implémenter.
 * Elle permet de gérer les périphériques d'entrée comme les encodeurs et les boutons.
 */
class IInputSystem {
public:
    virtual ~IInputSystem() = default;

    /**
     * @brief Initialise le système d'entrée
     *
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> init() = 0;

    /**
     * @brief Met à jour l'état du système d'entrée
     *
     * Cette méthode doit être appelée régulièrement pour traiter les événements d'entrée.
     */
    virtual void update() = 0;

    /**
     * @brief Configure les encodeurs avec les paramètres fournis
     *
     * @param encoderConfigs Configurations des encodeurs
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> configureEncoders(
        const std::vector<EncoderConfig>& encoderConfigs) = 0;

    /**
     * @brief Configure les boutons avec les paramètres fournis
     *
     * @param buttonConfigs Configurations des boutons
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> configureButtons(
        const std::vector<ButtonConfig>& buttonConfigs) = 0;
};