#include "app/services/ServiceLocator.hpp"

#include "adapters/primary/ui/ContextualView.hpp"
#include "adapters/primary/ui/ControlMonitorView.hpp"
#include "adapters/primary/ui/DebugView.hpp"
#include "adapters/primary/ui/MenuView.hpp"
#include "adapters/primary/ui/ModalView.hpp"
#include "adapters/secondary/storage/ProfileManager.hpp"
#include "app/services/ConfigurationService.hpp"
#include "app/services/ControllerService.hpp"
#include "app/services/InputSystem.hpp"
#include "app/services/MidiSystem.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/services/UiEventService.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/UIController.hpp"
#include "core/listeners/UIEventListener.hpp"

ServiceLocator::ServiceLocator()
    : configurationService_(nullptr),
      navigationConfigService_(nullptr),
      profileManager_(nullptr),
      midiSystem_(nullptr),
      inputSystem_(nullptr),
      uiEventService_(nullptr),
      controllerService_(nullptr),
      inputController_(nullptr),
      uiController_(nullptr),
      uiControllerEventListener_(nullptr),
      viewManager_(nullptr),
      menuController_(nullptr),
      configuration_(nullptr) {}

ServiceLocator::~ServiceLocator() = default;

ServiceLocator& ServiceLocator::getInstance() {
    static ServiceLocator instance;
    return instance;
}

void ServiceLocator::initialize(const ApplicationConfiguration& config) {
    auto& instance = getInstance();
    instance.configuration_ = std::make_shared<ApplicationConfiguration>(config);
}

const ApplicationConfiguration& ServiceLocator::getConfiguration() {
    auto& instance = getInstance();
    if (!instance.configuration_) {
        // En environnement embarqué, on ne peut pas lancer d'exceptions
        // Nous allons plutôt retourner une référence sur un objet statique par défaut
        static ApplicationConfiguration defaultConfig;
        return defaultConfig;
    }
    return *instance.configuration_;
}

ConfigurationService& ServiceLocator::getConfigurationService() {
    auto& instance = getInstance();
    // Vérification de sécurité - si le service n'est pas enregistré, renvoyer un service par défaut
    static ConfigurationService defaultService(getConfiguration());
    if (!instance.configurationService_) {
        return defaultService;
    }
    return *instance.configurationService_;
}

NavigationConfigService& ServiceLocator::getNavigationConfigService() {
    auto& instance = getInstance();
    // Vérification de sécurité - si le service n'est pas enregistré, renvoyer un service par défaut
    static NavigationConfigService defaultService;
    if (!instance.navigationConfigService_) {
        return defaultService;
    }
    return *instance.navigationConfigService_;
}

ProfileManager& ServiceLocator::getProfileManager() {
    auto& instance = getInstance();
    // Vérification de sécurité - si le service n'est pas enregistré, renvoyer un service par défaut
    static ProfileManager defaultService;
    if (!instance.profileManager_) {
        return defaultService;
    }
    return *instance.profileManager_;
}

MidiSystem& ServiceLocator::getMidiSystem() {
    auto& instance = getInstance();
    // Vérification de sécurité - si le service n'est pas enregistré, renvoyer un service par défaut
    static MidiSystem defaultService(getProfileManager());
    if (!instance.midiSystem_) {
        return defaultService;
    }
    return *instance.midiSystem_;
}

InputSystem& ServiceLocator::getInputSystem() {
    auto& instance = getInstance();
    // Vérification de sécurité - si le service n'est pas enregistré, renvoyer un service par défaut
    static InputSystem defaultService;
    if (!instance.inputSystem_) {
        return defaultService;
    }
    return *instance.inputSystem_;
}

UiEventService& ServiceLocator::getUiEventService() {
    auto& instance = getInstance();
    // Vérification de sécurité - si le service n'est pas enregistré, renvoyer un service par défaut
    static UiEventService defaultService;
    if (!instance.uiEventService_) {
        return defaultService;
    }
    return *instance.uiEventService_;
}

ControllerService& ServiceLocator::getControllerService() {
    auto& instance = getInstance();
    
    // Vérification de sécurité - si le service n'est pas enregistré
    if (!instance.controllerService_) {
#ifdef DEBUG
        Serial.println(F("ERREUR: ControllerService non enregistré !!"));
#endif
    }
    
    // Note: Cette ligne va provoquer un crash si le service n'est pas enregistré
    // C'est intentionnel - il ne faut pas utiliser ce service avant qu'il ne soit disponible
    static ControllerService* nullService = nullptr;
    return instance.controllerService_ ? *instance.controllerService_ : *nullService;
}

InputController& ServiceLocator::getInputController() {
    auto& instance = getInstance();
    // Vérification de sécurité - si le service n'est pas enregistré, renvoyer un service par défaut
    static InputController defaultController(getNavigationConfigService());
    if (!instance.inputController_) {
        return defaultController;
    }
    return *instance.inputController_;
}

