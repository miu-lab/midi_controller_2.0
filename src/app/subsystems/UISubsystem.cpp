#include "UISubsystem.hpp"

#include "adapters/ui/views/DefaultViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "config/PerformanceConfig.hpp"

#include "config/unified/UnifiedConfiguration.hpp"
#include "core/TaskScheduler.hpp"
#include "core/utils/Error.hpp"
#include "core/domain/events/core/EventBus.hpp"

UISubsystem::UISubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    // Créer la ViewFactory et UISystemAdapter
    if (container_) {
        viewFactory_ = std::make_shared<ViewFactory>(container_);
        
        IUIManager::UIConfig uiConfig;
        uiConfig.enableFullUI = false; // Sera activé lors de l'initialisation
        uiAdapter_ = std::make_shared<UISystemAdapter>(uiConfig);
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

    // Initialiser UISystemAdapter si l'UI complète est activée
    if (fullUIEnabled_) {
        if (!viewFactory_ || !uiAdapter_) {
            return Result<bool>::error({ErrorCode::DependencyMissing, "ViewFactory or UISystemAdapter not available"});
        }

        // Mettre à jour la configuration pour activer Full UI
        IUIManager::UIConfig uiConfig;
        uiConfig.enableFullUI = true;
        uiConfig.enableEventProcessing = true;
        uiConfig.enableDisplayRefresh = true;
        uiAdapter_ = std::make_shared<UISystemAdapter>(uiConfig);

        // Créer les composants via ViewFactory avec Full UI activé
        IViewFactory::ViewManagerConfig viewManagerConfig;
        viewManagerConfig.enableFullUI = true;
        viewManagerConfig.enableEventListener = true;
        viewManagerConfig.registerInContainer = true;
        
        auto viewManagerResult = viewFactory_->createViewManager(viewManagerConfig);
        if (!viewManagerResult.isSuccess()) {
            return Result<bool>::error(viewManagerResult.error().value());
        }

        // Récupérer EventBus unifié depuis le container (remplace EventManager)
        auto eventBus = container_->resolve<MidiController::Events::IEventBus>();
        if (!eventBus) {
            return Result<bool>::error({ErrorCode::DependencyMissing, "Failed to resolve IEventBus"});
        }
        
        // Initialiser et démarrer EventBus si nécessaire
        if (!eventBus->isStarted()) {
            eventBus->initialize();
            eventBus->start();
        }

        // Créer DisplayManagerAdapter
        std::unique_ptr<DisplayManagerAdapter> displayManager = nullptr;
        if (m_lvglBridge) {
            displayManager = std::make_unique<DisplayManagerAdapter>(m_lvglBridge);
        }

        // Initialiser UISystemAdapter avec tous les composants
        auto initResult = uiAdapter_->initializeWithComponents(
            viewManagerResult.value().value(),
            std::move(displayManager),
            eventBus
        );
        
        if (!initResult.isSuccess()) {
            return Result<bool>::error(initResult.error().value());
        }

        // Configurer l'écouteur d'événements
        if (uiAdapter_->getViewManager()) {
            // Récupérer EventBus pour ViewManagerEventListener
            auto eventBus = container_->resolve<MidiController::Events::IEventBus>();
            if (!eventBus) {
                return Result<bool>::error({ErrorCode::DependencyMissing, "Failed to resolve IEventBus"});
            }
            
            auto eventListener = std::make_unique<ViewManagerEventListener>(*uiAdapter_->getViewManager(), eventBus);
            auto listenerResult = uiAdapter_->configureEventListener(std::move(eventListener));
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

    // Déléguer la mise à jour à UISystemAdapter
    if (uiAdapter_) {
        uiAdapter_->update();
    }
}

Result<bool> UISubsystem::showMessage(const std::string& message) {
    if (!initialized_ || !uiAdapter_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "UI not initialized"});
    }

    // Déléguer à UISystemAdapter
    return uiAdapter_->showMessage(message);
}

Result<bool> UISubsystem::clearDisplay() {
    if (!initialized_ || !uiAdapter_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "UI not initialized"});
    }

    // Déléguer à UISystemAdapter
    return uiAdapter_->clearDisplay();
}