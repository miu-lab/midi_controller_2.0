#include "UISubsystem.hpp"

#include "core/TaskScheduler.hpp"
#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "adapters/primary/ui/UIEventListener.hpp"
#include "adapters/primary/ui/DisplayEventListener.hpp"
#include "adapters/secondary/hardware/display/Ssd1306Display.hpp"
#include "app/mocks/DisplayMock.hpp"
#include "core/tasks/DisplayUpdateTask.hpp"

UISubsystem::UISubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container), initialized_(false) {}

Result<bool, std::string> UISubsystem::init(bool enableFullUI) {
    if (initialized_) {
        return Result<bool, std::string>::success(true);
    }

    fullUIEnabled_ = enableFullUI;

    // Récupérer la configuration
    configuration_ = container_->resolve<IConfiguration>();
    if (!configuration_) {
        return Result<bool, std::string>::error("Failed to resolve IConfiguration");
    }

    // Essayer de récupérer un displayPort existant
    display_ = container_->resolve<DisplayPort>();
    
    if (!display_) {
        // Aucun DisplayPort disponible, créer un SSD1306Display
        auto ssd1306Display = std::make_shared<Ssd1306Display>();
        
        // Initialiser l'écran SSD1306
        if (!ssd1306Display->init()) {
            // Si l'initialisation échoue, créer un MockDisplay pour les tests
            std::cout << "SSD1306 initialization failed, creating MockDisplay instead" << std::endl;
            display_ = std::make_shared<DisplayMock>();
            
            if (!display_) {
                return Result<bool, std::string>::error("Failed to create display");
            }
            
            // Enregistrer le MockDisplay dans le conteneur
            container_->registerImplementation<DisplayPort, DisplayMock>(
                std::static_pointer_cast<DisplayMock>(display_));
        } else {
            // SSD1306 initialisé avec succès
            display_ = ssd1306Display;
            
            // Enregistrer le SSD1306Display dans le conteneur
            container_->registerImplementation<DisplayPort, Ssd1306Display>(
                std::static_pointer_cast<Ssd1306Display>(display_));
        }
    }

    // Créer le gestionnaire de vues si l'UI complète est activée
    if (fullUIEnabled_) {
        viewManager_ = std::make_shared<DefaultViewManager>(display_);
        
        if (!viewManager_->init()) {
            return Result<bool, std::string>::error("Failed to initialize ViewManager");
        }
        
        // Créer la tâche d'affichage et l'écouteur d'événements
        Serial.println(F("UISubsystem: Creating display task..."));
        displayTask_ = std::make_shared<DisplayUpdateTask>(display_, 50); // 50ms = 20fps max
        int taskIndex = scheduler.addTask(displayTask_->getTaskFunction(), 
                                     displayTask_->getIntervalMicros(), 
                                     displayTask_->getPriority(), 
                                     displayTask_->getName());
        Serial.print(F("UISubsystem: Display task added with index "));
        Serial.println(taskIndex);
        
        // Forcer une mise à jour immédiate pour vérifier que l'écran fonctionne
        displayTask_->requestUpdate();
        displayTask_->execute(); // Exécution directe pour tester

        // Créer et enregistrer l'écouteur d'événements d'affichage
        displayEventListener_ = std::make_unique<DisplayEventListener>(displayTask_);
        displayEventListener_->subscribe();
        
        // Créer l'écouteur d'événements UI et l'abonner aux événements
        eventListener_ = std::make_unique<UIEventListener>(*viewManager_);
        eventListener_->subscribe();
        Serial.println(F("UISubsystem: Created and subscribed UIEventListener"));
        
        // Enregistrer le ViewManager dans le conteneur
        container_->registerImplementation<ViewManager, DefaultViewManager>(
            std::static_pointer_cast<DefaultViewManager>(viewManager_));
    }

    // Enregistrer les dépendances dans le conteneur
    container_->registerImplementation<DisplayPort, DisplayPort>(display_);

    // Enregistrer ce sous-système comme implémentation de IUISystem
    container_->registerImplementation<IUISystem, UISubsystem>(
        std::shared_ptr<UISubsystem>(this, [](UISubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        }));

    initialized_ = true;
    return Result<bool, std::string>::success(true);
}

void UISubsystem::update() {
    if (!initialized_) {
        return;
    }

    // Mettre à jour le gestionnaire de vues si l'UI complète est activée
    if (fullUIEnabled_ && viewManager_) {
        // Mettre à jour la logique des vues
        // Le gestionnaire de vues gère maintenant lui-même les rendus uniquement si nécessaire
        viewManager_->update();
        
        // Forcer périodiquement une mise à jour du display pour vérifier que le système fonctionne
        static uint32_t updateCounter = 0;
        if (++updateCounter % 500 == 0 && displayTask_) {
            // Tous les 500 cycles (environ 10 secondes), forcer une mise à jour
            displayTask_->requestUpdate();
        }
    }
}

Result<bool, std::string> UISubsystem::showMessage(const std::string& message) {
    if (!initialized_) {
        return Result<bool, std::string>::error("UISubsystem not initialized");
    }

    if (!display_) {
        return Result<bool, std::string>::error("Display not available");
    }

    std::cout << "UISubsystem: Showing message: '" << message << "'" << std::endl;

    // Afficher directement le message en utilisant les méthodes de DisplayPort
    // Ne plus afficher de boîte de dialogue modale
    display_->clear();
    display_->drawText(0, 0, message.c_str());
    display_->update();

    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> UISubsystem::clearDisplay() {
    if (!initialized_) {
        return Result<bool, std::string>::error("UISubsystem not initialized");
    }

    if (!display_) {
        return Result<bool, std::string>::error("Display not available");
    }

    // Dans un environnement sans exceptions, nous vérifions simplement les erreurs directement
    display_->clear();
    display_->update();
    return Result<bool, std::string>::success(true);
}