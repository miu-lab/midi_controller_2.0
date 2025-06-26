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

Result<bool, std::string> InitializationScript::initializeContainer(
    std::shared_ptr<DependencyContainer> container, const ApplicationConfiguration& config) {
    if (!container) {
        return Result<bool, std::string>::error("Conteneur de dépendances invalide");
    }

    // Étape 1: Services de base
    registerBaseServices(container, config);

    // Créer et enregistrer l'OptimizedEventBus
    auto optimizedEventBus = std::make_shared<OptimizedEventBus>();
    container->registerDependency<OptimizedEventBus>(optimizedEventBus);

    // Étape 2: Adaptateurs matériels
    auto hardwareResult = setupHardwareAdapters(container);
    if (hardwareResult.isError()) return hardwareResult;

    // Étape 3: Sous-systèmes
    auto subsystemResult = initializeSubsystems(container);
    if (subsystemResult.isError()) return subsystemResult;

    // Étape 4: Contrôleurs et interactions
    if (!setupControllers(container)) {
        return Result<bool, std::string>::error("Échec lors de la configuration des contrôleurs");
    }

    // Étape 5: Configuration des écouteurs MIDI prioritaires
    setupMidiEventListeners(container);

    return Result<bool, std::string>::success(true);
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

Result<bool, std::string> InitializationScript::setupHardwareAdapters(
    std::shared_ptr<DependencyContainer> container) {
    // MIDI Out
    container->registerDependency<MidiOutputPort>(std::make_shared<TeensyUsbMidiOut>());

    // Écran LVGL - Nouvelle architecture modulaire
    Serial.println(F("Initializing display hardware driver..."));
    
    // 1. Créer et initialiser le driver hardware
    Ili9341Driver::Config driverConfig = Ili9341Driver::getDefaultConfig();
    auto driver = std::make_shared<Ili9341Driver>(driverConfig);
    if (!driver->initialize()) {
        return Result<bool, std::string>::error("Échec d'initialisation du driver hardware ILI9341");
    }
    Serial.println(F("Hardware driver initialized successfully"));
    
    // 2. Créer et initialiser le bridge LVGL
    Serial.println(F("Initializing LVGL bridge..."));
    Ili9341LvglBridge::LvglConfig lvglConfig = Ili9341LvglBridge::getDefaultLvglConfig();
    auto bridge = std::make_shared<Ili9341LvglBridge>(driver, lvglConfig);
    if (!bridge->initialize()) {
        return Result<bool, std::string>::error("Échec d'initialisation du bridge LVGL");
    }
    Serial.println(F("LVGL bridge initialized successfully"));
    
    // 3. Enregistrer les composants dans le container
    container->registerDependency<Ili9341Driver>(driver);
    container->registerDependency<Ili9341LvglBridge>(bridge);
    Serial.println(F("LVGL display components registered in container"));

    // Stockage de profils
    auto profileManager = std::make_shared<ProfileManager>();
    container->registerDependency<ProfileStoragePort>(profileManager);
    container->registerDependency<ProfileManager>(profileManager);

    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> InitializationScript::initializeSubsystems(
    std::shared_ptr<DependencyContainer> container) {
    // Créer le contrôleur d'entrée avant les sous-systèmes
    auto navConfig = container->resolve<NavigationConfigService>();
    if (!navConfig) {
        return Result<bool, std::string>::error("Impossible de résoudre NavigationConfigService");
    }

    // Récupérer l'OptimizedEventBus
    auto optimizedEventBus = container->resolve<OptimizedEventBus>();
    if (!optimizedEventBus) {
        Serial.println(
            F("AVERTISSEMENT: OptimizedEventBus non disponible, utilisant le mode legacy"));
    }

    // Créer InputController avec l'OptimizedEventBus
    auto inputController = std::make_shared<InputController>(navConfig, optimizedEventBus);
    container->registerDependency<InputController>(inputController);

    // Définition des sous-systèmes à initialiser dans l'ordre
    std::vector<SubsystemInfo> subsystems = {
        {"Configuration",
         std::make_shared<ConfigurationSubsystem>(container),
         [&container, &subsystems]() {
             auto system = std::static_pointer_cast<ConfigurationSubsystem>(subsystems[0].instance);
             container->registerDependency<ConfigurationSubsystem>(system);
             container->registerDependency<IConfiguration>(system);
             return system->init();
         }},
        {"Input",
         std::make_shared<InputSubsystem>(container),
         [&container, &subsystems]() {
             auto system = std::static_pointer_cast<InputSubsystem>(subsystems[1].instance);
             container->registerDependency<InputSubsystem>(system);
             container->registerDependency<IInputSystem>(system);
             return system->init();
         }},
        {"MIDI",
         std::make_shared<MidiSubsystem>(container),
         [&container, &subsystems]() {
             auto system = std::static_pointer_cast<MidiSubsystem>(subsystems[2].instance);
             container->registerDependency<MidiSubsystem>(system);
             container->registerDependency<IMidiSystem>(system);
             return system->init();
         }},
        {"UI", std::make_shared<UISubsystem>(container), [&container, &subsystems]() {
             auto system = std::static_pointer_cast<UISubsystem>(subsystems[3].instance);
             container->registerDependency<UISubsystem>(system);
             container->registerDependency<IUISystem>(system);
             return system->init(true);  // true = enable full UI
         }}};

    // Initialiser chaque sous-système séquentiellement
    for (auto& info : subsystems) {
        auto result = info.initFn();
        if (result.isError()) {
            // Convertir le std::string en const char* pour la concaténation Arduino
            std::string errorMsg = *(result.error());
            char msgBuffer[128];
            snprintf(msgBuffer,
                     sizeof(msgBuffer),
                     "Échec d'initialisation du sous-système %s: %s",
                     info.name,
                     errorMsg.c_str());
            return Result<bool, std::string>::error(msgBuffer);
        }
    }

    return Result<bool, std::string>::success(true);
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
        Serial.println(F("UIControllerEventListener abonné à l'EventBus."));
    } else {
        Serial.println(F("AVERTISSEMENT: OptimizedEventBus non disponible pour UIControllerEventListener."));
    }

    return true;
}

void InitializationScript::setupMidiEventListeners(std::shared_ptr<DependencyContainer> container) {
    Serial.println(F("=== Configuration des écouteurs MIDI ==="));
    
    // Récupération des composants nécessaires
    auto midiSystem = container->resolve<MidiSubsystem>();
    auto optimizedEventBus = container->resolve<OptimizedEventBus>();

    if (!midiSystem) {
        Serial.println(F("ERREUR: MidiSubsystem non disponible"));
        return;
    }
    
    if (!optimizedEventBus) {
        Serial.println(F("ERREUR: OptimizedEventBus non disponible"));
        return;
    }
    
    Serial.println(F("MidiSubsystem et OptimizedEventBus trouvés"));

    // Récupérer le MidiMapper du MidiSubsystem
    Serial.println(F("Récupération du MidiMapper..."));
    MidiMapper& midiMapper = midiSystem->getMidiMapper();
    Serial.print(F("MidiMapper obtenu à l'adresse: 0x"));
    Serial.println((uintptr_t)&midiMapper, HEX);

    // Enregistrer le MidiMapper comme écouteur de haute priorité
    Serial.println(F("Enregistrement du MidiMapper dans l'EventBus..."));
    SubscriptionId subscriptionId = optimizedEventBus->subscribeWithPriority(&midiMapper, EventPriority::PRIORITY_HIGH);
    
    if (subscriptionId != 0) {
        Serial.print(F("MidiMapper enregistré avec succès - ID: "));
        Serial.println(subscriptionId);
    } else {
        Serial.println(F("ERREUR: Échec de l'enregistrement du MidiMapper"));
    }

    // Configurer la propagation des événements haute priorité
    optimizedEventBus->setPropagateHighPriorityEvents(false);
    Serial.println(F("Configuration des écouteurs MIDI terminée"));
}
