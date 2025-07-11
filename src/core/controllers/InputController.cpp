#include "InputController.hpp"

InputController::InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                                std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                                std::shared_ptr<EventBus> eventBus)
    : processorManager_(std::make_unique<InputProcessorManager>(navigationConfig, unifiedConfig, eventBus)) {
}

void InputController::processEncoderTurn(EncoderId id, int32_t absolutePosition,
                                         int8_t relativeChange) {
    if (processorManager_) {
        processorManager_->processEncoderTurn(id, absolutePosition, relativeChange);
    }
}

void InputController::processButtonPress(ButtonId id, bool pressed) {
    if (processorManager_) {
        processorManager_->processButtonPress(id, pressed);
    }
}

