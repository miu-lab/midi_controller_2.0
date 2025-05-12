#pragma once

#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "adapters/primary/ui/ViewManager.hpp"
#include "core/domain/events/core/Event.hpp"

/**
 * @brief Écouteur d'événements pour mettre à jour l'interface utilisateur
 * 
 * Cette classe écoute les événements MIDI et de contrôle et met à jour l'UI en conséquence.
 */
class UIEventListener : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param viewManager Gestionnaire de vues à mettre à jour
     */
    explicit UIEventListener(ViewManager& viewManager)
        : m_viewManager(viewManager), m_subscriptionId(0) {}
    
    /**
     * @brief Destructeur
     */
    ~UIEventListener() override {
        unsubscribe();
    }
    
    /**
     * @brief S'abonne aux événements
     */
    void subscribe() {
        if (m_subscriptionId == 0) {
            m_subscriptionId = EventBus::getInstance().subscribe(this);
        }
    }
    
    /**
     * @brief Se désabonne des événements
     */
    void unsubscribe() {
        if (m_subscriptionId != 0) {
            EventBus::getInstance().unsubscribe(m_subscriptionId);
            m_subscriptionId = 0;
        }
    }
    
    /**
     * @brief Méthode appelée lorsqu'un événement est reçu
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    bool onEvent(const Event& event) override {
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

private:
    /**
     * @brief Gère les événements MIDI
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    bool handleMidiEvent(const Event& event) {
        switch (event.getType()) {
            case EventTypes::MidiControlChange: {
                // Traiter les événements CC MIDI
                auto& ccEvent = static_cast<const MidiCCEvent&>(event);
                
                // Mettre à jour l'écran de surveillance des contrôles avec les informations CC
                m_viewManager.updateControlMonitorInfo(
                    ccEvent.source,
                    "CC",
                    ccEvent.channel,
                    ccEvent.controller,
                    ccEvent.value
                );
                
                return true;
            }
            
            case EventTypes::MidiNoteOn: {
                // Traiter les événements Note On MIDI
                auto& noteOnEvent = static_cast<const MidiNoteOnEvent&>(event);
                
                // Mettre à jour l'écran de surveillance des contrôles avec les informations Note On
                m_viewManager.updateControlMonitorInfo(
                    noteOnEvent.source,
                    "Note On",
                    noteOnEvent.channel,
                    noteOnEvent.note,
                    noteOnEvent.velocity
                );
                
                return true;
            }
            
            case EventTypes::MidiNoteOff: {
                // Traiter les événements Note Off MIDI
                auto& noteOffEvent = static_cast<const MidiNoteOffEvent&>(event);
                
                // Mettre à jour l'écran de surveillance des contrôles avec les informations Note Off
                m_viewManager.updateControlMonitorInfo(
                    noteOffEvent.source,
                    "Note Off",
                    noteOffEvent.channel,
                    noteOffEvent.note,
                    noteOffEvent.velocity
                );
                
                return true;
            }
            
            case EventTypes::MidiMapping: {
                // Traiter les événements de mapping MIDI
                auto& mappingEvent = static_cast<const MidiMappingEvent&>(event);
                
                // Mettre à jour l'interface utilisateur avec les informations de mapping
                // Par exemple, afficher un message temporaire sur l'écran principal
                String message = "Mapping: ";
                message += String(mappingEvent.controlId);
                message += " -> ";
                
                if (mappingEvent.midiType == 0) {
                    message += "CC";
                } else if (mappingEvent.midiType == 1) {
                    message += "Note";
                } else {
                    message += "Type ";
                    message += String(mappingEvent.midiType);
                }
                
                message += " Ch:";
                message += String(mappingEvent.midiChannel);
                message += " #";
                message += String(mappingEvent.midiNumber);
                
                // Afficher le message dans une boîte de dialogue modale
                m_viewManager.showModalDialog(message);
                
                // Fermer automatiquement la boîte de dialogue après un certain temps
                // REMARQUE: Cela nécessiterait un système de temporisation que nous n'avons pas implémenté ici
                
                return true;
            }
            
            default:
                return false;
        }
    }
    
    /**
     * @brief Gère les événements d'entrée (encodeurs, boutons)
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    bool handleInputEvent(const Event& event) {
        switch (event.getType()) {
            case EventTypes::EncoderTurned: {
                // Traiter les événements d'encodeur tourné
                auto& encoderEvent = static_cast<const EncoderTurnedEvent&>(event);
                
                // Mettre à jour l'écran principal avec la position de l'encodeur
                m_viewManager.updateEncoderPosition(encoderEvent.id, encoderEvent.position);
                
                return true;
            }
            
            case EventTypes::EncoderButton: {
                // Traiter les événements de bouton d'encodeur
                auto& buttonEvent = static_cast<const EncoderButtonEvent&>(event);
                
                // Mettre à jour l'écran principal avec l'état du bouton
                m_viewManager.updateEncoderButtonState(buttonEvent.id, buttonEvent.pressed);
                
                return true;
            }
            
            case EventTypes::ButtonPressed:
            case EventTypes::ButtonReleased: {
                // Traiter les événements de bouton
                uint8_t id = (event.getType() == EventTypes::ButtonPressed) ?
                    static_cast<const ButtonPressedEvent&>(event).id :
                    static_cast<const ButtonReleasedEvent&>(event).id;
                    
                bool pressed = (event.getType() == EventTypes::ButtonPressed);
                
                // Mettre à jour l'écran principal avec l'état du bouton
                m_viewManager.updateButtonState(id, pressed);
                
                return true;
            }
            
            default:
                return false;
        }
    }
    
    /**
     * @brief Gère les événements UI
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    bool handleUIEvent(const Event& event) {
        // Pour le moment, aucun événement UI à traiter
        // À implémenter si nécessaire
        return false;
    }
    
    ViewManager& m_viewManager;        // Gestionnaire de vues à mettre à jour
    SubscriptionId m_subscriptionId;   // ID d'abonnement aux événements
};
