#pragma once

#include <Arduino.h>

#include <memory>

#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "adapters/secondary/hardware/display/Ssd1306Display.hpp"
#include "adapters/secondary/midi/MidiMapper.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "adapters/secondary/storage/ProfileManager.hpp"
#include "app/di/DependencyContainer.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/subsystems/InputSubsystem.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "app/subsystems/UISubsystem.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/MenuController.hpp"
#include "core/controllers/UIController.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/ports/output/DisplayPort.hpp"
#include "core/ports/output/MidiOutputPort.hpp"
#include "core/ports/output/ProfileStoragePort.hpp"

/**
 * @brief Script d'initialisation pour configurer le conteneur de dépendances
 *
 * Cette classe statique fournit des méthodes pour initialiser le système
 * avec l'injection de dépendances.
 */
class InitializationScript {
public:
    /**
     * @brief Initialise le conteneur de dépendances avec tous les composants requis
     *
     * @param container Conteneur de dépendances à configurer
     * @param config Configuration de l'application
     * @return true si l'initialisation a réussi
     * @return false sinon
     */
    static bool initializeContainer(std::shared_ptr<DependencyContainer> container,
                                    const ApplicationConfiguration& config) {
        if (!container) {
            return false;
        }

        // Enregistrement de la configuration
        auto appConfigShared = std::make_shared<ApplicationConfiguration>(config);
        container->registerDependency<ApplicationConfiguration>(appConfigShared);

        // Adaptateurs primaires
        auto midiOut = std::make_shared<TeensyUsbMidiOut>();
        container->registerDependency<MidiOutputPort>(midiOut);

        // Création de l'adaptateur d'affichage SSD1306
        auto ssd1306Display = std::make_shared<Ssd1306Display>();
        bool displayInitSuccess = ssd1306Display->init();

        // Si l'initialisation de l'écran échoue, utiliser un MockDisplay
        if (!displayInitSuccess) {
            Serial.println(F("SSD1306 initialization failed. Using MockDisplay instead."));
        } else {
            Serial.println(F("SSD1306 initialized successfully."));
            container->registerDependency<DisplayPort>(ssd1306Display);
        }

        auto profileManager = std::make_shared<ProfileManager>();
        container->registerDependency<ProfileStoragePort>(profileManager);
        container->registerDependency<ProfileManager>(profileManager);

        // Services de base
        auto navConfig = std::make_shared<NavigationConfigService>();
        container->registerDependency<NavigationConfigService>(navConfig);

        // CommandManager (créé avant les sous-systèmes)
        auto commandManager = std::make_shared<CommandManager>();
        container->registerDependency<CommandManager>(commandManager);

        // Sous-systèmes
        auto configSystem = std::make_shared<ConfigurationSubsystem>(container);
        container->registerDependency<ConfigurationSubsystem>(configSystem);
        container->registerDependency<IConfiguration>(configSystem);

        auto midiSystem = std::make_shared<MidiSubsystem>(container);
        container->registerDependency<MidiSubsystem>(midiSystem);
        container->registerDependency<IMidiSystem>(midiSystem);

        // Initialisation du sous-système UI avec l'option d'UI complète
        auto uiSystem = std::make_shared<UISubsystem>(container);
        auto uiResult = uiSystem->init(true);  // true = activer l'UI complète

        if (uiResult.isError()) {
            Serial.print(F("UISubsystem initialization failed: "));
            if (auto err = uiResult.error()) {
                Serial.println(err->c_str());
            } else {
                Serial.println(F("Unknown error"));
            }
        } else {
            Serial.println(F("UISubsystem initialized successfully."));
        }

        container->registerDependency<UISubsystem>(uiSystem);
        container->registerDependency<IUISystem>(uiSystem);

        // Récupérer le ViewManager configuré dans UISubsystem
        auto viewManager = container->resolve<ViewManager>();

        if (!viewManager) {
            Serial.println(F("Warning: ViewManager not found, creating DefaultViewManager."));
            auto display = container->resolve<DisplayPort>();
            auto defaultViewManager = std::make_shared<DefaultViewManager>(display);
            if (!defaultViewManager->init()) {
                Serial.println(F("DefaultViewManager initialization failed."));
            }
            viewManager = defaultViewManager;
            container->registerDependency<ViewManager>(viewManager);
        }

        // Pour UIController, nous avons besoin d'un MenuController d'abord
        auto menuController = std::make_shared<MenuController>(*viewManager, *commandManager);
        container->registerDependency<MenuController>(menuController);

        // Maintenant nous pouvons créer UIController qui dépend de MenuController
        auto uiController = std::make_shared<UIController>(*viewManager, *menuController);
        container->registerDependency<UIController>(uiController);

        // InputController a besoin de NavigationConfigService
        auto inputController = std::make_shared<InputController>(navConfig);
        container->registerDependency<InputController>(inputController);

        // Configurer les interactions entre contrôleurs
        inputController->setUIController(uiController);

        // Créer InputSubsystem après avoir configuré l'InputController
        auto inputSystem = std::make_shared<InputSubsystem>(container);
        container->registerDependency<InputSubsystem>(inputSystem);
        container->registerDependency<IInputSystem>(inputSystem);

        return true;
    }
};
