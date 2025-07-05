#include "ViewFactory.hpp"

#include "app/di/DependencyContainer.hpp"
#include "adapters/ui/views/DefaultViewManager.hpp"
#include "adapters/ui/views/ViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/utils/Error.hpp"

ViewFactory::ViewFactory(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
}

Result<std::shared_ptr<ViewManager>> ViewFactory::createViewManager(const ViewManagerConfig& config) {
    if (!config.enableFullUI) {
        return Result<std::shared_ptr<ViewManager>>::error(
            Error(ErrorCode::ConfigurationError, "Cannot create ViewManager with Full UI disabled")
        );
    }

    // Créer le DefaultViewManager
    auto defaultViewManagerResult = createDefaultViewManager();
    if (!defaultViewManagerResult.isSuccess()) {
        return Result<std::shared_ptr<ViewManager>>::error(defaultViewManagerResult.error().value());
    }

    auto defaultViewManager = defaultViewManagerResult.value().value();
    auto viewManager = std::static_pointer_cast<ViewManager>(defaultViewManager);

    // Initialiser le ViewManager
    if (!viewManager->init()) {
        return Result<std::shared_ptr<ViewManager>>::error(
            Error(ErrorCode::InitializationFailed, "Failed to initialize ViewManager")
        );
    }

    // Enregistrer dans le conteneur si demandé
    if (config.registerInContainer && container_) {
        container_->registerImplementation<ViewManager, DefaultViewManager>(defaultViewManager);
    }

    return Result<std::shared_ptr<ViewManager>>::success(viewManager);
}

Result<std::shared_ptr<DefaultViewManager>> ViewFactory::createDefaultViewManager() {
    if (!container_) {
        return Result<std::shared_ptr<DefaultViewManager>>::error(
            Error(ErrorCode::DependencyMissing, "DependencyContainer not available")
        );
    }

    // Résoudre les dépendances LVGL
    std::shared_ptr<Ili9341LvglBridge> lvglBridge;
    std::shared_ptr<UnifiedConfiguration> unifiedConfig;
    std::shared_ptr<EventBus> eventBus;

    if (!resolveLvglDependencies(lvglBridge, unifiedConfig, eventBus)) {
        return Result<std::shared_ptr<DefaultViewManager>>::error(
            Error(ErrorCode::DependencyMissing, "Missing required LVGL dependencies")
        );
    }

    // Créer le DefaultViewManager
    auto defaultViewManager = std::make_shared<DefaultViewManager>(
        lvglBridge, 
        unifiedConfig, 
        eventBus
    );

    return Result<std::shared_ptr<DefaultViewManager>>::success(defaultViewManager);
}

bool ViewFactory::validateDependencies() const {
    if (!container_) {
        return false;
    }

    std::shared_ptr<Ili9341LvglBridge> lvglBridge;
    std::shared_ptr<UnifiedConfiguration> unifiedConfig;
    std::shared_ptr<EventBus> eventBus;

    return resolveLvglDependencies(lvglBridge, unifiedConfig, eventBus);
}

bool ViewFactory::resolveLvglDependencies(
    std::shared_ptr<Ili9341LvglBridge>& lvglBridge,
    std::shared_ptr<UnifiedConfiguration>& unifiedConfig,
    std::shared_ptr<EventBus>& eventBus) const {
    
    if (!container_) {
        return false;
    }

    // Résoudre le bridge LVGL
    lvglBridge = container_->resolve<Ili9341LvglBridge>();
    if (!lvglBridge) {
        return false;
    }

    // Résoudre la configuration unifiée
    unifiedConfig = container_->resolve<UnifiedConfiguration>();
    if (!unifiedConfig) {
        return false;
    }

    // Résoudre le bus d'événements
    eventBus = container_->resolve<EventBus>();
    if (!eventBus) {
        return false;
    }

    return true;
}