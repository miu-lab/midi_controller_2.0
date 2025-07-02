#include "InitializationScript.hpp"

// Inclusions nécessaires pour l'implémentation
#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341Driver.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "adapters/secondary/storage/ProfileManager.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/subsystems/InputSubsystem.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "app/subsystems/UISubsystem.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/MenuController.hpp"
#include "core/controllers/UIController.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/domain/events/core/OptimizedEventBus.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/listeners/UIControllerEventListener.hpp"
#include "core/TaskScheduler.hpp"

#include "core/utils/Error.hpp"

Result<bool> InitializationScript::initializeContainer(
    std::shared_ptr<DependencyContainer> container, const ApplicationConfiguration& config) {
    if (!container) {
        return Result<bool>::error({ErrorCode::InvalidArgument, "Conteneur de dépendances invalide"});
    }

    // Étape 1: Services de base
    registerBaseServices(container, config);

    // Créer et enregistrer l'OptimizedEventBus
    auto optimizedEventBus = std::make_shared<OptimizedEventBus>();
    container->registerDependency<OptimizedEventBus>(optimizedEventBus);

    // Créer et enregistrer le TaskScheduler
    auto taskScheduler = std::make_shared<TaskScheduler>();
    container->registerDependency<TaskScheduler>(taskScheduler);

    // Étape 2: Adaptateurs matériels
    auto hardwareResult = setupHardwareAdapters(container);
    if (hardwareResult.isError()) return hardwareResult;

    // Étape 3: Sous-systèmes
    auto subsystemResult = initializeSubsystems(container);
    if (subsystemResult.isError()) return subsystemResult;

    // Étape 4: Contrôleurs et interactions
    if (!setupControllers(container)) {
        return Result<bool>::error({ErrorCode::InitializationFailed, "Échec lors de la configuration des contrôleurs"});
    }

    // Étape 5: Configuration des écouteurs MIDI prioritaires
    setupMidiEventListeners(container);

    return Result<bool>::success(true);
}

void InitializationScript::registerBaseServices(std::shared_ptr<DependencyContainer> container,
                                                const ApplicationConfiguration& config) {
    // Configuration de l'application - Créer un shared_ptr vers la référence existante
    // Note: On assume que config reste valide pendant la durée de vie du container
    std::shared_ptr<ApplicationConfiguration> configPtr(
        const_cast<ApplicationConfiguration*>(&config), 
        [](ApplicationConfiguration*) {
            // Custom deleter qui ne fait rien car on ne possède pas l'objet
        });
    container->registerDependency<ApplicationConfiguration>(configPtr);

    // Service de navigation
    container->registerDependency<NavigationConfigService>(
        std::make_shared<NavigationConfigService>());

    // Gestionnaire de commandes
    container->registerDependency<CommandManager>(std::make_shared<CommandManager>());
}

Result<bool> InitializationScript::setupHardwareAdapters(
    std::shared_ptr<DependencyContainer> container) {
    // MIDI Out
    container->registerDependency<MidiOutputPort>(std::make_shared<TeensyUsbMidiOut>());

    // Écran LVGL - Nouvelle architecture modulaire
    // DEBUG MSG TO IMPLEMENT
    
    // 1. Créer et initialiser le driver hardware
    Ili9341Driver::Config driverConfig = Ili9341Driver::getDefaultConfig();
    auto driver = std::make_shared<Ili9341Driver>(driverConfig);
    if (!driver->initialize()) {
        return Result<bool>::error({ErrorCode::HardwareError, "Échec d'initialisation du driver hardware ILI9341"});
    }
    // DEBUG MSG TO IMPLEMENT
    
    // 2. Créer et initialiser le bridge LVGL
    // DEBUG MSG TO IMPLEMENT
    Ili9341LvglBridge::LvglConfig lvglConfig = Ili9341LvglBridge::getDefaultLvglConfig();
    auto bridge = std::make_shared<Ili9341LvglBridge>(driver, lvglConfig);
    if (!bridge->initialize()) {
        return Result<bool>::error({ErrorCode::HardwareError, "Échec d'initialisation du bridge LVGL"});
    }
    // DEBUG MSG TO IMPLEMENT
    
    // 3. Enregistrer les composants dans le container
    container->registerDependency<Ili9341Driver>(driver);
    container->registerDependency<Ili9341LvglBridge>(bridge);
    // DEBUG MSG TO IMPLEMENT

    // Stockage de profils
    auto profileManager = std::make_shared<ProfileManager>();
    container->registerDependency<ProfileStoragePort>(profileManager);
    container->registerDependency<ProfileManager>(profileManager);

    return Result<bool>::success(true);
}

