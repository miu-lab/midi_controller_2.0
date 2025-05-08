#pragma once

#include <functional>
#include <memory>
#include <string>

#include "config/ApplicationConfiguration.hpp"
#include "config/common/CommonIncludes.hpp"
#include "core/domain/IButton.hpp"
#include "core/domain/IDisplay.hpp"
#include "core/domain/IEncoder.hpp"
#include "core/domain/IMidiOut.hpp"
#include "core/domain/IProfileManager.hpp"
#include "core/domain/events/EventSystem.hpp"
#include "adapters/primary/ui/ViewManager.hpp"
#include "core/controllers/MenuController.hpp"

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
class UIEventListener;
class ViewManager;
class MenuController;

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
     * @brief Récupère l'écouteur d'événements du UIController
     * @return Une référence à l'écouteur d'événements
     */
    static UIEventListener& getUIControllerEventListener();

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
     * @brief Enregistre l'écouteur d'événements du UIController
     * @param listener Pointeur vers l'écouteur à enregistrer
     */
    static void registerUIControllerEventListener(UIEventListener* listener);

    /**
     * @brief Enregistre le gestionnaire de vues
     * @param viewManager Pointeur vers le gestionnaire à enregistrer
     */
    static void registerViewManager(ViewManager* viewManager);

    /**
     * @brief Enregistre le contrôleur de menu
     * @param menuController Pointeur vers le contrôleur à enregistrer
     */
    static void registerMenuController(MenuController* menuController);

    /**
     * @brief Obtient le bus d'événements centralisé
     * @return Une référence au bus d'événements centralisé
     */
    static EventBus& getEventBus();

    /**
     * @brief Obtient le gestionnaire de vues
     * @return Une référence au gestionnaire de vues
     */
    static ViewManager& getViewManager();

    /**
     * @brief Obtient le contrôleur de menu
     * @return Une référence au contrôleur de menu
     */
    static MenuController& getMenuController();

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
    UIEventListener* uiControllerEventListener_;
    ViewManager* viewManager_;
    MenuController* menuController_;

    // Configuration
    std::unique_ptr<ApplicationConfiguration> configuration_;

    ServiceLocator();
    ~ServiceLocator();

    // Empêcher la copie et l'affectation
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;
};
