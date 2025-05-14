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
    explicit UIEventListener(ViewManager& viewManager);
    
    /**
     * @brief Destructeur
     */
    ~UIEventListener() override;
    
    /**
     * @brief S'abonne aux événements
     */
    void subscribe();
    
    /**
     * @brief Se désabonne des événements
     */
    void unsubscribe();
    
    /**
     * @brief Obtient l'identifiant d'abonnement
     * @return Identifiant d'abonnement, 0 si non abonné
     */
    SubscriptionId getSubscriptionId() const;
    
    /**
     * @brief Méthode appelée lorsqu'un événement est reçu
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    bool onEvent(const Event& event) override;

private:
    /**
     * @brief Gère les événements MIDI
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    bool handleMidiEvent(const Event& event);
    
    /**
     * @brief Gère les événements d'entrée (encodeurs, boutons)
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    bool handleInputEvent(const Event& event);
    
    /**
     * @brief Gère les événements UI
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    bool handleUIEvent(const Event& event);
    
    ViewManager& m_viewManager;        // Gestionnaire de vues à mettre à jour
    SubscriptionId m_subscriptionId;   // ID d'abonnement aux événements
};