#pragma once

#include <memory>
#include <vector>

#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderConfig.hpp"
#include "config/unified/ControlDefinition.hpp"
#include "core/utils/Result.hpp"

// Forward declarations
class EncoderManager;
class DigitalButtonManager;
class ProcessEncoders;
class ProcessButtons;
class InputController;

/**
 * @brief Gestionnaire centralisé pour les entrées utilisateur
 * 
 * Cette classe centralise la logique de gestion des entrées,
 * respectant le principe de responsabilité unique.
 */
class InputManager {
public:
    /**
     * @brief Configuration pour InputManager
     */
    struct ManagerConfig {
        bool enableEncoders;
        bool enableButtons;
        bool enableEventProcessing;
        
        ManagerConfig() : enableEncoders(true), enableButtons(true), enableEventProcessing(true) {}
    };

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration du gestionnaire
     */
    explicit InputManager(const ManagerConfig& config = ManagerConfig());

    /**
     * @brief Destructeur par défaut
     * Défini dans le .cpp pour éviter les problèmes de types incomplets
     */
    ~InputManager();

    /**
     * @brief Initialise le gestionnaire avec les définitions de contrôles
     * @param controlDefinitions Définitions des contrôles à gérer
     * @param inputController Contrôleur d'entrée pour les événements
     * @return Result<bool> Succès ou erreur
     */
    Result<bool> initialize(const std::vector<ControlDefinition>& controlDefinitions,
                           std::shared_ptr<InputController> inputController);

    /**
     * @brief Met à jour tous les composants de gestion des entrées
     */
    void update();

    /**
     * @brief Reconfigure les entrées avec nouvelles définitions
     * @param controlDefinitions Nouvelles définitions de contrôles
     * @return Result<bool> Succès ou erreur
     */
    Result<bool> reconfigure(const std::vector<ControlDefinition>& controlDefinitions);

    /**
     * @brief Vérifie si le gestionnaire est opérationnel
     * @return true si le gestionnaire est initialisé et opérationnel
     */
    bool isOperational() const;

    /**
     * @brief Obtient le gestionnaire d'encodeurs
     * @return Pointeur vers EncoderManager ou nullptr
     */
    std::shared_ptr<EncoderManager> getEncoderManager() const;

    /**
     * @brief Obtient le gestionnaire de boutons
     * @return Pointeur vers DigitalButtonManager ou nullptr
     */
    std::shared_ptr<DigitalButtonManager> getButtonManager() const;

private:
    ManagerConfig config_;
    bool initialized_;

    // Gestionnaires de matériel
    std::shared_ptr<EncoderManager> encoderManager_;
    std::shared_ptr<DigitalButtonManager> buttonManager_;

    // Processeurs d'événements
    std::unique_ptr<ProcessEncoders> processEncoders_;
    std::unique_ptr<ProcessButtons> processButtons_;

    // Contrôleur d'entrée
    std::shared_ptr<InputController> inputController_;

    /**
     * @brief Extrait les configurations d'encodeurs depuis les définitions
     * @param controlDefinitions Définitions de contrôles
     * @return Vecteur de configurations d'encodeurs
     */
    std::vector<EncoderConfig> extractEncoderConfigs(const std::vector<ControlDefinition>& controlDefinitions) const;

    /**
     * @brief Extrait les configurations de boutons depuis les définitions
     * @param controlDefinitions Définitions de contrôles
     * @return Vecteur de configurations de boutons
     */
    std::vector<ButtonConfig> extractButtonConfigs(const std::vector<ControlDefinition>& controlDefinitions) const;

    /**
     * @brief Crée les gestionnaires de matériel
     * @param encoderConfigs Configurations d'encodeurs
     * @param buttonConfigs Configurations de boutons
     * @return Result<bool> Succès ou erreur
     */
    Result<bool> createManagers(const std::vector<EncoderConfig>& encoderConfigs,
                               const std::vector<ButtonConfig>& buttonConfigs);

    /**
     * @brief Initialise les processeurs d'événements
     * @return Result<bool> Succès ou erreur
     */
    Result<bool> initializeProcessors();

    /**
     * @brief Connecte les processeurs au contrôleur
     */
    void connectProcessors();
};