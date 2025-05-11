#include "UISubsystem.hpp"
#include "adapters/primary/ui/ViewManager.hpp"
#include "MockDisplay.hpp"

UISubsystem::UISubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container), initialized_(false) {
}

void UISubsystem::init(bool enableFullUI) {
    if (initialized_) {
        return;
    }
    
    fullUIEnabled_ = enableFullUI;
    
    // Récupérer la configuration
    configuration_ = container_->resolve<IConfiguration>();
    if (!configuration_) {
        // Si aucune configuration n'est trouvée, on ne peut pas initialiser correctement
        return;
    }
    
    // Pour l'instant, utiliser toujours un MockDisplay pour les tests
    display_ = container_->resolve<IDisplay>();
    if (!display_) {
        // Si aucun display n'est disponible, en créer un pour les tests
        display_ = std::make_shared<MockDisplay>();
        // Enregistrer ce display dans le conteneur
        container_->registerImplementation<IDisplay, MockDisplay>(std::static_pointer_cast<MockDisplay>(display_));
    }
    
    // Le ViewManager est une classe abstraite et ne peut pas être instancié directement
    // TODO: Implémenter une classe dérivée concrète de ViewManager
    if (fullUIEnabled_) {
        // viewManager_ = std::make_shared<ConcreteViewManager>(*display_);
        // Pas de gestionnaire de vues pour l'instant
    }
    
    // Enregistrer les dépendances dans le conteneur
    container_->registerImplementation<IDisplay, IDisplay>(display_);
    
    // Enregistrer ce sous-système comme implémentation de IUISystem
    container_->registerImplementation<IUISystem, UISubsystem>(
        std::shared_ptr<UISubsystem>(this, [](UISubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        })
    );
    
    initialized_ = true;
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

void UISubsystem::showMessage(const std::string& message) {
    if (!initialized_ || !display_) {
        std::cout << "UISubsystem: Not initialized or display is null" << std::endl;
        return;
    }
    
    std::cout << "UISubsystem: Showing message: '" << message << "'" << std::endl;
    
    // Afficher directement le message en utilisant les méthodes de DisplayPort
    if (!fullUIEnabled_) {
        display_->clear();
        display_->drawText(0, 0, message.c_str());
        display_->update();
        return;
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
        std::cout << "UISubsystem: No view manager, displaying directly in full UI mode" << std::endl;
        display_->clear();
        display_->drawText(0, 0, message.c_str());
        display_->update();
    }
}

void UISubsystem::clearDisplay() {
    if (!initialized_ || !display_) {
        return;
    }
    
    display_->clear();
    display_->update();
}