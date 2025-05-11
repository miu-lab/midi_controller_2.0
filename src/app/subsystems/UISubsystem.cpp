#include "UISubsystem.hpp"

#include "MockDisplay.hpp"
#include "adapters/primary/ui/ViewManager.hpp"

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

    // Pour l'instant, utiliser toujours un MockDisplay pour les tests
    display_ = container_->resolve<DisplayPort>();
    if (!display_) {
        // Si aucun display n'est disponible, en créer un pour les tests
        display_ = std::make_shared<MockDisplay>();
        if (!display_) {
            return Result<bool, std::string>::error("Failed to create MockDisplay");
        }
        // Enregistrer ce display dans le conteneur
        container_->registerImplementation<DisplayPort, MockDisplay>(
            std::static_pointer_cast<MockDisplay>(display_));
    }

    // Le ViewManager est une classe abstraite et ne peut pas être instancié directement
    // TODO: Implémenter une classe dérivée concrète de ViewManager
    if (fullUIEnabled_) {
        // viewManager_ = std::make_shared<ConcreteViewManager>(*display_);
        // Pas de gestionnaire de vues pour l'instant
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
        // viewManager_->update();
        // Pas de gestionnaire de vues pour l'instant
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
    if (!fullUIEnabled_) {
        display_->clear();
        display_->drawText(0, 0, message.c_str());
        display_->update();
        return Result<bool, std::string>::success(true);
    }

    // Sinon, utiliser le gestionnaire de vues pour afficher le message
    if (viewManager_) {
        // Cette partie dépend de l'implémentation de ViewManager
        // Pour l'instant, on utilise un affichage direct
        display_->clear();
        display_->drawText(0, 0, message.c_str());
        display_->update();
    } else {
        // Si pas de gestionnaire de vues même en mode UI complète,
        // afficher directement le message
        std::cout << "UISubsystem: No view manager, displaying directly in full UI mode"
                  << std::endl;
        display_->clear();
        display_->drawText(0, 0, message.c_str());
        display_->update();
    }

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