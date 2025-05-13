#pragma once

#include "app/services/NavigationConfigService.hpp"
#include "core/controllers/UIController.hpp"
#include "core/domain/events/core/Events.hpp"

/**
 * @brief Écouteur d'événements pour le UIController
 *
 * Cet écouteur intercepte les événements liés à la navigation et les transmet au UIController.
 * Il filtre les contrôles pour ne traiter que ceux dédiés à la navigation.
 */
class UIControllerEventListener : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param uiController Contrôleur d'interface utilisateur
     * @param navService Service de configuration de navigation
     */
    UIControllerEventListener(UIController& uiController, NavigationConfigService& navService)
        : uiController_(uiController), navService_(navService) {}

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

            // Ne traiter l'événement que si c'est un contrôle de navigation
            if (navService_.isNavigationControl(encoderEvent.id)) {
                // Transmettre la direction de rotation au UIController
                uiController_.handleEncoderTurn(encoderEvent.delta);
                return true;
            }
            break;
        }

        case EventTypes::EncoderButton: {
            const EncoderButtonEvent& buttonEvent = static_cast<const EncoderButtonEvent&>(event);

            // Ne traiter l'événement que si c'est un contrôle de navigation et que le bouton est
            // pressé
            if (navService_.isNavigationControl(buttonEvent.id) && buttonEvent.pressed) {
                // Traiter le clic d'encodeur comme un clic dans l'interface utilisateur
                uiController_.handleEncoderClick();
                return true;
            }
            break;
        }

        case EventTypes::ButtonPressed: {
            const ButtonPressedEvent& pressEvent = static_cast<const ButtonPressedEvent&>(event);

            // Ne traiter l'événement que si c'est un contrôle de navigation
            if (navService_.isNavigationControl(pressEvent.id)) {
                // En fonction de l'ID du bouton, déclencher différentes actions
                // Par exemple, ID 0 pour retour, ID 1 pour OK
                if (pressEvent.id == 0) {  // Bouton Back
                    uiController_.handleBackButton();
                    return true;
                } else if (pressEvent.id == 1) {  // Bouton OK
                    uiController_.handleOkButton();
                    return true;
                }
            }
            break;
        }
        }

        return false;  // Événement non traité
    }

private:
    UIController& uiController_;
    NavigationConfigService& navService_;
};
