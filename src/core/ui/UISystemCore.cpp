#include "UISystemCore.hpp"

#include "adapters/ui/views/ViewManager.hpp"
#include "adapters/ui/views/ViewManagerEventListener.hpp"
#include "core/ui/DisplayManager.hpp"
#include "core/domain/events/core/IEventBus.hpp"
#include "core/utils/Error.hpp"

UISystemCore::UISystemCore(const CoreConfig& config)
    : config_(config)
    , initialized_(false) {
}

Result<bool> UISystemCore::initialize(
    std::shared_ptr<ViewManager> viewManager,
    std::unique_ptr<DisplayManager> displayManager,
    std::shared_ptr<MidiController::Events::IEventBus> eventBus) {
    
    if (initialized_) {
        return Result<bool>::success(true);
    }

    // Stocker les composants
    viewManager_ = viewManager;
    displayManager_ = std::move(displayManager);
    eventBus_ = eventBus;

    // Valider les composants selon la configuration
    if (!validateComponents()) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, "Required UI components missing"}
        );
    }

    initialized_ = true;
    return Result<bool>::success(true);
}

void UISystemCore::update() {
    if (!isOperational()) {
        return;
    }

    // Mettre à jour dans le bon ordre pour optimiser les performances
    processEvents();
    updateViewManager();
    refreshDisplay();
}

Result<bool> UISystemCore::showMessage(const std::string& message) {
    if (!isOperational()) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, "UI system not operational"}
        );
    }

    if (!viewManager_) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, "ViewManager not available"}
        );
    }

    // Utiliser le système modal LVGL
    viewManager_->showModal(message.c_str());
    return Result<bool>::success(true);
}

Result<bool> UISystemCore::clearDisplay() {
    if (!isOperational()) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, "UI system not operational"}
        );
    }

    if (!viewManager_) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, "ViewManager not available"}
        );
    }

    // Cacher le modal et retourner à la vue principale
    viewManager_->hideModal();
    return Result<bool>::success(true);
}

bool UISystemCore::isInitialized() const {
    return initialized_;
}

bool UISystemCore::isOperational() const {
    return initialized_ && 
           config_.enableFullUI && 
           viewManager_ != nullptr;
}

Result<bool> UISystemCore::configureEventListener(std::unique_ptr<ViewManagerEventListener> eventListener) {
    if (!viewManager_) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, "ViewManager required for event listener"}
        );
    }

    eventListener_ = std::move(eventListener);
    
    // Abonner l'écouteur aux événements
    if (eventListener_) {
        eventListener_->subscribe();
    }

    return Result<bool>::success(true);
}

std::shared_ptr<ViewManager> UISystemCore::getViewManager() const {
    return viewManager_;
}

bool UISystemCore::validateComponents() const {
    // ViewManager est toujours requis si Full UI est activé
    if (config_.enableFullUI && !viewManager_) {
        return false;
    }

    // DisplayManager est requis si le refresh est activé
    if (config_.enableDisplayRefresh && config_.enableFullUI && !displayManager_) {
        return false;
    }

    // EventManager est requis si le traitement d'événements est activé
    if (config_.enableEventProcessing && config_.enableFullUI && !eventBus_) {
        return false;
    }

    return true;
}

void UISystemCore::processEvents() {
    if (config_.enableEventProcessing && eventBus_) {
        eventBus_->update();
    }
}

void UISystemCore::updateViewManager() {
    if (viewManager_) {
        viewManager_->update();
    }
}

void UISystemCore::refreshDisplay() {
    if (config_.enableDisplayRefresh && displayManager_) {
        displayManager_->update();
    }
}