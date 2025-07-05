#include "InputController.hpp"



InputController::InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                                 std::shared_ptr<EventBus> eventBus)
    : m_navigationConfig(navigationConfig), eventBus_(eventBus) {
}

void InputController::processEncoderTurn(EncoderId id, int32_t absolutePosition,
                                         int8_t relativeChange) {
    // Vérifier si c'est un contrôle de navigation
    if (m_navigationConfig && m_navigationConfig->isNavigationControl(id)) {
        // Les événements de navigation sont traités directement via les listeners UI
        // ne pas les publier sur le bus pour éviter qu'ils atteignent MidiMapper
        return;
    }
    
    // Publier l'événement sur le bus d'événements uniquement pour les contrôles MIDI
    if (eventBus_) {
        HighPriorityEncoderChangedEvent event(id, absolutePosition, relativeChange);
        eventBus_->publish(event);
    }
}


void InputController::processButtonPress(ButtonId id, bool pressed) {
    // Vérifier si c'est un contrôle de navigation
    if (m_navigationConfig && m_navigationConfig->isNavigationControl(id)) {
        // Les événements de navigation sont traités directement via les listeners UI
        // ne pas les publier sur le bus pour éviter qu'ils atteignent MidiMapper
        return;
    }
    
    // Publier l'événement sur le bus d'événements uniquement pour les contrôles MIDI
    if (eventBus_) {
        HighPriorityButtonPressEvent event(id, pressed);
        eventBus_->publish(event);
    }
}