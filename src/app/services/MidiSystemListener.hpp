#pragma once

#include "core/domain/events/EventSystem.hpp"
#include "app/services/MidiSystem.hpp"
#include "app/services/NavigationConfigService.hpp"

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
                
                // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
                if (!navService_.isNavigationControl(encoderEvent.id)) {
                    midiSystem_.getMidiMapper().processEncoderChange(encoderEvent.id, encoderEvent.position);
                    return true;
                }
                break;
            }
            
            case EventTypes::EncoderButton: {
                const EncoderButtonEvent& buttonEvent = static_cast<const EncoderButtonEvent&>(event);
                
                // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
                if (!navService_.isNavigationControl(buttonEvent.id)) {
                    midiSystem_.getMidiMapper().processEncoderButton(buttonEvent.id, buttonEvent.pressed);
                    return true;
                }
                break;
            }
            
            case EventTypes::ButtonPressed: {
                const ButtonPressedEvent& pressEvent = static_cast<const ButtonPressedEvent&>(event);
                
                // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
                if (!navService_.isNavigationControl(pressEvent.id)) {
                    midiSystem_.getMidiMapper().processButtonPress(pressEvent.id, true);
                    return true;
                }
                break;
            }
            
            case EventTypes::ButtonReleased: {
                const ButtonReleasedEvent& releaseEvent = static_cast<const ButtonReleasedEvent&>(event);
                
                // Ne traiter l'événement que si ce n'est pas un contrôle de navigation
                if (!navService_.isNavigationControl(releaseEvent.id)) {
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
                midiSystem_.getMidiMapper().processEncoderChange(encoderEvent.id, encoderEvent.position);
                return true;
            }
            
            case EventTypes::EncoderButton: {
                const EncoderButtonEvent& buttonEvent = static_cast<const EncoderButtonEvent&>(event);
                midiSystem_.getMidiMapper().processEncoderButton(buttonEvent.id, buttonEvent.pressed);
                return true;
            }
            
            case EventTypes::ButtonPressed: {
                const ButtonPressedEvent& pressEvent = static_cast<const ButtonPressedEvent&>(event);
                midiSystem_.getMidiMapper().processButtonPress(pressEvent.id, true);
                return true;
            }
            
            case EventTypes::ButtonReleased: {
                const ButtonReleasedEvent& releaseEvent = static_cast<const ButtonReleasedEvent&>(event);
                midiSystem_.getMidiMapper().processButtonPress(releaseEvent.id, false);
                return true;
            }
        }
        
        return false;  // Événement non traité
    }
    
private:
    MidiSystem& midiSystem_;
};
