#pragma once

#include "core/domain/events/EventSystem.hpp"
#include "app/services/ControllerService.hpp"

/**
 * @brief Écouteur d'événements pour le service de contrôleurs
 */
class ControllerServiceEventListener : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param controllerService Service de contrôleurs à notifier
     */
    ControllerServiceEventListener(ControllerService& controllerService)
        : controllerService_(controllerService) {}
    
    /**
     * @brief Méthode appelée lors de la réception d'un événement
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    virtual bool onEvent(const Event& event) override {
        // Récupérer les contrôleurs
        auto& menuController = controllerService_.getMenuController();
        auto& midiMapper = controllerService_.getMidiMapper();
        
        // Vérifier le type d'événement
        switch (event.getType()) {
            case EventTypes::EncoderTurned: {
                const EncoderTurnedEvent& encoderEvent = static_cast<const EncoderTurnedEvent&>(event);
                
                // TODO: Déterminer si c'est un encodeur de navigation ou MIDI
                // Pour l'instant, on suppose que l'encodeur 79 est pour la navigation
                if (encoderEvent.id == 79) {
                    // Encodeur de navigation
                    if (menuController.isInMenu()) {
                        // Si on est dans un menu, naviguer
                        if (encoderEvent.position > 0) {
                            menuController.selectNextItem();
                        } else {
                            menuController.selectPreviousItem();
                        }
                    }
                } else {
                    // Encodeur MIDI
                    midiMapper.processEncoderChange(encoderEvent.id, encoderEvent.position);
                }
                return true;
            }
            
            case EventTypes::EncoderButton: {
                const EncoderButtonEvent& buttonEvent = static_cast<const EncoderButtonEvent&>(event);
                
                // TODO: Déterminer si c'est un bouton de navigation ou MIDI
                // Pour l'instant, on suppose que le bouton d'encodeur 79 est pour la navigation
                if (buttonEvent.id == 79) {
                    // Bouton de navigation
                    if (buttonEvent.pressed) {
                        if (menuController.isInMenu()) {
                            // Si on est dans un menu, sélectionner l'élément
                            // TODO: Action spécifique selon l'élément sélectionné
                        } else {
                            // Sinon, entrer dans le menu
                            menuController.enterMenu();
                        }
                    }
                } else {
                    // Bouton MIDI
                    midiMapper.processEncoderButton(buttonEvent.id, buttonEvent.pressed);
                }
                return true;
            }
            
            case EventTypes::ButtonPressed: {
                const ButtonPressedEvent& pressEvent = static_cast<const ButtonPressedEvent&>(event);
                
                // TODO: Déterminer si c'est un bouton de navigation ou MIDI
                // Pour l'instant, on suppose que les boutons 51 et 52 sont pour la navigation
                if (pressEvent.id == 51) {
                    // Bouton Menu
                    menuController.enterMenu();
                } else if (pressEvent.id == 52) {
                    // Bouton Valider
                    // Action selon le contexte
                } else {
                    // Bouton MIDI
                    midiMapper.processButtonPress(pressEvent.id, true);
                }
                return true;
            }
            
            case EventTypes::ButtonReleased: {
                const ButtonReleasedEvent& releaseEvent = static_cast<const ButtonReleasedEvent&>(event);
                
                // Pour les boutons MIDI uniquement
                if (releaseEvent.id != 51 && releaseEvent.id != 52) {
                    midiMapper.processButtonPress(releaseEvent.id, false);
                }
                return true;
            }
        }
        
        return false;  // Événement non traité
    }
    
private:
    ControllerService& controllerService_;
};