UIController& ServiceLocator::getUIController() {
    auto& instance = getInstance();
    
    // Vérification de sécurité - si le service n'est pas enregistré
    if (!instance.uiController_) {
#ifdef DEBUG
        Serial.println(F("ERREUR: UIController non enregistré !!"));
#endif
    }
    
    // Note: Cette ligne va provoquer un crash si le service n'est pas enregistré
    // C'est intentionnel - il ne faut pas utiliser ce service avant qu'il ne soit disponible
    static UIController* nullController = nullptr;
    return instance.uiController_ ? *instance.uiController_ : *nullController;
}

UIEventListener& ServiceLocator::getUIControllerEventListener() {
    auto& instance = getInstance();
    
    // Vérification de sécurité - si l'écouteur n'est pas enregistré
    if (!instance.uiControllerEventListener_) {
#ifdef DEBUG
        Serial.println(F("ERREUR: UIControllerEventListener non enregistré !!"));
#endif
    }
    
    // Note: Cette ligne va provoquer un crash si l'écouteur n'est pas enregistré
    // C'est intentionnel - il ne faut pas utiliser cet écouteur avant qu'il ne soit disponible
    static UIEventListener* nullListener = nullptr;
    return instance.uiControllerEventListener_ ? *instance.uiControllerEventListener_ : *nullListener;
}

void ServiceLocator::registerConfigurationService(std::shared_ptr<ConfigurationService> service) {
    auto& instance = getInstance();
    instance.configurationService_ = service;
}

void ServiceLocator::registerNavigationConfigService(std::shared_ptr<NavigationConfigService> service) {
    auto& instance = getInstance();
    instance.navigationConfigService_ = service;
}

void ServiceLocator::registerProfileManager(std::shared_ptr<ProfileManager> service) {
    auto& instance = getInstance();
    instance.profileManager_ = service;
}

void ServiceLocator::registerMidiSystem(std::shared_ptr<MidiSystem> service) {
    auto& instance = getInstance();
    instance.midiSystem_ = service;
}

void ServiceLocator::registerInputSystem(std::shared_ptr<InputSystem> service) {
    auto& instance = getInstance();
    instance.inputSystem_ = service;
}

void ServiceLocator::registerUiEventService(std::shared_ptr<UiEventService> service) {
    auto& instance = getInstance();
    instance.uiEventService_ = service;
}

void ServiceLocator::registerControllerService(std::shared_ptr<ControllerService> service) {
    auto& instance = getInstance();
    instance.controllerService_ = service;
}

void ServiceLocator::registerInputController(std::shared_ptr<InputController> controller) {
    auto& instance = getInstance();
    instance.inputController_ = controller;
}

void ServiceLocator::registerUIController(std::shared_ptr<UIController> controller) {
    auto& instance = getInstance();
    instance.uiController_ = controller;
}

void ServiceLocator::registerUIControllerEventListener(std::shared_ptr<UIEventListener> listener) {
    auto& instance = getInstance();
    instance.uiControllerEventListener_ = listener;
}

void ServiceLocator::registerViewManager(std::shared_ptr<ViewManager> viewManager) {
    auto& instance = getInstance();
    instance.viewManager_ = viewManager;
}

void ServiceLocator::registerMenuController(std::shared_ptr<MenuController> menuController) {
    auto& instance = getInstance();
    instance.menuController_ = menuController;
}


EventBus& ServiceLocator::getEventBus() {
    return EventBus::getInstance();
}

ViewManager& ServiceLocator::getViewManager() {
    auto& instance = getInstance();
    
    // Vérification de sécurité - si le gestionnaire n'est pas enregistré
    if (!instance.viewManager_) {
#ifdef DEBUG
        Serial.println(F("ERREUR: ViewManager non enregistré !!"));
#endif
    }
    
    // Note: Cette ligne va provoquer un crash si le gestionnaire n'est pas enregistré
    // C'est intentionnel - il ne faut pas utiliser ce gestionnaire avant qu'il ne soit disponible
    static ViewManager* nullManager = nullptr;
    return instance.viewManager_ ? *instance.viewManager_ : *nullManager;
}

MenuController& ServiceLocator::getMenuController() {
    auto& instance = getInstance();
    
    // Vérification de sécurité - si le contrôleur n'est pas enregistré
    if (!instance.menuController_) {
#ifdef DEBUG
        Serial.println(F("ERREUR: MenuController non enregistré !!"));
#endif
    }
    
    // Note: Cette ligne va provoquer un crash si le contrôleur n'est pas enregistré
    // C'est intentionnel - il ne faut pas utiliser ce contrôleur avant qu'il ne soit disponible
    static MenuController* nullController = nullptr;
    return instance.menuController_ ? *instance.menuController_ : *nullController;
}
