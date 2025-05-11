#pragma once

#include <memory>

#include "app/di/DependencyContainer.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "adapters/secondary/storage/ProfileManager.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/subsystems/InputSubsystem.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "app/subsystems/UISubsystem.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/controllers/UIController.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/MenuController.hpp"
#include "core/domain/IDisplay.hpp"
#include "core/domain/IMidiOut.hpp"
#include "core/domain/IProfileManager.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "adapters/secondary/midi/MidiMapper.hpp"
#include <Arduino.h>

// Importer tous les mocks dans un seul fichier d'inclusion
#include "app/mocks/AllMocks.hpp"

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
        container->registerDependency<IMidiOut>(midiOut);
        
        // Utilisation de notre mock pour l'affichage
        auto display = std::make_shared<MockDisplay>();
        container->registerDependency<IDisplay>(display);
        
        auto profileManager = std::make_shared<ProfileManager>();
        container->registerDependency<IProfileManager>(profileManager);
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
        
        auto uiSystem = std::make_shared<UISubsystem>(container);
        container->registerDependency<UISubsystem>(uiSystem);
        container->registerDependency<IUISystem>(uiSystem);
        
        // Création des composants d'interface utilisateur avec le mock ViewManager
        auto viewManager = std::make_shared<ViewManagerMock>(*display);
        container->registerDependency<ViewManager>(viewManager);
        
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
        
        // Configuration des callbacks pour MIDI
        if (midiSystem && inputController) {
            
            // CORRECTION: Utiliser capturer midiSystem par référence pour maintenir le pointeur valide
            // Obtenir une référence au MidiMapper pour la capture optimale
            auto& midiMapper = midiSystem->getMidiMapper();
            
            // Callback pour les rotations d'encodeurs
            inputController->setMidiEncoderCallback(
                [&midiMapper](EncoderId id, int32_t position, int8_t delta) {
                    
                    midiMapper.processEncoderChange(id, position);
                }
            );
            
            // Callback pour les boutons d'encodeurs
            inputController->setMidiEncoderButtonCallback(
                [&midiMapper](EncoderId id, bool pressed) {
                    
                    midiMapper.processEncoderButton(id, pressed);
                }
            );
            
            // Callback pour les boutons
            inputController->setMidiButtonCallback(
                [&midiMapper](ButtonId id, bool pressed) {
                    
                    midiMapper.processButtonPress(id, pressed);
                }
            );
            
        } else {
        }
        
        // Créer InputSubsystem après avoir configuré l'InputController
        auto inputSystem = std::make_shared<InputSubsystem>(container);
        container->registerDependency<InputSubsystem>(inputSystem);
        container->registerDependency<IInputSystem>(inputSystem);
        
        return true;
    }
};
