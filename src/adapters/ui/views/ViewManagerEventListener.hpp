#pragma once

#include "core/domain/events/core/IEventBus.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include <memory>
#include "core/domain/events/MidiEvents.hpp"
#include "adapters/ui/views/ViewManager.hpp"
#include "core/domain/events/core/Event.hpp"

/**
 * @brief Écouteur d'événements pour mettre à jour l'interface utilisateur via le ViewManager
 * 
 * Cette classe écoute les événements MIDI et de contrôle et met à jour l'UI en conséquence.
 */
class ViewManagerEventListener : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param viewManager Gestionnaire de vues à mettre à jour
     * @param eventBus Bus d'événements pour s'abonner/désabonner
     */
    explicit ViewManagerEventListener(ViewManager& viewManager, std::shared_ptr<MidiController::Events::IEventBus> eventBus);
    
    /**
     * @brief Destructeur
     */
    ~ViewManagerEventListener() override;
    
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
    
    /**
     * @brief Mappe un numéro de CC à un nom de paramètre lisible
     * @param ccNumber Numéro de Control Change
     * @return Nom du paramètre
     */
    String mapCCToParameterName(uint8_t ccNumber);
    
    /**
     * @brief Détermine si un bouton est un bouton de navigation
     * @param buttonId ID du bouton
     * @return true si c'est un bouton de navigation
     */
    bool isNavigationButton(uint8_t buttonId);
    
    ViewManager& m_viewManager;        // Gestionnaire de vues à mettre à jour
    SubscriptionId m_subscriptionId;   // ID d'abonnement aux événements
    std::shared_ptr<MidiController::Events::IEventBus> m_eventBus; // Bus d'événements injecté
};