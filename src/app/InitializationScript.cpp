#include "InitializationScript.hpp"

#include <Arduino.h>

// Inclusions nécessaires pour l'implémentation
#include "adapters/ui/views/DefaultViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341Driver.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "adapters/secondary/storage/ProfileManager.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/subsystems/InputSubsystem.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
// #include "app/subsystems/NavigationSubsystem.hpp" // TODO: Réintégrer après refactoring interface
#include "app/subsystems/UISubsystem.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/MenuController.hpp"
#include "core/controllers/NavigationController.hpp"
#include "core/domain/navigation/NavigationStateManager.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/core/IEventBus.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/TaskScheduler.hpp"
#include "core/memory/EventPoolManager.hpp"

#include "core/utils/Error.hpp"

Result<bool> InitializationScript::initializeContainer(
    std::shared_ptr<DependencyContainer> container, const ApplicationConfiguration& config) {
    if (!container) {
        return Result<bool>::error({ErrorCode::InvalidArgument, "Conteneur de dépendances invalide"});
    }

    // Étape 1: Services de base
    Serial.println("Registering base services...");
    registerBaseServices(container, config);

    // Créer et enregistrer l'EventPoolManager
    Serial.println("Setting up EventPoolManager...");
    auto eventPoolManager = std::make_shared<EventPoolManager>();
    container->registerDependency<EventPoolManager>(eventPoolManager);
    
    // Initialiser l'EventFactory avec le pool manager
    EventFactory::initialize(eventPoolManager);

    // Créer et enregistrer l'EventBus via injection de dépendance
    Serial.println("Setting up EventBus...");
    auto eventBus = std::make_shared<EventBus>();
    container->registerDependency<EventBus>(eventBus);
    container->registerDependency<MidiController::Events::IEventBus>(eventBus);

    // Créer et enregistrer le TaskScheduler
    auto taskScheduler = std::make_shared<TaskScheduler>();
    container->registerDependency<TaskScheduler>(taskScheduler);

    // Étape 2: Adaptateurs matériels
    Serial.println("Setting up hardware adapters...");
    auto hardwareResult = setupHardwareAdapters(container);
    if (hardwareResult.isError()) {
        Serial.print("Hardware setup failed: ");
        Serial.println(hardwareResult.error().value().message);
        return hardwareResult;
    }

    // Étape 3: Sous-systèmes
    Serial.println("Initializing subsystems...");
    auto subsystemResult = initializeSubsystems(container);
    if (subsystemResult.isError()) {
        Serial.print("Subsystem init failed: ");
        Serial.println(subsystemResult.error().value().message);
        return subsystemResult;
    }

    // Étape 3.5: Services de navigation (après création du ViewManager)
    Serial.println("Registering navigation services...");
    registerNavigationServices(container);

    // Étape 3.6: Créer InputController (après UnifiedConfiguration disponible)
    Serial.println("Creating InputController...");
    auto inputResult = createInputController(container);
    if (inputResult.isError()) {
        Serial.print("InputController creation failed: ");
        Serial.println(inputResult.error().value().message);
        return inputResult;
    }

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
    // Configuration de l'application - Wrapped pointer avec garantie de cycle de vie
    // Note: Le caller garantit que 'config' reste valide pendant toute la durée de vie du container
    std::shared_ptr<ApplicationConfiguration> configPtr(
        const_cast<ApplicationConfiguration*>(&config), 
        [](ApplicationConfiguration*) {
            // Custom deleter vide - l'objet n'est pas détruit par ce shared_ptr
            // L'ownership réel reste chez le caller
        });
    container->registerDependency<ApplicationConfiguration>(configPtr);

    // REFACTOR: Service de navigation avec interface
    auto navigationService = std::make_shared<NavigationConfigService>();
    container->registerDependency<NavigationConfigService>(navigationService);
    container->registerDependency<INavigationService>(navigationService);

    // Gestionnaire de commandes
    container->registerDependency<CommandManager>(std::make_shared<CommandManager>());
}

Result<bool> InitializationScript::setupHardwareAdapters(
    std::shared_ptr<DependencyContainer> container) {
    // MIDI Out
    container->registerDependency<MidiOutputPort>(std::make_shared<TeensyUsbMidiOut>());

    // Écran LVGL - Nouvelle architecture modulaire
    // TODO DEBUG MSG
    
    // 1. Créer et initialiser le driver hardware
    Ili9341Driver::Config driverConfig = Ili9341Driver::getDefaultConfig();
    auto driver = std::make_shared<Ili9341Driver>(driverConfig);
    auto driverInitResult = driver->initialize();
    if (driverInitResult.isError()) {
        return Result<bool>::error({ErrorCode::HardwareError, "Échec d'initialisation du driver hardware ILI9341"});
    }
    // TODO DEBUG MSG
    
    // 2. Créer et initialiser le bridge LVGL
    // TODO DEBUG MSG
    Ili9341LvglBridge::LvglConfig lvglConfig = Ili9341LvglBridge::getDefaultLvglConfig();
    auto bridge = std::make_shared<Ili9341LvglBridge>(driver, lvglConfig);
    auto bridgeInitResult = bridge->initialize();
    if (bridgeInitResult.isError()) {
        return Result<bool>::error({ErrorCode::HardwareError, "Échec d'initialisation du bridge LVGL"});
    }
    // TODO DEBUG MSG
    
    // 3. Enregistrer les composants dans le container
    container->registerDependency<Ili9341Driver>(driver);
    container->registerDependency<Ili9341LvglBridge>(bridge);
    // TODO DEBUG MSG

    // Stockage de profils
    auto profileManager = std::make_shared<ProfileManager>();
    container->registerDependency<ProfileStoragePort>(profileManager);
    container->registerDependency<ProfileManager>(profileManager);

    return Result<bool>::success(true);
}

