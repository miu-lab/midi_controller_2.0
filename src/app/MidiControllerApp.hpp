#pragma once

#include <memory>
#include "config/ApplicationConfiguration.hpp"
#include "core/domain/types.hpp"
#include "core/utils/Result.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"

// Forward declarations pour éviter les inclusions inutiles
class DependencyContainer;
class ConfigurationSubsystem;
class InputSubsystem;
class MidiSubsystem;
class UISubsystem;

/**
 * @brief Nouvelle implémentation de l'application principale utilisant l'architecture
 * modulaire avec injection de dépendances.
 * 
 * @details Cette classe utilise les nouveaux sous-systèmes basés sur les interfaces
 * abstraites pour une meilleure modularité et testabilité.
 */
class MidiControllerApp {
public:
    /**
     * @brief Constructeur prenant une configuration d'application
     * @param config Configuration de l'application
     * @param externalContainer Conteneur de dépendances externe (optionnel, pour les tests)
     */
    explicit MidiControllerApp(const ApplicationConfiguration& config, 
                            std::shared_ptr<DependencyContainer> externalContainer = nullptr);
    
    /**
     * @brief Destructeur - assure une destruction ordonnée des sous-systèmes
     */
    ~MidiControllerApp();
    
    /**
     * @brief Initialise tous les sous-systèmes dans l'ordre approprié
     * @return Résultat de l'initialisation
     */
    Result<bool, std::string> init();
    
    /**
     * @brief Met à jour tous les sous-systèmes dans l'ordre approprié
     */
    void update();
    
    // API publique (compatible avec l'ancienne application)
    /**
     * @brief Définit si un contrôle est utilisé pour la navigation
     * @param id Identifiant du contrôle
     * @param isNavigation true si le contrôle est utilisé pour la navigation, false sinon
     */
    void setControlForNavigation(ControlId id, bool isNavigation);
    
    /**
     * @brief Vérifie si un contrôle est utilisé pour la navigation
     * @param id Identifiant du contrôle
     * @return true si le contrôle est utilisé pour la navigation, false sinon
     */
    bool isNavigationControl(ControlId id) const;
    
    // Accesseurs pour les tests (dépréciés - à remplacer par l'accès aux interfaces)
    std::shared_ptr<ConfigurationSubsystem> getConfigSystem() const { return m_configSystem; }
    std::shared_ptr<InputSubsystem> getInputSystem() const { return m_inputSystem; }
    std::shared_ptr<MidiSubsystem> getMidiSystem() const { return m_midiSystem; }
    std::shared_ptr<UISubsystem> getUISystem() const { return m_uiSystem; }
    
    // Accesseurs des interfaces (meilleure pratique)
    std::shared_ptr<IConfiguration> getConfiguration() const;
    std::shared_ptr<IInputSystem> getInputInterface() const;
    std::shared_ptr<IMidiSystem> getMidiInterface() const;
    std::shared_ptr<IUISystem> getUIInterface() const;
    
private:
    std::shared_ptr<DependencyContainer> m_dependencies;
    
    std::shared_ptr<ConfigurationSubsystem> m_configSystem;
    std::shared_ptr<InputSubsystem> m_inputSystem;
    std::shared_ptr<MidiSubsystem> m_midiSystem;
    std::shared_ptr<UISubsystem> m_uiSystem;
};