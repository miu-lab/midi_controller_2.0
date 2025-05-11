#pragma once

#include <memory>

#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"

// Forward declarations des services et contrôleurs
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
class EventBus;

/**
 * @brief Adaptateur pour ServiceLocator qui utilise DependencyContainer
 * 
 * Cette classe permet de maintenir la compatibilité avec le code existant
 * tout en utilisant la nouvelle architecture d'injection de dépendances.
 */
class ServiceLocatorAdapter {
public:
    explicit ServiceLocatorAdapter(std::shared_ptr<DependencyContainer> container);
    ~ServiceLocatorAdapter() = default;
    
    /**
     * @brief Initialise l'adaptateur avec la configuration
     * @param config La configuration de l'application
     */
    void initialize(const ApplicationConfiguration& config);
    
    /**
     * @brief Obtient la configuration de l'application
     * @return Une référence à la configuration
     */
    const ApplicationConfiguration& getConfiguration() const;
    
    /**
     * @brief Récupère le service ConfigurationService
     * @return Une référence au service
     */
    ConfigurationService& getConfigurationService();
    
    /**
     * @brief Récupère le service NavigationConfigService
     * @return Une référence au service
     */
    NavigationConfigService& getNavigationConfigService();
    
    /**
     * @brief Récupère le service ProfileManager
     * @return Une référence au service
     */
    ProfileManager& getProfileManager();
    
    /**
     * @brief Récupère le service MidiSystem
     * @return Une référence au service
     */
    MidiSystem& getMidiSystem();
    
    /**
     * @brief Récupère le service InputSystem
     * @return Une référence au service
     */
    InputSystem& getInputSystem();
    
    /**
     * @brief Récupère le service UiEventService
     * @return Une référence au service
     */
    UiEventService& getUiEventService();
    
    /**
     * @brief Récupère le service ControllerService
     * @return Une référence au service
     */
    ControllerService& getControllerService();
    
    /**
     * @brief Récupère le contrôleur d'entrées
     * @return Une référence au contrôleur
     */
    InputController& getInputController();
    
    /**
     * @brief Récupère le contrôleur d'interface utilisateur
     * @return Une référence au contrôleur
     */
    UIController& getUIController();
    
    /**
     * @brief Récupère l'écouteur d'événements du UIController
     * @return Une référence à l'écouteur
     */
    UIEventListener& getUIControllerEventListener();
    
    /**
     * @brief Obtient le bus d'événements centralisé
     * @return Une référence au bus d'événements
     */
    EventBus& getEventBus();
    
    /**
     * @brief Récupère le gestionnaire de vues
     * @return Une référence au gestionnaire
     */
    ViewManager& getViewManager();
    
    /**
     * @brief Récupère le contrôleur de menu
     * @return Une référence au contrôleur
     */
    MenuController& getMenuController();
    
    /**
     * @brief Enregistre le service ConfigurationService
     * @param service Pointeur vers le service
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerConfigurationService(ConfigurationService* service);
    
    /**
     * @brief Enregistre le service ConfigurationService
     * @param service Pointeur partagé vers le service
     */
    void registerConfigurationService(std::shared_ptr<ConfigurationService> service) {
        container_->registerDependency<ConfigurationService>(service);
    }
    
    /**
     * @brief Enregistre le service NavigationConfigService
     * @param service Pointeur vers le service
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerNavigationConfigService(NavigationConfigService* service);
    
    /**
     * @brief Enregistre le service NavigationConfigService
     * @param service Pointeur partagé vers le service
     */
    void registerNavigationConfigService(std::shared_ptr<NavigationConfigService> service) {
        container_->registerDependency<NavigationConfigService>(service);
    }
    
    /**
     * @brief Enregistre le service ProfileManager
     * @param service Pointeur vers le service
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerProfileManager(ProfileManager* service);
    
    /**
     * @brief Enregistre le service ProfileManager
     * @param service Pointeur partagé vers le service
     */
    void registerProfileManager(std::shared_ptr<ProfileManager> service) {
        container_->registerDependency<ProfileManager>(service);
    }
    
