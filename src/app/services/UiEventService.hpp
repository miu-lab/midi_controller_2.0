#pragma once
#include <Arduino.h>

#include "app/services/NavigationConfigService.hpp"
#include "core/domain/events/EventTypes.hpp"

/**
 * @brief Service gérant les abonnements aux événements d'interface utilisateur
 *
 * Ce service est responsable de la configuration des abonnements aux événements
 * liés à l'interface utilisateur, comme les mouvements d'encodeurs et les pressions de boutons.
 * Il gère également l'affichage des messages de débogage pour ces événements.
 */
class UiEventService {
public:
    /**
     * @brief Initialise le service avec le service de navigation
     * @param navService Service de configuration de navigation
     */
    void init(NavigationConfigService& navService);

    /**
     * @brief Configure les abonnements pour le débogage des événements
     */
    void setupDebugSubscriptions();

private:
    // Méthodes pour afficher les messages de débogage
    void printEncoderEvent(const EventTypes::EncoderTurned& e);
    void printEncoderButtonEvent(const EventTypes::EncoderButton& e);
    void printButtonPressedEvent(const EventTypes::ButtonPressed& e);
    void printButtonReleasedEvent(const EventTypes::ButtonReleased& e);

    // Service de navigation pour vérifier si un contrôle est dédié à la navigation
    NavigationConfigService* navService_ = nullptr;
};
