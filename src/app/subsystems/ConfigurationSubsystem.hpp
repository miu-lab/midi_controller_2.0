#pragma once

#include <memory>

#include "app/di/DependencyContainer.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "config/HardwareConfiguration.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Sous-système de gestion des configurations
 *
 * Cette classe implémente l'interface IConfiguration et centralise toutes
 * les configurations de l'application.
 *
 * @details Le sous-système de configuration est responsable de charger et gérer
 * toutes les configurations de l'application, y compris :
 * - Les configurations des contrôles de navigation
 * - Les configurations des encodeurs et boutons
 * - Les paramètres généraux de l'application
 */
class ConfigurationSubsystem : public IConfiguration {
public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param container Conteneur de dépendances
     */
    explicit ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container);

    /**
     * @brief Destructeur par défaut
     */
    ~ConfigurationSubsystem() override = default;

    /**
     * @brief Initialise le sous-système de configuration
     *
     * @details Cette méthode configure le sous-système en :
     * - Initialisant les services de navigation
     * - Chargeant les configurations des encodeurs et boutons
     * - Appliquant les configurations par défaut si nécessaire
     *
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> init();

    /**
     * @brief Vérifie si un contrôle est dédié à la navigation
     *
     * @param id Identifiant du contrôle à vérifier
     * @return true si le contrôle est utilisé pour la navigation, false sinon
     */
    bool isNavigationControl(ControlId id) const override;

    /**
     * @brief Définit si un contrôle est utilisé pour la navigation
     *
     * @param id Identifiant du contrôle à configurer
     * @param isNavigation true pour définir le contrôle comme contrôle de navigation, false sinon
     */
    void setControlForNavigation(ControlId id, bool isNavigation = true) override;

    /**
     * @brief Obtient les configurations des encodeurs
     *
     * @return Tableau des configurations des encodeurs
     */
    const std::vector<EncoderConfig>& encoderConfigs() const override;

    /**
     * @brief Obtient les configurations des boutons
     *
     * @return Tableau des configurations des boutons
     */
    const std::vector<ButtonConfig>& buttonConfigs() const override;

    /**
     * @brief Vérifie si le mode debug est activé
     *
     * @return true si le mode debug est activé, false sinon
     */
    bool isDebugEnabled() const override;

    /**
     * @brief Obtient le canal MIDI par défaut
     *
     * @return Numéro du canal MIDI (1-16)
     */
    int midiChannel() const override;

    /**
     * @brief Vérifie si l'initialisation du matériel est activée
     *
     * @return true si l'initialisation du matériel est activée, false sinon
     */
    bool isHardwareInitEnabled() const override;

    // Constantes de configuration
    static constexpr int DEFAULT_MIDI_CHANNEL = 1;
    static constexpr bool DEFAULT_DEBUG_MODE = false;
    static constexpr bool DEFAULT_HARDWARE_INIT = true;

private:
    // Membres privés
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<ApplicationConfiguration> config_;
    std::shared_ptr<NavigationConfigService> navService_;
    std::shared_ptr<HardwareConfiguration> hardwareConfig_;  // Ajout de la configuration matérielle
    std::vector<EncoderConfig> encoderConfigs_;
    std::vector<ButtonConfig> buttonConfigs_;

    /**
     * @brief Charge les configurations des encodeurs depuis HardwareConfiguration
     *
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> loadEncoderConfigs();

    /**
     * @brief Charge les configurations des boutons depuis HardwareConfiguration
     *
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> loadButtonConfigs();
};