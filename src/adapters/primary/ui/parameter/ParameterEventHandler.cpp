#include "ParameterEventHandler.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include "core/domain/events/UIEvent.hpp"
#include <Arduino.h>

ParameterEventHandler::ParameterEventHandler(const EventConfig& config,
                                           WidgetAccessor widgetAccessor,
                                           std::shared_ptr<WidgetMappingManager> mappingManager)
    : config_(config)
    , widgetAccessor_(widgetAccessor)
    , mappingManager_(mappingManager)
    , active_(true)
    , stats_{0, 0, 0, 0} {
    
    logInfo("ParameterEventHandler initialized");
}

bool ParameterEventHandler::onEvent(const Event& event) {
    if (!active_) {
        stats_.eventsIgnored++;
        return false;
    }
    
    stats_.totalEvents++;
    
    // Traiter les événements de mise à jour de paramètres MIDI
    if (config_.enableMidiEvents && event.getType() == UIDisplayEvents::UIParameterUpdate) {
        const UIParameterUpdateEvent& uiEvent = static_cast<const UIParameterUpdateEvent&>(event);
        bool handled = handleUIParameterUpdateEvent(uiEvent);
        if (handled) {
            stats_.midiEventsProcessed++;
        }
        return handled;
    }
    
    // Traiter les événements de boutons haute priorité
    if (config_.enableButtonEvents && event.getType() == EventTypes::HighPriorityButtonPress) {
        bool handled = handleButtonEvent(event);
        if (handled) {
            stats_.buttonEventsProcessed++;
        }
        // Ne pas marquer comme "handled" pour permettre à d'autres de le traiter
        return false;
    }
    
    // Événement non géré
    stats_.eventsIgnored++;
    return false;
}

void ParameterEventHandler::setActive(bool active) {
    if (active_ != active) {
        active_ = active;
        logInfo("ParameterEventHandler " + String(active ? "activated" : "deactivated"));
    }
}

bool ParameterEventHandler::isActive() const {
    return active_;
}

void ParameterEventHandler::updateConfig(const EventConfig& config) {
    config_ = config;
    logInfo("ParameterEventHandler configuration updated");
}

ParameterEventHandler::EventStats ParameterEventHandler::getStats() const {
    return stats_;
}

void ParameterEventHandler::resetStats() {
    stats_ = {0, 0, 0, 0};
    logDebug("Event statistics reset");
}

bool ParameterEventHandler::handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event) {
    logDebug("Processing MIDI parameter update: CC" + String(event.controller) + 
            " CH" + String(event.channel + 1) + " Val=" + String(event.value));
    
    // Obtenir le widget correspondant au CC
    ParameterWidget* widget = getWidgetForCC(event.controller);
    if (!widget) {
        logDebug("No widget mapped for CC" + String(event.controller));
        return false;
    }
    
    // Convertir le canal 0-based vers 1-based pour l'affichage
    uint8_t displayChannel = event.channel + 1;
    
    // Générer le nom du paramètre
    String parameterName = event.parameter_name.length() > 0 ? 
                          event.parameter_name : 
                          ("CC" + String(event.controller));
    
    // Mettre à jour le widget
    widget->setParameter(event.controller, displayChannel, event.value, parameterName, config_.enableAnimation);
    
    logDebug("Updated widget for CC" + String(event.controller) + " with value " + String(event.value));
    return true;
}

bool ParameterEventHandler::handleButtonEvent(const Event& event) {
    // Cast vers HighPriorityButtonPressEvent
    if (event.getType() != EventTypes::HighPriorityButtonPress) {
        return false;
    }
    
    const auto& buttonEvent = static_cast<const HighPriorityButtonPressEvent&>(event);
    
    logDebug("Processing button event: ID=" + String(buttonEvent.buttonId) + 
            " Pressed=" + String(buttonEvent.pressed ? "true" : "false"));
    
    // Obtenir le widget parent pour ce bouton
    ParameterWidget* widget = getWidgetForButton(buttonEvent.buttonId);
    if (!widget) {
        logDebug("No widget mapped for button " + String(buttonEvent.buttonId));
        return false;
    }
    
    // Vérifier que le widget a un indicateur de bouton
    if (!widget->hasButtonIndicator()) {
        logDebug("Widget for button " + String(buttonEvent.buttonId) + " has no button indicator");
        return false;
    }
    
    // Mettre à jour l'état du bouton
    widget->setButtonState(buttonEvent.pressed, config_.enableAnimation);
    
    logDebug("Updated button state for button " + String(buttonEvent.buttonId) + 
            " to " + String(buttonEvent.pressed ? "pressed" : "released"));
    return true;
}

ParameterWidget* ParameterEventHandler::getWidget(uint8_t index) const {
    if (!widgetAccessor_) {
        logError("Widget accessor not set");
        return nullptr;
    }
    
    if (index >= 8) {  // Assuming max 8 widgets
        logError("Invalid widget index: " + String(index));
        return nullptr;
    }
    
    return widgetAccessor_(index);
}

ParameterWidget* ParameterEventHandler::getWidgetForCC(uint8_t cc_number) const {
    if (!mappingManager_) {
        logError("Mapping manager not available");
        return nullptr;
    }
    
    int8_t widgetIndex = mappingManager_->getWidgetIndexForCC(cc_number);
    if (widgetIndex < 0) {
        return nullptr;  // Not mapped
    }
    
    return getWidget(static_cast<uint8_t>(widgetIndex));
}

ParameterWidget* ParameterEventHandler::getWidgetForButton(uint16_t button_id) const {
    if (!mappingManager_) {
        logError("Mapping manager not available");
        return nullptr;
    }
    
    int8_t widgetIndex = mappingManager_->getWidgetIndexForButton(button_id);
    if (widgetIndex < 0) {
        return nullptr;  // Not mapped
    }
    
    return getWidget(static_cast<uint8_t>(widgetIndex));
}

void ParameterEventHandler::logInfo(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[ParameterEventHandler] " + message);
    }
}

void ParameterEventHandler::logDebug(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[ParameterEventHandler DEBUG] " + message);
    }
}

void ParameterEventHandler::logError(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[ParameterEventHandler ERROR] " + message);
    }
}