#include "app/services/UiEventService.hpp"
#include <memory>

void UiEventService::init(NavigationConfigService& navService) {
    // Désabonner l'écouteur existant s'il existe
    if (subscriptionId_ != 0) {
        EventBus::getInstance().unsubscribe(subscriptionId_);
        subscriptionId_ = 0;
    }

    // Stocker la référence au service de navigation
    navService_ = std::shared_ptr<NavigationConfigService>(&navService, [](NavigationConfigService*) {});

    // Configurer les abonnements pour le débogage
    setupDebugSubscriptions();
}

void UiEventService::setupDebugSubscriptions() {
    // Annuler l'abonnement existant s'il y en a un
    if (subscriptionId_ != 0) {
        EventBus::getInstance().unsubscribe(subscriptionId_);
        subscriptionId_ = 0;
    }

    // S'abonner à tous les événements
    subscriptionId_ = EventBus::getInstance().subscribe(this);
}

bool UiEventService::onEvent(const Event& event) {
    // Traiter les événements en fonction de leur type
    switch (event.getType()) {
    case EventTypes::EncoderTurned:
        // printEncoderEvent(static_cast<const EncoderTurnedEvent&>(event));
        break;

    case EventTypes::EncoderButton:
        // printEncoderButtonEvent(static_cast<const EncoderButtonEvent&>(event));
        break;

    case EventTypes::ButtonPressed:
        // printButtonPressedEvent(static_cast<const ButtonPressedEvent&>(event));
        break;

    case EventTypes::ButtonReleased:
        // printButtonReleasedEvent(static_cast<const ButtonReleasedEvent&>(event));
        break;

    default:
        // Événement non géré
        return false;
    }

    // L'événement a été traité (pour l'affichage), mais on ne le marque pas comme entièrement géré
    // pour permettre à d'autres écouteurs de le traiter également
    return false;
}

void UiEventService::printEncoderEvent(const EncoderTurnedEvent& e) {
    // Distinguer les encodeurs de navigation des encodeurs MIDI
    if (navService_ && navService_->isNavigationControl(e.id)) {
        Serial.print("NAV_ENC ");
    } else {
        Serial.print("MIDI_ENC ");
    }
    Serial.print(e.id);
    Serial.print(" abs:");
    Serial.print(e.position);
    Serial.print(" delta:");
    Serial.println(e.delta);
}

void UiEventService::printEncoderButtonEvent(const EncoderButtonEvent& e) {
    // Distinguer les boutons d'encodeur de navigation
    if (navService_ && navService_->isNavigationControl(e.id)) {
        Serial.print("NAV_ENC_BTN ");
    } else {
        Serial.print("MIDI_ENC_BTN ");
    }
    Serial.print(e.id);
    Serial.println(e.pressed ? " PRESSED" : " RELEASED");
}

void UiEventService::printButtonPressedEvent(const ButtonPressedEvent& e) {
    // Distinguer les boutons de navigation
    if (navService_ && navService_->isNavigationControl(e.id)) {
        Serial.print("NAV_BTN ");
    } else {
        Serial.print("MIDI_BTN ");
    }
    Serial.print(e.id);
    Serial.println(" PRESSED");
}

void UiEventService::printButtonReleasedEvent(const ButtonReleasedEvent& e) {
    // Distinguer les boutons de navigation
    if (navService_ && navService_->isNavigationControl(e.id)) {
        Serial.print("NAV_BTN ");
    } else {
        Serial.print("MIDI_BTN ");
    }
    Serial.print(e.id);
    Serial.println(" RELEASED");
}