Result<bool> InitializationScript::initializeSubsystems(
    std::shared_ptr<DependencyContainer> container) {
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
    // TODO: Réintégrer NavigationSubsystem après refactoring interface
    // subsystems.push_back({"Navigation",
    //      std::make_shared<NavigationSubsystem>(container),
    //      [&]() -> Result<bool> {
    //          auto system = std::static_pointer_cast<NavigationSubsystem>(subsystems[1].instance);
    //          container->registerDependency<NavigationSubsystem>(system);
    //          container->registerDependency<INavigationService>(system);
    //          auto initResult = system->init();
    //          if (initResult.isSuccess()) {
    //              scheduler->addTask([system]() { system->update(); },
    //                                 PerformanceConfig::NAVIGATION_TIME_INTERVAL,
    //                                 0,
    //                                 "NavigationUpdate");
    //          }
    //          return initResult;
    //      }});
    subsystems.push_back({"Input",
         std::make_shared<InputSubsystem>(container),
         [&]() -> Result<bool> {
             auto system = std::static_pointer_cast<InputSubsystem>(subsystems[1].instance);
             container->registerDependency<InputSubsystem>(system);
             container->registerDependency<IInputSystem>(system);
             auto initResult = system->init();
             if (initResult.isSuccess()) {
                 scheduler->addTask([system]() { system->update(); },
                                    SystemConstants::Performance::INPUT_TIME_INTERVAL,
                                    0,
                                    "InputUpdate");
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
                                    SystemConstants::Performance::MIDI_TIME_INTERVAL,
                                    1,
                                    "MidiUpdate");
                 
                 // REFACTOR: Navigation maintenant gérée par InputSubsystem
                 // Plus besoin de synchronisation manuelle depuis MidiSubsystem
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
                                    SystemConstants::Performance::DISPLAY_REFRESH_PERIOD_MS * 1000,
                                    1,
                                    "UIUpdate");
             }
             return initResult;
         }});

    // Initialiser chaque sous-système séquentiellement
    for (auto& info : subsystems) {
        auto result = info.initFn();
        if (result.isError()) {
            auto err = result.error().value_or(Errors::Unknown);
            // TODO DEBUG MSG
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

    // Initialiser le NavigationController
    auto navigationController = container->resolve<NavigationController>();
    if (navigationController) {
        navigationController->initialize();
    }

    return true;
}

void InitializationScript::setupMidiEventListeners(std::shared_ptr<DependencyContainer> container) {
    // TODO DEBUG MSG
    
    // Récupération des composants nécessaires
    auto midiSystem = container->resolve<MidiSubsystem>();
    auto eventBus = container->resolve<EventBus>();

    if (!midiSystem) {
        return;
    }
    
    if (!eventBus) {
        return;
    }
    MidiMapper& midiMapper = midiSystem->getMidiMapper();
    SubscriptionId subscriptionId = eventBus->subscribeHigh(&midiMapper);
    
    if (subscriptionId != 0) {
    } else {
        // TODO DEBUG MSG
        // Échec de l'abonnement, gérer l'erreur
        return;
    }
    // Plus besoin de configuration de propagation - tout est géré automatiquement
}

// REFACTOR: Méthode supprimée - la synchronisation est maintenant gérée automatiquement
// par InputSubsystem lors de son initialisation

void InitializationScript::registerNavigationServices(std::shared_ptr<DependencyContainer> container) {
    // Récupérer les dépendances nécessaires
    auto viewManager = container->resolve<ViewManager>();
    auto eventBus = container->resolve<EventBus>();
    
    if (!viewManager || !eventBus) {
        Serial.println("Error: ViewManager or EventBus not available for navigation services");
        return;
    }
    
    // Créer NavigationStateManager
    auto stateManager = std::make_shared<NavigationStateManager>(*viewManager);
    container->registerDependency<NavigationStateManager>(stateManager);
    
    // Créer NavigationController
    auto navigationController = std::make_shared<NavigationController>(stateManager, eventBus);
    container->registerDependency<NavigationController>(navigationController);
    
    Serial.println("Navigation services registered successfully");
}

Result<bool> InitializationScript::createInputController(std::shared_ptr<DependencyContainer> container) {
    // Créer le contrôleur d'entrée après l'initialisation des sous-systèmes
    auto navConfig = container->resolve<NavigationConfigService>();
    if (!navConfig) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Impossible de résoudre NavigationConfigService"});
    }

    // Récupérer l'EventBus
    auto eventBus = container->resolve<EventBus>();
    if (!eventBus) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Impossible de résoudre EventBus"});
    }

    // Récupérer UnifiedConfiguration pour InputController
    auto unifiedConfig = container->resolve<UnifiedConfiguration>();
    if (!unifiedConfig) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Impossible de résoudre UnifiedConfiguration"});
    }
    
    // Créer InputController avec toutes les dépendances
    auto inputController = std::make_shared<InputController>(navConfig, unifiedConfig, eventBus);
    container->registerDependency<InputController>(inputController);

    return Result<bool>::success(true);
}
