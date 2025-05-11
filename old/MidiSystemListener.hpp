#pragma once

#include "core/domain/events/EventSystem.hpp"
#include "app/services/MidiSystem.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "tools/Diagnostics.hpp"

/**
 * @brief Écouteur d'événements pour le système MIDI avec navigation
 */
class MidiSystemEventListener : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param midiSystem Système MIDI à notifier
     * @param navService Service de configuration de navigation
     */
    MidiSystemEventListener(MidiSystem& midiSystem, NavigationConfigService& navService)
        : midiSystem_(midiSystem), navService_(navService) {}
    
    /**
     * @brief Méthode appelée lors de la réception d'un événement
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    virtual bool onEvent(const Event& event) override {
        // Vérifier le type d'événement
        switch (event.getType()) {
            case EventTypes::EncoderTurned: {
                const EncoderTurnedEvent& encoderEvent = static_cast<const EncoderTurnedEvent&>(event);
                
                // Diagnostics pour l'événement d'encodeur
                char diagEvent[60];
                bool isNav = navService_.isNavigationControl(encoderEvent.id);
                snprintf(diagEvent, sizeof(diagEvent), 
                         "MidiEventListener: Encodeur %d (%s)", encoderEvent.id, 
                         isNav ? "navigation" : "MIDI");
                DIAG_ON_EVENT(diagEvent);
                
                // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
                if (!isNav) {
                    midiSystem_.getMidiMapper().processEncoderChange(encoderEvent.id, encoderEvent.position);
                    return true;
                }
                break;
            }
            
            case EventTypes::EncoderButton: {
                const EncoderButtonEvent& buttonEvent = static_cast<const EncoderButtonEvent&>(event);
                
                // Diagnostics pour l'événement de bouton d'encodeur
                char diagEvent[60];
                bool isNav = navService_.isNavigationControl(buttonEvent.id);
                snprintf(diagEvent, sizeof(diagEvent), 
                         "MidiEventListener: Bouton enc %d %s (%s)", 
                         buttonEvent.id, buttonEvent.pressed ? "pressé" : "relâché",
                         isNav ? "navigation" : "MIDI");
                DIAG_ON_EVENT(diagEvent);
                
                // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
                if (!isNav) {
                    midiSystem_.getMidiMapper().processEncoderButton(buttonEvent.id, buttonEvent.pressed);
                    return true;
                }
                break;
            }
            
            case EventTypes::ButtonPressed: {
                const ButtonPressedEvent& pressEvent = static_cast<const ButtonPressedEvent&>(event);
                
                // Diagnostics pour l'événement de bouton pressé
                char diagEvent[60];
                bool isNav = navService_.isNavigationControl(pressEvent.id);
                snprintf(diagEvent, sizeof(diagEvent), 
                         "MidiEventListener: Bouton %d pressé (%s)",
                         pressEvent.id, isNav ? "navigation" : "MIDI");
                DIAG_ON_EVENT(diagEvent);
                
                // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
                if (!isNav) {
                    midiSystem_.getMidiMapper().processButtonPress(pressEvent.id, true);
                    return true;
                }
                break;
            }
            
            case EventTypes::ButtonReleased: {
                const ButtonReleasedEvent& releaseEvent = static_cast<const ButtonReleasedEvent&>(event);
                
                // Diagnostics pour l'événement de bouton relâché
                char diagEvent[60];
                bool isNav = navService_.isNavigationControl(releaseEvent.id);
                snprintf(diagEvent, sizeof(diagEvent), 
                         "MidiEventListener: Bouton %d relâché (%s)",
                         releaseEvent.id, isNav ? "navigation" : "MIDI");
                DIAG_ON_EVENT(diagEvent);
                
                // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
                if (!isNav) {
                    midiSystem_.getMidiMapper().processButtonPress(releaseEvent.id, false);
                    return true;
                }
                break;
            }
        }
        
        return false;  // Événement non traité
    }
    
private:
    MidiSystem& midiSystem_;
    NavigationConfigService& navService_;
};

/**
 * @brief Écouteur d'événements simple pour le système MIDI sans filtrage de navigation
 */
class MidiSystemSimpleListener : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param midiSystem Système MIDI à notifier
     */
    MidiSystemSimpleListener(MidiSystem& midiSystem)
        : midiSystem_(midiSystem) {}
    
    /**
     * @brief Méthode appelée lors de la réception d'un événement
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    virtual bool onEvent(const Event& event) override {
        // Vérifier le type d'événement
        switch (event.getType()) {
            case EventTypes::EncoderTurned: {
                const EncoderTurnedEvent& encoderEvent = static_cast<const EncoderTurnedEvent&>(event);
                
                // Diagnostics pour l'événement d'encodeur
                char diagEvent[70];
                snprintf(diagEvent, sizeof(diagEvent), 
                         "MidiSimpleListener: Encodeur %d (pos=%ld, delta=%d)", 
                         encoderEvent.id, encoderEvent.position, encoderEvent.delta);
                DIAG_ON_EVENT(diagEvent);
                
                midiSystem_.getMidiMapper().processEncoderChange(encoderEvent.id, encoderEvent.position);
                return true;
            }
            
            case EventTypes::EncoderButton: {
                const EncoderButtonEvent& buttonEvent = static_cast<const EncoderButtonEvent&>(event);
                
                // Diagnostics pour l'événement de bouton d'encodeur
                char diagEvent[60];
                snprintf(diagEvent, sizeof(diagEvent), 
                         "MidiSimpleListener: Bouton enc %d %s", 
                         buttonEvent.id, buttonEvent.pressed ? "pressé" : "relâché");
                DIAG_ON_EVENT(diagEvent);
                
                midiSystem_.getMidiMapper().processEncoderButton(buttonEvent.id, buttonEvent.pressed);
                return true;
            }
            
            case EventTypes::ButtonPressed: {
                const ButtonPressedEvent& pressEvent = static_cast<const ButtonPressedEvent&>(event);
                
                // Diagnostics pour l'événement de bouton pressé
                char diagEvent[50];
                snprintf(diagEvent, sizeof(diagEvent), 
                         "MidiSimpleListener: Bouton %d pressé", pressEvent.id);
                DIAG_ON_EVENT(diagEvent);
                
                midiSystem_.getMidiMapper().processButtonPress(pressEvent.id, true);
                return true;
            }
            
            case EventTypes::ButtonReleased: {
                const ButtonReleasedEvent& releaseEvent = static_cast<const ButtonReleasedEvent&>(event);
                
                // Diagnostics pour l'événement de bouton relâché
                char diagEvent[50];
                snprintf(diagEvent, sizeof(diagEvent), 
                         "MidiSimpleListener: Bouton %d relâché", releaseEvent.id);
                DIAG_ON_EVENT(diagEvent);
                
                midiSystem_.getMidiMapper().processButtonPress(releaseEvent.id, false);
                return true;
            }
        }
        
        return false;  // Événement non traité
    }
    
private:
    MidiSystem& midiSystem_;
};