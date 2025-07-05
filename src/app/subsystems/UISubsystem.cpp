#include "UISubsystem.hpp"

#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "config/PerformanceConfig.hpp"

#include "config/unified/UnifiedConfiguration.hpp"
#include "core/TaskScheduler.hpp"
#include "core/utils/Error.hpp"
#include "core/domain/events/core/EventBus.hpp"

UISubsystem::UISubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    // Créer la ViewFactory et UISystemCore
    if (container_) {
        viewFactory_ = std::make_unique<ViewFactory>(container_);
        
        UISystemCore::CoreConfig coreConfig;
        coreConfig.enableFullUI = false; // Sera activé lors de l'initialisation
        uiCore_ = std::make_unique<UISystemCore>(coreConfig);
    }
}

Result<bool> UISubsystem::init(bool enableFullUI) {
    if (initialized_) {
        return Result<bool>::success(true);
    }

    fullUIEnabled_ = enableFullUI;

    // Récupérer la configuration
    configuration_ = container_->resolve<IConfiguration>();
    if (!configuration_) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Failed to resolve IConfiguration"});
    }

    // Récupérer le bridge LVGL
    m_lvglBridge = container_->resolve<Ili9341LvglBridge>();
    if (!m_lvglBridge) {
        // TODO DEBUG MSG
    }

    // Initialiser UISystemCore si l'UI complète est activée
    if (fullUIEnabled_) {
        if (!viewFactory_ || !uiCore_) {
            return Result<bool>::error({ErrorCode::DependencyMissing, "ViewFactory or UISystemCore not available"});
        }

        // Mettre à jour la configuration du core pour activer Full UI
        UISystemCore::CoreConfig coreConfig;
        coreConfig.enableFullUI = true;
        coreConfig.enableEventProcessing = true;
        coreConfig.enableDisplayRefresh = true;
        uiCore_ = std::make_unique<UISystemCore>(coreConfig);

        // Créer les composants via ViewFactory avec Full UI activé
        ViewFactory::ViewManagerConfig viewManagerConfig;
        viewManagerConfig.enableFullUI = true;
        viewManagerConfig.enableEventListener = true;
        viewManagerConfig.registerInContainer = true;
        
        auto viewManagerResult = viewFactory_->createViewManager(viewManagerConfig);
        if (!viewManagerResult.isSuccess()) {
            return Result<bool>::error(viewManagerResult.error().value());
        }

        // Créer EventManager (remplace EventBatcher)
        EventManager::Config managerConfig;
        managerConfig.uiUpdateIntervalMs = PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS *
                                          PerformanceConfig::VSYNC_SPACING;
        managerConfig.coalesceIdenticalValues = true;
        managerConfig.enableBatching = true;
        auto eventManager = std::make_unique<EventManager>(managerConfig);
        eventManager->initialize();
        eventManager->start();

        // Créer DisplayManager
        std::unique_ptr<DisplayManager> displayManager = nullptr;
        if (m_lvglBridge) {
            displayManager = std::make_unique<DisplayManager>(m_lvglBridge);
        }

        // Initialiser UISystemCore avec tous les composants
        auto initResult = uiCore_->initialize(
            viewManagerResult.value().value(),
            std::move(displayManager),
            std::move(eventManager)
        );
        
        if (!initResult.isSuccess()) {
            return Result<bool>::error(initResult.error().value());
        }

        // Configurer l'écouteur d'événements
        if (uiCore_->getViewManager()) {
            auto eventListener = std::make_unique<ViewManagerEventListener>(*uiCore_->getViewManager());
            auto listenerResult = uiCore_->configureEventListener(std::move(eventListener));
            if (!listenerResult.isSuccess()) {
                return Result<bool>::error(listenerResult.error().value());
            }
        }
    }

    initialized_ = true;
    return Result<bool>::success(true);
}

void UISubsystem::update() {
    if (!initialized_) {
        return;
    }

    // Déléguer la mise à jour à UISystemCore
    if (uiCore_) {
        uiCore_->update();
    }
}

Result<bool> UISubsystem::showMessage(const std::string& message) {
    if (!initialized_ || !uiCore_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "UI not initialized"});
    }

    // Déléguer à UISystemCore
    return uiCore_->showMessage(message);
}

Result<bool> UISubsystem::clearDisplay() {
    if (!initialized_ || !uiCore_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "UI not initialized"});
    }

    // Déléguer à UISystemCore
    return uiCore_->clearDisplay();
}