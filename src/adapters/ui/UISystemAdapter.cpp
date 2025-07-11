#include "UISystemAdapter.hpp"

#include "adapters/ui/views/ViewManager.hpp"
#include "adapters/ui/views/ViewManagerEventListener.hpp"
#include "core/domain/interfaces/IDisplayManager.hpp"
#include "core/domain/events/core/IEventBus.hpp"
#include "core/utils/Error.hpp"
#include "config/SystemConstants.hpp"

UISystemAdapter::UISystemAdapter(const UIConfig& config)
    : config_(config)
    , initialized_(false) {
}

Result<bool> UISystemAdapter::initialize(std::shared_ptr<MidiController::Events::IEventBus> eventBus) {
    if (initialized_) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, SystemConstants::ErrorMessages::ALREADY_INITIALIZED}
        );
    }

    if (!isConfigurationValid()) {
        return Result<bool>::error(
            {ErrorCode::ConfigurationError, "Invalid UI system configuration"}
        );
    }

    // Créer un processor manager minimal avec seulement EventBus
    if (eventBus) {
        processorManager_ = std::make_unique<UIProcessorManager>(nullptr, nullptr, eventBus);
    }

    initialized_ = true;
    return Result<bool>::success(true);
}

Result<bool> UISystemAdapter::initializeWithComponents(
    std::shared_ptr<ViewManager> viewManager,
    std::unique_ptr<IDisplayManager> displayManager,
    std::shared_ptr<MidiController::Events::IEventBus> eventBus) {
    
    if (initialized_) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, SystemConstants::ErrorMessages::ALREADY_INITIALIZED}
        );
    }

    if (!isConfigurationValid()) {
        return Result<bool>::error(
            {ErrorCode::ConfigurationError, "Invalid UI system configuration"}
        );
    }

    // Créer le processor manager avec tous les composants
    processorManager_ = std::make_unique<UIProcessorManager>(
        viewManager, std::move(displayManager), eventBus
    );

    // Valider les composants selon la configuration
    if (!validateComponents()) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, SystemConstants::ErrorMessages::COMPONENTS_MISSING}
        );
    }

    initialized_ = true;
    return Result<bool>::success(true);
}

void UISystemAdapter::update() {
    if (!isOperational()) {
        return;
    }

    // Déléguer au processor manager qui gère l'ordre optimal
    if (processorManager_) {
        processorManager_->executeUpdateCycle();
    }
}

Result<bool> UISystemAdapter::showMessage(const std::string& message) {
    if (!isOperational()) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, SystemConstants::ErrorMessages::SYSTEM_NOT_OPERATIONAL}
        );
    }

    if (!processorManager_) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, SystemConstants::ErrorMessages::NULL_VIEW_MANAGER}
        );
    }

    // Déléguer au processor manager
    bool success = processorManager_->showMessage(message);
    if (!success) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, "Failed to show modal message"}
        );
    }

    return Result<bool>::success(true);
}

Result<bool> UISystemAdapter::clearDisplay() {
    if (!isOperational()) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, SystemConstants::ErrorMessages::SYSTEM_NOT_OPERATIONAL}
        );
    }

    if (!processorManager_) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, SystemConstants::ErrorMessages::NULL_VIEW_MANAGER}
        );
    }

    // Déléguer au processor manager
    bool success = processorManager_->clearDisplay();
    if (!success) {
        return Result<bool>::error(
            {ErrorCode::OperationFailed, "Failed to clear display"}
        );
    }

    return Result<bool>::success(true);
}

bool UISystemAdapter::isInitialized() const {
    return initialized_;
}

bool UISystemAdapter::isOperational() const {
    return initialized_ && 
           config_.enableFullUI && 
           processorManager_ && 
           processorManager_->areProcessorsOperational();
}

Result<bool> UISystemAdapter::configureEventListener(std::unique_ptr<ViewManagerEventListener> eventListener) {
    if (!processorManager_ || !processorManager_->getViewManager()) {
        return Result<bool>::error(
            {ErrorCode::DependencyMissing, SystemConstants::ErrorMessages::NULL_VIEW_MANAGER}
        );
    }

    eventListener_ = std::move(eventListener);
    
    // Abonner l'écouteur aux événements
    if (eventListener_) {
        eventListener_->subscribe();
    }

    return Result<bool>::success(true);
}

std::shared_ptr<ViewManager> UISystemAdapter::getViewManager() const {
    if (!processorManager_) {
        return nullptr;
    }
    return processorManager_->getViewManager();
}

bool UISystemAdapter::validateComponents() const {
    if (!processorManager_) {
        return false;
    }

    // ViewManager est requis si Full UI est activé
    if (config_.enableFullUI && !processorManager_->getViewManager()) {
        return false;
    }

    // Vérifier que les processors sont opérationnels selon la configuration
    if (config_.enableFullUI && !processorManager_->areProcessorsOperational()) {
        return false;
    }

    return true;
}

bool UISystemAdapter::isConfigurationValid() const {
    // Vérifications de base de la configuration
    if (SystemConstants::Validation::STRICT_COMPONENT_VALIDATION) {
        // Vérifier que la configuration est cohérente
        if (config_.enableDisplayRefresh && !config_.enableFullUI) {
            return false; // Refresh sans UI n'a pas de sens
        }
        
        if (config_.enableEventProcessing && !config_.enableFullUI) {
            return false; // Événements UI sans UI n'a pas de sens
        }
    }
    
    return true;
}