#pragma once
#include <Arduino.h>

#include "app/services/NavigationConfigService.hpp"
#include "core/domain/events/EventSystem.hpp"

/**
 * @brief Service gérant les abonnements aux événements d'interface utilisateur
 */
class UiEventService : public EventListener {
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
    
    /**
     * @brief Traite les événements reçus
     * @param event Événement à traiter
     * @return true si l'événement a été traité, false sinon
     */
    virtual bool onEvent(const Event& event) override;

private:
    // Méthodes pour afficher les messages de débogage
    void printEncoderEvent(const EncoderTurnedEvent& e);
    void printEncoderButtonEvent(const EncoderButtonEvent& e);
    void printButtonPressedEvent(const ButtonPressedEvent& e);
    void printButtonReleasedEvent(const ButtonReleasedEvent& e);

    // Service de navigation pour vérifier si un contrôle est dédié à la navigation
    NavigationConfigService* navService_ = nullptr;
    
    // ID de souscription
    SubscriptionId subscriptionId_ = 0;
};
