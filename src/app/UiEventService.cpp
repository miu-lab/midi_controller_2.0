#include "app/UiEventService.hpp"
#include "utils/EventBus.hpp"

void UiEventService::init(NavigationConfigService& navService) {
    // Stocker la référence au service de navigation
    navService_ = &navService;
    
    // Configurer les abonnements pour le débogage
    setupDebugSubscriptions();
}

void UiEventService::setupDebugSubscriptions() {
    // Abonnement aux événements d'encodeurs
    EventBus<EncoderTurnedEvent>::subscribe([this](const EncoderTurnedEvent& e) {
        printEncoderEvent(e);
    });
    
    // Abonnement aux événements de boutons d'encodeurs
    EventBus<EncoderButtonEvent>::subscribe([this](const EncoderButtonEvent& e) {
        printEncoderButtonEvent(e);
    });
    
    // Abonnement aux événements de pression de boutons
    EventBus<ButtonPressed>::subscribe([this](const ButtonPressed& e) {
        printButtonPressedEvent(e);
    });
    
    // Abonnement aux événements de relâchement de boutons
    EventBus<ButtonReleased>::subscribe([this](const ButtonReleased& e) {
        printButtonReleasedEvent(e);
    });
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
    Serial.println(e.absolutePosition);
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

void UiEventService::printButtonPressedEvent(const ButtonPressed& e) {
    // Distinguer les boutons de navigation
    if (navService_ && navService_->isNavigationControl(e.id)) {
        Serial.print("NAV_BTN ");
    } else {
        Serial.print("MIDI_BTN ");
    }
    Serial.print(e.id);
    Serial.println(" PRESSED");
}

void UiEventService::printButtonReleasedEvent(const ButtonReleased& e) {
    // Distinguer les boutons de navigation
    if (navService_ && navService_->isNavigationControl(e.id)) {
        Serial.print("NAV_BTN ");
    } else {
        Serial.print("MIDI_BTN ");
    }
    Serial.print(e.id);
    Serial.println(" RELEASED");
}