Result<bool> InitializationScript::initializeSubsystems(
    std::shared_ptr<DependencyContainer> container) {
    // Créer le contrôleur d'entrée avant les sous-systèmes
    auto navConfig = container->resolve<NavigationConfigService>();
    if (!navConfig) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Impossible de résoudre NavigationConfigService"});
    }

    // Récupérer l'OptimizedEventBus
    auto optimizedEventBus = container->resolve<OptimizedEventBus>();
    if (!optimizedEventBus) {
        // DEBUG MSG TO IMPLEMENT
    }

    // Créer InputController avec l'OptimizedEventBus
    auto inputController = std::make_shared<InputController>(navConfig, optimizedEventBus);
    container->registerDependency<InputController>(inputController);

    // Récupérer le TaskScheduler
    auto scheduler = container->resolve<TaskScheduler>();
    if (!scheduler) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Impossible de résoudre TaskScheduler"});
    }

    // Définition des sous-systèmes à initialiser dans l'ordre
    std::vector<SubsystemInfo> subsystems;
    subsystems.push_back({"Configuration",
         std::make_shared<ConfigurationSubsystem>(container),
         [&]() -> Result<bool> {
             auto system = std::static_pointer_cast<ConfigurationSubsystem>(subsystems[0].instance);
             container->registerDependency<ConfigurationSubsystem>(system);
             container->registerDependency<IConfiguration>(system);
             return system->init();
         }});
    subsystems.push_back({"Input",
         std::make_shared<InputSubsystem>(container),
         [&]() -> Result<bool> {
             auto system = std::static_pointer_cast<InputSubsystem>(subsystems[1].instance);
             container->registerDependency<InputSubsystem>(system);
             container->registerDependency<IInputSystem>(system);
             auto initResult = system->init();
             if (initResult.isSuccess()) {
                 scheduler->addTask([system]() { system->update(); },
                                    PerformanceConfig::INPUT_TIME_INTERVAL,
                                    0,
                                    "InputUpdate");  // 1ms pour l'InputSystem
             }
             return initResult;
         }});
    subsystems.push_back({"MIDI",
         std::make_shared<MidiSubsystem>(container),
         [&]() -> Result<bool> {
             auto system = std::static_pointer_cast<MidiSubsystem>(subsystems[2].instance);
             container->registerDependency<MidiSubsystem>(system);
             container->registerDependency<IMidiSystem>(system);
             auto initResult = system->init();
             if (initResult.isSuccess()) {
                 scheduler->addTask([system]() { system->update(); },
                                    PerformanceConfig::MIDI_TIME_INTERVAL,
                                    1,
                                    "MidiUpdate");  // 1ms pour le MidiSubsystem
             }
             return initResult;
         }});
    subsystems.push_back({"UI", std::make_shared<UISubsystem>(container), [&]() -> Result<bool> {
             auto system = std::static_pointer_cast<UISubsystem>(subsystems[3].instance);
             container->registerDependency<UISubsystem>(system);
             container->registerDependency<IUISystem>(system);
             auto initResult = system->init(true);  // true = enable full UI
             if (initResult.isSuccess()) {
                 scheduler->addTask([system]() { system->update(); },
                                    PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS * 1000,
                                    1,
                                    "UIUpdate");  // 16ms pour l'UISubsystem (approx 60 FPS)
             }
             return initResult;
         }});

    // Initialiser chaque sous-système séquentiellement
    for (auto& info : subsystems) {
        auto result = info.initFn();
        if (result.isError()) {
            auto err = result.error().value_or(Errors::Unknown);
            // DEBUG MSG TO IMPLEMENT
            return Result<bool>::error(err);
        }
    }

    return Result<bool>::success(true);
}

bool InitializationScript::setupControllers(std::shared_ptr<DependencyContainer> container) {
    // Récupérer les composants nécessaires
    auto viewManager = container->resolve<ViewManager>();
    auto commandManager = container->resolve<CommandManager>();

    if (!viewManager || !commandManager) {
        return false;
    }

    // Créer et enregistrer les contrôleurs
    auto menuController = std::make_shared<MenuController>(*viewManager, *commandManager);
    container->registerDependency<MenuController>(menuController);

    auto uiController = std::make_shared<UIController>(*viewManager, *menuController);
    container->registerDependency<UIController>(uiController);

    // Configurer l'interaction entre InputController et UIController
    auto inputController = container->resolve<InputController>();

    // Instancier et abonner UIControllerEventListener
    auto uiControllerRef = *uiController; // Obtenir une référence
    auto navServiceRef = *container->resolve<NavigationConfigService>(); // Obtenir une référence
    auto uiEventListener = std::make_shared<UIControllerEventListener>(uiControllerRef, navServiceRef);
    container->registerDependency<UIControllerEventListener>(uiEventListener);

    auto optimizedEventBus = container->resolve<OptimizedEventBus>();
    if (optimizedEventBus) {
        optimizedEventBus->subscribe(uiEventListener.get());
        // DEBUG MSG TO IMPLEMENT
    } else {
        // DEBUG MSG TO IMPLEMENT
    }

    return true;
}

void InitializationScript::setupMidiEventListeners(std::shared_ptr<DependencyContainer> container) {
    // DEBUG MSG TO IMPLEMENT
    
    // Récupération des composants nécessaires
    auto midiSystem = container->resolve<MidiSubsystem>();
    auto optimizedEventBus = container->resolve<OptimizedEventBus>();

    if (!midiSystem) {
        return;
    }
    
    if (!optimizedEventBus) {
        return;
    }
    MidiMapper& midiMapper = midiSystem->getMidiMapper();
    SubscriptionId subscriptionId = optimizedEventBus->subscribeWithPriority(&midiMapper, EventPriority::PRIORITY_HIGH);
    
    if (subscriptionId != 0) {
    } else {
        // DEBUG MSG TO IMPLEMENT
        // Échec de l'abonnement, gérer l'erreur
        return;
    }
    optimizedEventBus->setPropagateHighPriorityEvents(false);
}