    /**
     * @brief Enregistre le service MidiSystem
     * @param service Pointeur vers le service
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerMidiSystem(MidiSystem* service);
    
    /**
     * @brief Enregistre le service MidiSystem
     * @param service Pointeur partagé vers le service
     */
    void registerMidiSystem(std::shared_ptr<MidiSystem> service) {
        container_->registerDependency<MidiSystem>(service);
    }
    
    /**
     * @brief Enregistre le service InputSystem
     * @param service Pointeur vers le service
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerInputSystem(InputSystem* service);
    
    /**
     * @brief Enregistre le service InputSystem
     * @param service Pointeur partagé vers le service
     */
    void registerInputSystem(std::shared_ptr<InputSystem> service) {
        container_->registerDependency<InputSystem>(service);
    }
    
    /**
     * @brief Enregistre le service UiEventService
     * @param service Pointeur vers le service
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerUiEventService(UiEventService* service);
    
    /**
     * @brief Enregistre le service UiEventService
     * @param service Pointeur partagé vers le service
     */
    void registerUiEventService(std::shared_ptr<UiEventService> service) {
        container_->registerDependency<UiEventService>(service);
    }
    
    /**
     * @brief Enregistre le service ControllerService
     * @param service Pointeur vers le service
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerControllerService(ControllerService* service);
    
    /**
     * @brief Enregistre le service ControllerService
     * @param service Pointeur partagé vers le service
     */
    void registerControllerService(std::shared_ptr<ControllerService> service) {
        container_->registerDependency<ControllerService>(service);
    }
    
    /**
     * @brief Enregistre le contrôleur d'entrées
     * @param controller Pointeur vers le contrôleur
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerInputController(InputController* controller);
    
    /**
     * @brief Enregistre le contrôleur d'entrées
     * @param controller Pointeur partagé vers le contrôleur
     */
    void registerInputController(std::shared_ptr<InputController> controller) {
        container_->registerDependency<InputController>(controller);
    }
    
    /**
     * @brief Enregistre le contrôleur d'interface utilisateur
     * @param controller Pointeur vers le contrôleur
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerUIController(UIController* controller);
    
    /**
     * @brief Enregistre le contrôleur d'interface utilisateur
     * @param controller Pointeur partagé vers le contrôleur
     */
    void registerUIController(std::shared_ptr<UIController> controller) {
        container_->registerDependency<UIController>(controller);
    }
    
    /**
     * @brief Enregistre l'écouteur d'événements du UIController
     * @param listener Pointeur vers l'écouteur
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerUIControllerEventListener(UIEventListener* listener);
    
    /**
     * @brief Enregistre l'écouteur d'événements du UIController
     * @param listener Pointeur partagé vers l'écouteur
     */
    void registerUIControllerEventListener(std::shared_ptr<UIEventListener> listener) {
        container_->registerDependency<UIEventListener>(listener);
    }
    
    /**
     * @brief Enregistre le gestionnaire de vues
     * @param viewManager Pointeur vers le gestionnaire
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerViewManager(ViewManager* viewManager);
    
    /**
     * @brief Enregistre le gestionnaire de vues
     * @param viewManager Pointeur partagé vers le gestionnaire
     */
    void registerViewManager(std::shared_ptr<ViewManager> viewManager) {
        container_->registerDependency<ViewManager>(viewManager);
    }
    
    /**
     * @brief Enregistre le contrôleur de menu
     * @param menuController Pointeur vers le contrôleur
     * @deprecated Utilisez la version avec std::shared_ptr à la place
     */
    void registerMenuController(MenuController* menuController);
    
    /**
     * @brief Enregistre le contrôleur de menu
     * @param menuController Pointeur partagé vers le contrôleur
     */
    void registerMenuController(std::shared_ptr<MenuController> menuController) {
        container_->registerDependency<MenuController>(menuController);
    }
    
private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<ApplicationConfiguration> configuration_;
};
