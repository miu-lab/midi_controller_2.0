#pragma once

#include <functional>
#include <memory>
#include <string>

#include "config/ApplicationConfiguration.hpp"
#include "core/domain/EventBus.hpp"
#include "core/domain/IButton.hpp"
#include "core/domain/IDisplay.hpp"
#include "core/domain/IEncoder.hpp"
#include "core/domain/IMidiOut.hpp"
#include "core/domain/IProfileManager.hpp"
#include "core/domain/events/EventTypes.hpp"

// Forward declarations
class ConfigurationService;
class NavigationConfigService;
class ProfileManager;
class MidiSystem;
class InputSystem;
class UiEventService;
class ControllerService;
class InputController;
class UIController;

/**
 * @brief Service d'injection de dépendances
 *
 * Cette classe centralise la création et l'accès aux objets du système.
 * Elle implémente le pattern Service Locator pour faciliter l'injection de dépendances.
 */
class ServiceLocator {
public:
    /**
     * @brief Initialise le ServiceLocator avec les dépendances principales
     * @param config La configuration de l'application
     */
    static void initialize(const ApplicationConfiguration& config);

    /**
     * @brief Récupère la configuration de l'application
     * @return Une référence à la configuration
     */
    static const ApplicationConfiguration& getConfiguration();

    /**
     * @brief Récupère le service ConfigurationService
     * @return Une référence au service de configuration
     */
    static ConfigurationService& getConfigurationService();

    /**
     * @brief Récupère le service NavigationConfigService
     * @return Une référence au service de configuration de navigation
     */
    static NavigationConfigService& getNavigationConfigService();

    /**
     * @brief Récupère le service ProfileManager
     * @return Une référence au gestionnaire de profils
     */
    static ProfileManager& getProfileManager();

    /**
     * @brief Récupère le service MidiSystem
     * @return Une référence au système MIDI
     */
    static MidiSystem& getMidiSystem();

    /**
     * @brief Récupère le service InputSystem
     * @return Une référence au système d'entrée
     */
    static InputSystem& getInputSystem();

    /**
     * @brief Récupère le service UiEventService
     * @return Une référence au service d'événements UI
     */
    static UiEventService& getUiEventService();

    /**
     * @brief Récupère le service ControllerService
     * @return Une référence au service de contrôleurs
     */
    static ControllerService& getControllerService();

    /**
     * @brief Récupère le contrôleur d'entrées
     * @return Une référence au contrôleur d'entrées
     */
    static InputController& getInputController();

    /**
     * @brief Récupère le contrôleur d'interface utilisateur
     * @return Une référence au contrôleur d'interface utilisateur
     */
    static UIController& getUIController();

    /**
     * @brief Enregistre le service ConfigurationService
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerConfigurationService(ConfigurationService* service);

    /**
     * @brief Enregistre le service NavigationConfigService
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerNavigationConfigService(NavigationConfigService* service);

    /**
     * @brief Enregistre le service ProfileManager
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerProfileManager(ProfileManager* service);

    /**
     * @brief Enregistre le service MidiSystem
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerMidiSystem(MidiSystem* service);

    /**
     * @brief Enregistre le service InputSystem
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerInputSystem(InputSystem* service);

    /**
     * @brief Enregistre le service UiEventService
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerUiEventService(UiEventService* service);

    /**
     * @brief Enregistre le service ControllerService
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerControllerService(ControllerService* service);

    /**
     * @brief Enregistre le contrôleur d'entrées
     * @param controller Pointeur vers le contrôleur à enregistrer
     */
    static void registerInputController(InputController* controller);

    /**
     * @brief Enregistre le contrôleur d'interface utilisateur
     * @param controller Pointeur vers le contrôleur à enregistrer
     */
    static void registerUIController(UIController* controller);

    /**
     * @brief Obtient le bus d'événements pour les encodeurs
     * @return Une référence au bus d'événements
     */
    static EventBus<EventTypes::EncoderTurned>& getEncoderTurnedEventBus();

    /**
     * @brief Obtient le bus d'événements pour les boutons d'encodeurs
     * @return Une référence au bus d'événements
     */
    static EventBus<EventTypes::EncoderButton>& getEncoderButtonEventBus();

    /**
     * @brief Obtient le bus d'événements pour les boutons pressés
     * @return Une référence au bus d'événements
     */
    static EventBus<EventTypes::ButtonPressed>& getButtonPressedEventBus();

    /**
     * @brief Obtient le bus d'événements pour les boutons relâchés
     * @return Une référence au bus d'événements
     */
    static EventBus<EventTypes::ButtonReleased>& getButtonReleasedEventBus();

private:
    // Singleton pattern
    static ServiceLocator& getInstance();

    // Services
    ConfigurationService* configurationService_;
    NavigationConfigService* navigationConfigService_;
    ProfileManager* profileManager_;
    MidiSystem* midiSystem_;
    InputSystem* inputSystem_;
    UiEventService* uiEventService_;
    ControllerService* controllerService_;
    InputController* inputController_;
    UIController* uiController_;

    // Configuration
    std::unique_ptr<ApplicationConfiguration> configuration_;

    ServiceLocator();
    ~ServiceLocator();

    // Empêcher la copie et l'affectation
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;
};
