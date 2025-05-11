#pragma once

#include <functional>
#include <memory>
#include <string>

#include "adapters/primary/ui/ViewManager.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "config/common/CommonIncludes.hpp"
#include "core/controllers/MenuController.hpp"
#include "core/domain/IButton.hpp"
#include "core/domain/IDisplay.hpp"
#include "core/domain/IEncoder.hpp"
#include "core/domain/IMidiOut.hpp"
#include "core/domain/IProfileManager.hpp"
#include "core/domain/events/EventSystem.hpp"

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
     * @param service Pointeur partagé vers le service à enregistrer
     */
    static void registerConfigurationService(std::shared_ptr<ConfigurationService> service);

    /**
     * @brief Enregistre le service NavigationConfigService
     * @param service Pointeur partagé vers le service à enregistrer
     */
    static void registerNavigationConfigService(std::shared_ptr<NavigationConfigService> service);

    /**
     * @brief Enregistre le service ProfileManager
     * @param service Pointeur partagé vers le service à enregistrer
     */
    static void registerProfileManager(std::shared_ptr<ProfileManager> service);

    /**
     * @brief Enregistre le service MidiSystem
     * @param service Pointeur partagé vers le service à enregistrer
     */
    static void registerMidiSystem(std::shared_ptr<MidiSystem> service);

    /**
     * @brief Enregistre le service InputSystem
     * @param service Pointeur partagé vers le service à enregistrer
     */
    static void registerInputSystem(std::shared_ptr<InputSystem> service);

    /**
     * @brief Enregistre le service UiEventService
     * @param service Pointeur partagé vers le service à enregistrer
     */
    static void registerUiEventService(std::shared_ptr<UiEventService> service);

    /**
     * @brief Enregistre le service ControllerService
     * @param service Pointeur partagé vers le service à enregistrer
     */
    static void registerControllerService(std::shared_ptr<ControllerService> service);

    /**
     * @brief Enregistre le contrôleur d'entrées
     * @param controller Pointeur partagé vers le contrôleur à enregistrer
     */
    static void registerInputController(std::shared_ptr<InputController> controller);

    /**
     * @brief Enregistre le contrôleur d'interface utilisateur
     * @param controller Pointeur partagé vers le contrôleur à enregistrer
     */
    static void registerUIController(std::shared_ptr<UIController> controller);

    /**
     * @brief Enregistre l'écouteur d'événements du UIController
     * @param listener Pointeur partagé vers l'écouteur à enregistrer
     */
    static void registerUIControllerEventListener(std::shared_ptr<UIEventListener> listener);

    /**
     * @brief Enregistre le gestionnaire de vues
     * @param viewManager Pointeur partagé vers le gestionnaire à enregistrer
     */
    static void registerViewManager(std::shared_ptr<ViewManager> viewManager);

    /**
     * @brief Enregistre le contrôleur de menu
     * @param menuController Pointeur partagé vers le contrôleur à enregistrer
     */
    static void registerMenuController(std::shared_ptr<MenuController> menuController);

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
    std::shared_ptr<ConfigurationService> configurationService_;
    std::shared_ptr<NavigationConfigService> navigationConfigService_;
    std::shared_ptr<ProfileManager> profileManager_;
    std::shared_ptr<MidiSystem> midiSystem_;
    std::shared_ptr<InputSystem> inputSystem_;
    std::shared_ptr<UiEventService> uiEventService_;
    std::shared_ptr<ControllerService> controllerService_;
    std::shared_ptr<InputController> inputController_;
    std::shared_ptr<UIController> uiController_;
    std::shared_ptr<UIEventListener> uiControllerEventListener_;
    std::shared_ptr<ViewManager> viewManager_;
    std::shared_ptr<MenuController> menuController_;

    // Configuration
    std::shared_ptr<ApplicationConfiguration> configuration_;

    ServiceLocator();
    ~ServiceLocator();

    // Empêcher la copie et l'affectation
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;
};
