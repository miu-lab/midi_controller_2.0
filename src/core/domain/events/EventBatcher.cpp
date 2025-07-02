#include "EventBatcher.hpp"


EventBatcher::EventBatcher(const BatchConfig& config)
    : config_(config) {
}

void EventBatcher::start() {
    if (started_) return;
    
    // S'abonner aux événements avec l'API actuelle
    EventBus& eventBus = EventBus::getInstance();
    subscription_id_ = eventBus.subscribe(this, 90); // Priorité haute pour traiter avant l'UI
    
    started_ = true;
}

void EventBatcher::stop() {
    if (!started_) return;
    
    // Se désabonner
    if (subscription_id_ > 0) {
        EventBus& eventBus = EventBus::getInstance();
        eventBus.unsubscribe(subscription_id_);
        subscription_id_ = 0;
    }
    
    // Vider les batchs en attente
    flushUIBatch();
    
    started_ = false;
}

bool EventBatcher::onEvent(const Event& event) {
    if (!started_) return false;
    
    // Traiter seulement les événements MIDI CC
    if (event.getType() == EventTypes::MidiControlChange) {
        const auto& midi_event = static_cast<const MidiCCEvent&>(event);
        handleMidiCCEvent(midi_event);
        return true; // Événement consommé
    }
    
    return false;
}

void EventBatcher::handleMidiCCEvent(const MidiCCEvent& midi_event) {
    uint16_t key = getParameterKey(midi_event.controller, midi_event.channel);
    unsigned long now = millis();
    
    // Chercher ou créer l'entrée
    auto it = pending_parameters_.find(key);
    if (it == pending_parameters_.end()) {
        // Nouveau paramètre
        PendingParameter param;
        param.controller = midi_event.controller;
        param.channel = midi_event.channel;
        param.value = midi_event.value;
        param.name = "CC" + String(midi_event.controller);
        param.last_update_ms = now;
        param.needs_ui_update = true;
        
        pending_parameters_[key] = param;
        
    } else {
        // Paramètre existant - mise à jour
        PendingParameter& param = it->second;
        
        // Vérifier si la valeur a changé
        if (config_.coalesce_identical_values && param.value == midi_event.value) {
            return; // Valeur identique, ignorer
        }
        
        param.value = midi_event.value;
        param.last_update_ms = now;
        param.needs_ui_update = true;
        
    }
}

void EventBatcher::processPendingBatches() {
    if (!started_) return;
    
    unsigned long now = millis();
    
    // Traiter le batch UI si nécessaire
    if ((now - last_ui_batch_ms_) >= config_.ui_update_interval_ms) {
        flushUIBatch();
        last_ui_batch_ms_ = now;
    }
    
    // Traiter le batch status si nécessaire
    if ((now - last_status_batch_ms_) >= config_.status_update_interval_ms) {
        flushStatusBatch();
        last_status_batch_ms_ = now;
    }
}

void EventBatcher::flushUIBatch() {
    EventBus& eventBus = EventBus::getInstance();
    
    for (auto& [key, param] : pending_parameters_) {
        if (param.needs_ui_update) {
            // Créer et envoyer l'événement UI optimisé
            UIParameterUpdateEvent ui_event(
                param.controller,
                param.channel,
                param.value,
                param.name
            );
            
            eventBus.publish(ui_event);
            param.needs_ui_update = false;
            
        }
    }
}

void EventBatcher::flushStatusBatch() {
    // Pour l'instant, pas d'événements de status à traiter
    // Peut être étendu plus tard pour des mises à jour de status général
}