#pragma once

#include <memory>
#include <vector>
#include <optional>

#include "adapters/secondary/hardware/input/InputConfig.hpp"
#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderConfig.hpp"
#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/use_cases/ProcessButtons.hpp"
#include "core/use_cases/ProcessEncoders.hpp"
#include "core/utils/Result.hpp"

class EncoderManager;
class DigitalButtonManager;
class InputController;

/**
 * @brief Sous-système de gestion des entrées
 *
 * Cette classe implémente l'interface IInputSystem et gère toutes
 * les entrées utilisateur (encodeurs, boutons).
 * Interface unifiée basée sur InputConfig.
 */
class InputSubsystem : public IInputSystem {
public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param container Conteneur de dépendances
     */
    explicit InputSubsystem(std::shared_ptr<DependencyContainer> container);

    /**
     * @brief Destructeur par défaut
     */
    ~InputSubsystem() override = default;

    /**
     * @brief Initialise le sous-système d'entrée
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> init() override;

    /**
     * @brief Met à jour l'état des entrées
     */
    void update() override;

    // === INTERFACE UNIFIÉE ===
    
    /**
     * @brief Configure toutes les entrées avec les paramètres fournis
     * @param inputConfigs Configurations d'entrée unifiées
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> configureInputs(const std::vector<InputConfig>& inputConfigs) override;
    
    /**
     * @brief Obtient toutes les configurations d'entrée actives
     * @return Vecteur des configurations d'entrée
     */
    std::vector<InputConfig> getAllActiveInputConfigurations() const override;
    
    /**
     * @brief Obtient une configuration d'entrée par ID
     * @param id Identifiant de l'entrée
     * @return Configuration trouvée ou std::nullopt
     */
    std::optional<InputConfig> getInputConfigurationById(InputId id) const override;

    /**
     * @brief Obtient le nombre d'entrées actives par type
     * @param type Type d'entrée à compter
     * @return Nombre d'entrées actives du type spécifié
     */
    size_t getActiveInputCountByType(InputType type) const override;
    
    /**
     * @brief Vérifie si toutes les entrées sont initialisées correctement
     * @return true si toutes les entrées sont fonctionnelles
     */
    bool validateInputsStatus() const override;

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<IConfiguration> configuration_;
    std::shared_ptr<EncoderManager> encoderManager_;
    std::shared_ptr<DigitalButtonManager> buttonManager_;
    std::shared_ptr<InputController> inputController_;

    // Processeurs d'événements
    std::unique_ptr<ProcessEncoders> processEncoders_;
    std::unique_ptr<ProcessButtons> processButtons_;

    bool initialized_ = false;
    
    // === MÉTHODES PRIVÉES ===
    
    /**
     * @brief Charge les configurations depuis la nouvelle interface unifiée
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> loadUnifiedConfigurations();
    
    /**
     * @brief Extrait les configurations d'encodeurs depuis les InputConfig
     * @param inputConfigs Configurations d'entrée unifiées
     * @return Vecteur des configurations d'encodeurs
     */
    std::vector<EncoderConfig> extractEncoderConfigs(const std::vector<InputConfig>& inputConfigs) const;
    
    /**
     * @brief Extrait les configurations de boutons depuis les InputConfig
     * @param inputConfigs Configurations d'entrée unifiées
     * @return Vecteur des configurations de boutons
     */
    std::vector<ButtonConfig> extractButtonConfigs(const std::vector<InputConfig>& inputConfigs) const;
    
    /**
     * @brief Crée les managers avec les configurations extraites
     * @param encoderConfigs Configurations d'encodeurs
     * @param buttonConfigs Configurations de boutons
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> createManagers(const std::vector<EncoderConfig>& encoderConfigs,
                                            const std::vector<ButtonConfig>& buttonConfigs);
    
    /**
     * @brief Initialise les processeurs d'événements
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> initializeProcessors();
    
    /**
     * @brief Connecte le contrôleur d'entrée
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> connectInputController();
};
