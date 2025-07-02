#include "ViewManagerEventListener.hpp"



ViewManagerEventListener::ViewManagerEventListener(ViewManager& viewManager)
    : m_viewManager(viewManager), m_subscriptionId(0) {
}

ViewManagerEventListener::~ViewManagerEventListener() {
    unsubscribe();
}

void ViewManagerEventListener::subscribe() {
    if (m_subscriptionId == 0) {
        m_subscriptionId = EventBus::getInstance().subscribe(this);
        // DEBUG MSG TO IMPLEMENT
    }
}

void ViewManagerEventListener::unsubscribe() {
    if (m_subscriptionId != 0) {
        EventBus::getInstance().unsubscribe(m_subscriptionId);
        m_subscriptionId = 0;
    }
}

SubscriptionId ViewManagerEventListener::getSubscriptionId() const {
    return m_subscriptionId;
}

bool ViewManagerEventListener::onEvent(const Event& event) {
    // Gestion des événements MIDI
    if (event.getCategory() == EventCategory::MIDI) {
        return handleMidiEvent(event);
    }
    
    // Gestion des événements d'entrée (encodeurs, boutons)
    if (event.getCategory() == EventCategory::Input) {
        return handleInputEvent(event);
    }
    
    // Gestion des événements UI
    if (event.getCategory() == EventCategory::UI) {
        return handleUIEvent(event);
    }
    
    return false;
}

bool ViewManagerEventListener::handleMidiEvent(const Event& event) {
    switch (event.getType()) {
        case EventTypes::MidiControlChange: {
            // Traiter les événements CC MIDI
            auto& ccEvent = static_cast<const MidiCCEvent&>(event);

            // DEBUG MSG TO IMPLEMENT

            // Mapper le CC à un nom de paramètre
            String paramName = mapCCToParameterName(ccEvent.controller);
            
            // Afficher directement la vue ParameterFocus
            m_viewManager.showParameterFocus(
                ccEvent.controller,
                ccEvent.channel,
                ccEvent.value,
                paramName
            );
            
            return true;
        }
        
        case EventTypes::MidiNoteOn:
        case EventTypes::MidiNoteOff: {
            // Pour les notes, on peut ignorer ou traiter différemment
            // DEBUG MSG TO IMPLEMENT
            return true;
        }
        
        case EventTypes::MidiMapping: {
            // Traiter les événements de mapping MIDI
            auto& mappingEvent = static_cast<const MidiMappingEvent&>(event);
            
            String message = "Mapping: ";
            message += String(mappingEvent.controlId);
            message += " -> CC";
            message += String(mappingEvent.midiNumber);
            
            // Afficher le message dans une boîte de dialogue modale
            m_viewManager.showModal(message);
            
            return true;
        }
        
        default:
            return false;
    }
}

bool ViewManagerEventListener::handleInputEvent(const Event& event) {
    switch (event.getType()) {
        case EventTypes::EncoderTurned: {
            // DEBUG MSG TO IMPLEMENT
            return true;
        }
        
        case EventTypes::EncoderButton: {
            // DEBUG MSG TO IMPLEMENT
            return true;
        }
        
        case EventTypes::ButtonPressed:
        case EventTypes::ButtonReleased: {
            // Traiter les événements de bouton
            uint8_t id;
            bool pressed = (event.getType() == EventTypes::ButtonPressed);
            
            if (pressed) {
                id = static_cast<const ButtonPressedEvent&>(event).id;
            } else {
                id = static_cast<const ButtonReleasedEvent&>(event).id;
            }

            // DEBUG MSG TO IMPLEMENT

            // Déterminer si c'est un bouton de navigation
            if (pressed && isNavigationButton(id)) {
                // DEBUG MSG TO IMPLEMENT

                if (id == 51) { // Bouton MENU
                    // DEBUG MSG TO IMPLEMENT
                    m_viewManager.showMenu();
                } else if (id == 52) { // Bouton HOME/VALIDATION
                    // DEBUG MSG TO IMPLEMENT
                    m_viewManager.showHome();
                }
            }
            
            return true;
        }
        
        default:
            return false;
    }
}

bool ViewManagerEventListener::handleUIEvent(const Event& event) {
    // Pour le moment, aucun événement UI à traiter
    // À implémenter si nécessaire
    return false;
}

// Fonctions utilitaires
String ViewManagerEventListener::mapCCToParameterName(uint8_t ccNumber) {
    switch (ccNumber) {
        case 1: return "MOD WHEEL";
        case 7: return "VOLUME";
        case 10: return "PAN";
        case 11: return "EXPRESSION";
        case 71: return "RESONANCE";
        case 72: return "RELEASE";
        case 73: return "ATTACK";
        case 74: return "FREQUENCY";
        case 75: return "DECAY";
        case 91: return "REVERB";
        case 93: return "CHORUS";
        case 127: return "CUTOFF";
        default:
            return "CC " + String(ccNumber);
    }
}

bool ViewManagerEventListener::isNavigationButton(uint8_t buttonId) {
    // Définir quels boutons sont des boutons de navigation
    // Basé sur MappingConfiguration : 51 (Menu), 52 (Validation)
    return (buttonId == 51 || buttonId == 52);
}