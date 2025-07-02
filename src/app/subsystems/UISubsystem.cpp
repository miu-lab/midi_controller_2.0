#include "UISubsystem.hpp"

#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "config/PerformanceConfig.hpp"

#include "config/unified/UnifiedConfiguration.hpp"
#include "core/TaskScheduler.hpp"
#include "core/utils/Error.hpp"

UISubsystem::UISubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {}

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

    // Initialiser et démarrer l'EventBatcher
    EventBatcher::BatchConfig batchConfig;
    batchConfig.ui_update_interval_ms = PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS *
                                        PerformanceConfig::VSYNC_SPACING;  // 60 FPS max
    batchConfig.coalesce_identical_values = true;

    m_eventBatcher = std::make_unique<EventBatcher>(batchConfig);
    m_eventBatcher->start();

    // Récupérer le bridge LVGL
    m_lvglBridge = container_->resolve<Ili9341LvglBridge>();
    if (!m_lvglBridge) {
        // DEBUG MSG TO IMPLEMENT
    }

    // Créer le gestionnaire de vues si l'UI complète est activée
    if (fullUIEnabled_) {
        // Récupérer les dépendances LVGL
        auto lvglBridge = container_->resolve<Ili9341LvglBridge>();
        auto unifiedConfig = container_->resolve<UnifiedConfiguration>();
        
        if (!lvglBridge || !unifiedConfig) {
            return Result<bool>::error({ErrorCode::DependencyMissing, "Missing LVGL dependencies"});
        }
        
        viewManager_ = std::make_shared<DefaultViewManager>(lvglBridge, unifiedConfig);

        if (!viewManager_->init()) {
            return Result<bool>::error({ErrorCode::InitializationFailed, "Failed to initialize ViewManager"});
        }

        // Créer l'écouteur d'événements UI et l'abonner aux événements
        eventListener_ = std::make_unique<ViewManagerEventListener>(*viewManager_);
        eventListener_->subscribe();

        // Enregistrer le ViewManager dans le conteneur
        container_->registerImplementation<ViewManager, DefaultViewManager>(
            std::static_pointer_cast<DefaultViewManager>(viewManager_));
    }

    initialized_ = true;
    return Result<bool>::success(true);
}

void UISubsystem::update() {
    if (!initialized_ || !fullUIEnabled_ || !viewManager_) {
        return;
    }

    // Traiter les batchs d'événements UI
    if (m_eventBatcher) {
        m_eventBatcher->processPendingBatches();
    }

    // Mettre à jour le gestionnaire de vues
    viewManager_->update();

    // Rafraîchir l'affichage LVGL avec limitation de fréquence
    static unsigned long last_display_refresh = 0;
    constexpr unsigned long DISPLAY_REFRESH_INTERVAL_MS =
        PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS *
        PerformanceConfig::VSYNC_SPACING;  // 60 FPS max

    if (m_lvglBridge && (millis() - last_display_refresh) >= DISPLAY_REFRESH_INTERVAL_MS) {
        m_lvglBridge->refreshDisplay();
        last_display_refresh = millis();
    }
}

Result<bool> UISubsystem::showMessage(const std::string& message) {
    if (!initialized_ || !fullUIEnabled_ || !viewManager_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "UI not initialized or disabled"});
    }

    // Utiliser le système modal LVGL
    viewManager_->showModal(message.c_str());
    return Result<bool>::success(true);
}

Result<bool> UISubsystem::clearDisplay() {
    if (!initialized_ || !fullUIEnabled_ || !viewManager_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "UI not initialized or disabled"});
    }

    // Cacher le modal et retourner à la vue principale
    viewManager_->hideModal();
    return Result<bool>::success(true);
}