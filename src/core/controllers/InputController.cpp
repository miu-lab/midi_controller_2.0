#include "InputController.hpp"



InputController::InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                                 std::shared_ptr<EventBus> eventBus)
    : m_navigationConfig(navigationConfig), eventBus_(eventBus) {
}

void InputController::processEncoderTurn(EncoderId id, int32_t absolutePosition,
                                         int8_t relativeChange) {
    // Publier l'événement sur le bus d'événements
    if (eventBus_) {
        HighPriorityEncoderChangedEvent event(id, absolutePosition, relativeChange);
        eventBus_->publish(event);
    }
}


void InputController::processButtonPress(ButtonId id, bool pressed) {
    if (eventBus_) {
        HighPriorityButtonPressEvent event(id, pressed);
        eventBus_->publish(event);
    }
}