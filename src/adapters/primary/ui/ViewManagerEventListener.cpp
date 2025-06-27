#include "ViewManagerEventListener.hpp"

#include "config/debug/DebugMacros.hpp"

ViewManagerEventListener::ViewManagerEventListener(ViewManager& viewManager)
    : m_viewManager(viewManager), m_subscriptionId(0) {
}

ViewManagerEventListener::~ViewManagerEventListener() {
    unsubscribe();
}

void ViewManagerEventListener::subscribe() {
    if (m_subscriptionId == 0) {
        m_subscriptionId = EventBus::getInstance().subscribe(this);
        DEBUG_LOG(DEBUG_LEVEL_INFO,
                  "ViewManagerEventListener: Subscribed with ID %d",
                  m_subscriptionId);
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

            DEBUG_LOG(DEBUG_LEVEL_INFO,
                      "MIDI CC Event: CH%d CC%d = %d",
                      ccEvent.channel,
                      ccEvent.controller,
                      ccEvent.value);

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
            DEBUG_LOG(DEBUG_LEVEL_INFO, "MIDI Note event received (ignored for now)");
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
            // Traiter les événements d'encodeur tourné
            const auto& encoderEvent = static_cast<const EncoderTurnedEvent&>(event);
            DEBUG_LOG(DEBUG_LEVEL_INFO,
                      "Encoder %d position: %d",
                      encoderEvent.id,
                      encoderEvent.position);

            // Pour l'instant, on peut simplement logger
            // L'implémentation dépendra de comment vous voulez mapper les encodeurs aux paramètres
            
            return true;
        }
        
        case EventTypes::EncoderButton: {
            // Traiter les événements de bouton d'encodeur
            const auto& buttonEvent = static_cast<const EncoderButtonEvent&>(event);
            DEBUG_LOG(DEBUG_LEVEL_INFO,
                      "Encoder button %d %s",
                      buttonEvent.id,
                      buttonEvent.pressed ? "pressed" : "released");

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

            DEBUG_LOG(DEBUG_LEVEL_INFO, "Button %d %s", id, pressed ? "pressed" : "released");

            // Déterminer si c'est un bouton de navigation
            if (pressed && isNavigationButton(id)) {
                DEBUG_LOG(DEBUG_LEVEL_INFO, "Navigation button pressed: %d", id);

                if (id == 51) { // Bouton MENU
                    DEBUG_LOG(DEBUG_LEVEL_INFO, "Showing menu via navigation button");
                    m_viewManager.showMenu();
                } else if (id == 52) { // Bouton HOME/VALIDATION
                    DEBUG_LOG(DEBUG_LEVEL_INFO, "Showing home via navigation button");
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