#include "InputController.hpp"

InputController::InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                                 std::shared_ptr<OptimizedEventBus> eventBus)
    : m_navigationConfig(navigationConfig), eventBus_(eventBus) {
    }

void InputController::processEncoderTurn(EncoderId id, int32_t absolutePosition,
                                         int8_t relativeChange) {
    // Publier l'événement sur le bus d'événements
    if (eventBus_) {
        HighPriorityEncoderChangedEvent event(id, absolutePosition, relativeChange);
        eventBus_->publishHighPriority(event);
    }
}

void InputController::processEncoderButton(EncoderId id, bool pressed) {
    // Publier l'événement sur le bus d'événements
    if (eventBus_) {
        HighPriorityEncoderButtonEvent event(id, pressed);
        eventBus_->publishHighPriority(event);
    }
}

void InputController::processButtonPress(ButtonId id, bool pressed) {
    // Publier l'événement sur le bus d'événements
    if (eventBus_) {
        HighPriorityButtonPressEvent event(id, pressed);
        eventBus_->publishHighPriority(event);
    }
}