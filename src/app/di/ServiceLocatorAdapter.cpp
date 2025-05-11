#include "app/di/ServiceLocatorAdapter.hpp"

#include "app/services/ConfigurationService.hpp"
#include "app/services/ControllerService.hpp"
#include "app/services/EventInputSystem.hpp"
#include "app/services/InputSystem.hpp"
#include "app/services/MidiSystem.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/services/UiEventService.hpp"
#include "adapters/secondary/storage/ProfileManager.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/MenuController.hpp"
#include "core/controllers/UIController.hpp"
#include "core/domain/events/EventSystem.hpp"
#include "core/listeners/UIEventListener.hpp"
#include "adapters/primary/ui/ViewManager.hpp"

ServiceLocatorAdapter::ServiceLocatorAdapter(std::shared_ptr<DependencyContainer> container)
    : container_(container), configuration_(nullptr) {
}

void ServiceLocatorAdapter::initialize(const ApplicationConfiguration& config) {
    configuration_ = std::make_shared<ApplicationConfiguration>(config);
    container_->registerDependency<ApplicationConfiguration>(configuration_);
}

const ApplicationConfiguration& ServiceLocatorAdapter::getConfiguration() const {
    if (!configuration_) {
        static ApplicationConfiguration defaultConfig;
        return defaultConfig;
    }
    return *configuration_;
}

ConfigurationService& ServiceLocatorAdapter::getConfigurationService() {
    auto service = container_->resolve<ConfigurationService>();
    if (!service) {
        static ConfigurationService defaultService(getConfiguration());
        return defaultService;
    }
    return *service;
}

NavigationConfigService& ServiceLocatorAdapter::getNavigationConfigService() {
    auto service = container_->resolve<NavigationConfigService>();
    if (!service) {
        static NavigationConfigService defaultService;
        return defaultService;
    }
    return *service;
}

ProfileManager& ServiceLocatorAdapter::getProfileManager() {
    auto service = container_->resolve<ProfileManager>();
    if (!service) {
        static ProfileManager defaultService;
        return defaultService;
    }
    return *service;
}

MidiSystem& ServiceLocatorAdapter::getMidiSystem() {
    auto service = container_->resolve<MidiSystem>();
    if (!service) {
        static MidiSystem defaultService(getProfileManager());
        return defaultService;
    }
    return *service;
}

InputSystem& ServiceLocatorAdapter::getInputSystem() {
    auto service = container_->resolve<InputSystem>();
    if (!service) {
        static InputSystem defaultService;
        return defaultService;
    }
    return *service;
}

UiEventService& ServiceLocatorAdapter::getUiEventService() {
    auto service = container_->resolve<UiEventService>();
    if (!service) {
        static UiEventService defaultService;
        return defaultService;
    }
    return *service;
}

ControllerService& ServiceLocatorAdapter::getControllerService() {
    auto service = container_->resolve<ControllerService>();
    
    // C'est un service critique, on retourne un pointeur null si non disponible
    // pour être cohérent avec le comportement du ServiceLocator original
    static ControllerService* nullService = nullptr;
    return service ? *service : *nullService;
}

InputController& ServiceLocatorAdapter::getInputController() {
    auto controller = container_->resolve<InputController>();
    if (!controller) {
        static InputController defaultController(getNavigationConfigService());
        return defaultController;
    }
    return *controller;
}

UIController& ServiceLocatorAdapter::getUIController() {
    auto controller = container_->resolve<UIController>();
    
    // C'est un contrôleur critique, on retourne un pointeur null si non disponible
    static UIController* nullController = nullptr;
    return controller ? *controller : *nullController;
}

UIEventListener& ServiceLocatorAdapter::getUIControllerEventListener() {
    auto listener = container_->resolve<UIEventListener>();
    
    // C'est un écouteur critique, on retourne un pointeur null si non disponible
    static UIEventListener* nullListener = nullptr;
    return listener ? *listener : *nullListener;
}

EventBus& ServiceLocatorAdapter::getEventBus() {
    return EventBus::getInstance();
}

ViewManager& ServiceLocatorAdapter::getViewManager() {
    auto manager = container_->resolve<ViewManager>();
    
    // C'est un gestionnaire critique, on retourne un pointeur null si non disponible
    static ViewManager* nullManager = nullptr;
    return manager ? *manager : *nullManager;
}

MenuController& ServiceLocatorAdapter::getMenuController() {
    auto controller = container_->resolve<MenuController>();
    
    // C'est un contrôleur critique, on retourne un pointeur null si non disponible
    static MenuController* nullController = nullptr;
    return controller ? *controller : *nullController;
}

void ServiceLocatorAdapter::registerConfigurationService(ConfigurationService* service) {
    if (service) {
        container_->registerDependency<ConfigurationService>(std::shared_ptr<ConfigurationService>(service, [](ConfigurationService*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerNavigationConfigService(NavigationConfigService* service) {
    if (service) {
        container_->registerDependency<NavigationConfigService>(std::shared_ptr<NavigationConfigService>(service, [](NavigationConfigService*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerProfileManager(ProfileManager* service) {
    if (service) {
        container_->registerDependency<ProfileManager>(std::shared_ptr<ProfileManager>(service, [](ProfileManager*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerMidiSystem(MidiSystem* service) {
    if (service) {
        container_->registerDependency<MidiSystem>(std::shared_ptr<MidiSystem>(service, [](MidiSystem*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerInputSystem(InputSystem* service) {
    if (service) {
        container_->registerDependency<InputSystem>(std::shared_ptr<InputSystem>(service, [](InputSystem*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerUiEventService(UiEventService* service) {
    if (service) {
        container_->registerDependency<UiEventService>(std::shared_ptr<UiEventService>(service, [](UiEventService*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerControllerService(ControllerService* service) {
    if (service) {
        // Utiliser un raw pointer pour éviter les problèmes de dépendance circulaire
        container_->registerDependency<ControllerService>(std::shared_ptr<ControllerService>(service, [](ControllerService*) {
            // Ne rien faire lors de la destruction
            // Note: cela peut causer des fuites de mémoire, mais c'est nécessaire pour éviter les problèmes de dépendance circulaire
        }));
    }
}

void ServiceLocatorAdapter::registerInputController(InputController* controller) {
    if (controller) {
        container_->registerDependency<InputController>(std::shared_ptr<InputController>(controller, [](InputController*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerUIController(UIController* controller) {
    if (controller) {
        container_->registerDependency<UIController>(std::shared_ptr<UIController>(controller, [](UIController*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerUIControllerEventListener(UIEventListener* listener) {
    if (listener) {
        container_->registerDependency<UIEventListener>(std::shared_ptr<UIEventListener>(listener, [](UIEventListener*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerViewManager(ViewManager* viewManager) {
    if (viewManager) {
        container_->registerDependency<ViewManager>(std::shared_ptr<ViewManager>(viewManager, [](ViewManager*) {
            // Ne rien faire lors de la destruction
        }));
    }
}

void ServiceLocatorAdapter::registerMenuController(MenuController* menuController) {
    if (menuController) {
        container_->registerDependency<MenuController>(std::shared_ptr<MenuController>(menuController, [](MenuController*) {
            // Ne rien faire lors de la destruction
        }));
    }
}
