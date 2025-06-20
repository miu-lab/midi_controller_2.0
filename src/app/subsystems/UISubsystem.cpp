#include "UISubsystem.hpp"

#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "adapters/primary/ui/DisplayEventListener.hpp"
#include "adapters/primary/ui/ViewManagerEventListener.hpp"
#include "adapters/secondary/hardware/output/display/Ssd1306Display.hpp"
#include "config/debug/TaskSchedulerConfig.hpp"
#include "core/tasks/DisplayUpdateTask.hpp"
#include "core/utils/AppStrings.hpp"
#include "core/utils/FlashStrings.hpp"

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

            if (!display_) {
                return Result<bool, std::string>::error("Failed to create display");
            }

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
        char buffer[80];
        char temp[64];

        // Construire "UISubsystem: Creating display task..."
        FlashStrings::copy(temp, sizeof(temp), PFX_UI);
        strcpy(buffer, temp);
        FlashStrings::copy(temp, sizeof(temp), MSG_CREATING_TASK);
        strcat(buffer, temp);
        Serial.println(buffer);
        displayTask_ = std::make_shared<DisplayUpdateTask>(
            display_,
            TaskTiming::UI_MIN_PERIOD_MS);  // Utilise la constante globale pour le taux de
                                            // rafraîchissement
        int taskIndex = scheduler.addTask(displayTask_->getTaskFunction(),
                                          displayTask_->getIntervalMicros(),
                                          displayTask_->getPriority(),
                                          displayTask_->getName());
        // Afficher "UISubsystem: Display task added with index X"
        FlashStrings::copy(temp, sizeof(temp), PFX_UI);
        strcpy(buffer, temp);
        FlashStrings::copy(temp, sizeof(temp), MSG_TASK_ADDED);
        strcat(buffer, temp);
        Serial.print(buffer);
        Serial.println(taskIndex);

        // Forcer une mise à jour immédiate pour vérifier que l'écran fonctionne
        displayTask_->requestUpdate();
        displayTask_->execute();  // Exécution directe pour tester

        // Créer et enregistrer l'écouteur d'événements d'affichage
        displayEventListener_ = std::make_unique<DisplayEventListener>(displayTask_);
        displayEventListener_->subscribe();

        // Créer l'écouteur d'événements UI et l'abonner aux événements
        // Note: UISubsystem est le seul endroit où ViewManagerEventListener doit être instancié
        // pour éviter les abonnements multiples aux événements UI et MIDI
        eventListener_ = std::make_unique<ViewManagerEventListener>(*viewManager_);
        eventListener_->subscribe();

        // Log d'information sur l'abonnement
        Serial.print(F("\nUI Event Listener subscription status: "));
        Serial.println(EventBus::getInstance().exists(eventListener_->getSubscriptionId())
                           ? F("SUCCESS")
                           : F("FAILED"));
        Serial.print(F("Total event bus subscribers: "));
        Serial.println(EventBus::getInstance().getCount());
        // Afficher "UISubsystem: Created and subscribed ViewManagerEventListener"
        FlashStrings::copy(temp, sizeof(temp), PFX_UI);
        strcpy(buffer, temp);
        FlashStrings::copy(temp, sizeof(temp), MSG_VIEW_MANAGER);
        strcat(buffer, temp);
        Serial.println(buffer);

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

        // Vérifier si le ViewManager a demandé une mise à jour de l'affichage
        if (viewManager_->needsDisplayUpdate()) {
            displayTask_->requestUpdate();
            viewManager_->clearDisplayUpdateFlag();
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