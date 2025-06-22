#include "UISubsystem.hpp"

#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "core/TaskScheduler.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "config/unified/UnifiedConfiguration.hpp"

UISubsystem::UISubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {}

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

    // Créer le gestionnaire de vues si l'UI complète est activée
    if (fullUIEnabled_) {
        // Récupérer les dépendances LVGL
        auto lvglBridge = container_->resolve<Ili9341LvglBridge>();
        auto unifiedConfig = container_->resolve<UnifiedConfiguration>();
        
        if (!lvglBridge || !unifiedConfig) {
            return Result<bool, std::string>::error("Missing LVGL dependencies");
        }
        
        viewManager_ = std::make_shared<DefaultViewManager>(lvglBridge, unifiedConfig);

        if (!viewManager_->init()) {
            return Result<bool, std::string>::error("Failed to initialize ViewManager");
        }

        // Créer l'écouteur d'événements UI et l'abonner aux événements
        eventListener_ = std::make_unique<ViewManagerEventListener>(*viewManager_);
        eventListener_->subscribe();

        // Enregistrer le ViewManager dans le conteneur
        container_->registerImplementation<ViewManager, DefaultViewManager>(
            std::static_pointer_cast<DefaultViewManager>(viewManager_));
    }

    initialized_ = true;
    return Result<bool, std::string>::success(true);
}

void UISubsystem::update() {
    if (!initialized_ || !fullUIEnabled_ || !viewManager_) {
        return;
    }

    // Mettre à jour le gestionnaire de vues
    viewManager_->update();
}

Result<bool, std::string> UISubsystem::showMessage(const std::string& message) {
    if (!initialized_ || !fullUIEnabled_ || !viewManager_) {
        return Result<bool, std::string>::error("UI not initialized or disabled");
    }

    // Utiliser le système modal LVGL
    viewManager_->showModal(message.c_str());
    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> UISubsystem::clearDisplay() {
    if (!initialized_ || !fullUIEnabled_ || !viewManager_) {
        return Result<bool, std::string>::error("UI not initialized or disabled");
    }

    // Cacher le modal et retourner à la vue principale
    viewManager_->hideModal();
    return Result<bool, std::string>::success(true);
}