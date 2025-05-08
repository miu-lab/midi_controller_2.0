#include "core/listeners/UIEventListener.hpp"
#include "core/controllers/UIController.hpp"
#include "config/debug/DebugConfig.hpp"

UIEventListener::UIEventListener(UIController& uiController)
    : uiController_(uiController), subscriptionId_(0) {}

void UIEventListener::initialize() {
    // S'abonner au bus d'événements
    subscriptionId_ = EventBus::getInstance().subscribe(this);
    
#if defined(DEBUG) && defined(DEBUG_LISTENERS)
    Serial.print(F("UIEventListener: Initialized with subscription ID "));
    Serial.println(subscriptionId_);
#endif
}

void UIEventListener::cleanup() {
    // Se désabonner du bus d'événements
    if (subscriptionId_ != 0) {
        EventBus::getInstance().unsubscribe(subscriptionId_);
        subscriptionId_ = 0;
    }
    
#if defined(DEBUG) && defined(DEBUG_LISTENERS)
    Serial.println(F("UIEventListener: Cleaned up"));
#endif
}

bool UIEventListener::onEvent(const Event& event) {
    // Vérifier le type d'événement et dispatcher vers le gestionnaire approprié
    switch (event.getType()) {
        case EventTypes::EncoderTurned:
            return handleEncoderTurn(static_cast<const EncoderTurnedEvent&>(event));
            
        case EventTypes::EncoderButton:
            return handleEncoderButton(static_cast<const EncoderButtonEvent&>(event));
            
        case EventTypes::ButtonPressed:
            return handleButtonPressed(static_cast<const ButtonPressedEvent&>(event));
            
        case EventTypes::ButtonReleased:
            return handleButtonReleased(static_cast<const ButtonReleasedEvent&>(event));
            
        default:
            // Ignorer les autres types d'événements
            return false;
    }
}

bool UIEventListener::handleEncoderTurn(const EncoderTurnedEvent& event) {
    // Ne traiter que l'encodeur de navigation
    if (event.id == NAVIGATION_ENCODER) {
        uiController_.handleEncoderTurn(event.delta);
        return true;
    }
    return false;
}

bool UIEventListener::handleEncoderButton(const EncoderButtonEvent& event) {
    // Ne traiter que l'encodeur de navigation lors de l'appui
    if (event.id == NAVIGATION_ENCODER && event.pressed) {
        uiController_.handleEncoderClick();
        return true;
    }
    return false;
}

bool UIEventListener::handleButtonPressed(const ButtonPressedEvent& event) {
    // Gérer les boutons de navigation (OK et Back)
    if (event.id == BACK_BUTTON) {
        uiController_.handleBackButton();
        return true;
    } else if (event.id == OK_BUTTON) {
        uiController_.handleOkButton();
        return true;
    }
    return false;
}

bool UIEventListener::handleButtonReleased(const ButtonReleasedEvent& event) {
    // Pour l'instant, on ne fait rien lors du relâchement
    return false;
}
