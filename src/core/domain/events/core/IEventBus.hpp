#pragma once

#include "Event.hpp"
#include "EventTypes.hpp"
#include <cstdint>

// Identifiant d'abonnement
using SubscriptionId = uint16_t;

/**
 * @brief Classe de base pour les écouteurs d'événements
 */
class EventListener {
public:
    virtual ~EventListener() = default;
    
    /**
     * @brief Méthode appelée lorsqu'un événement est reçu
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    virtual bool onEvent(const Event& event) = 0;
};

/**
 * @brief Niveau de priorité pour les écouteurs d'événements
 */
enum class EventPriority : uint8_t {
    PRIORITY_HIGH = 0,    // Haute priorité (chemin critique MIDI)
    PRIORITY_NORMAL = 1,  // Priorité normale
    PRIORITY_LOW = 2      // Basse priorité
};

namespace MidiController::Events {

/**
 * @brief Interface pour le bus d'événements
 * 
 * Cette interface définit le contrat pour tous les bus d'événements,
 * permettant l'injection de dépendance et les tests avec mocks.
 */
class IEventBus {
public:
    virtual ~IEventBus() = default;
    
    /**
     * @brief S'abonne au bus d'événements avec priorité
     * @param listener Écouteur d'événements
     * @param priority Niveau de priorité
     * @return Identifiant d'abonnement, 0 si échec
     */
    virtual SubscriptionId subscribe(EventListener* listener, EventPriority priority = EventPriority::PRIORITY_NORMAL) = 0;
    
    /**
     * @brief API de compatibilité avec l'ancien EventBus
     * @param listener Écouteur d'événements
     * @param priority Priorité numérique (0-255)
     * @return Identifiant d'abonnement
     */
    virtual SubscriptionId subscribe(EventListener* listener, uint8_t priority) = 0;
    
    /**
     * @brief Méthodes utilitaires pour un usage simplifié
     */
    virtual SubscriptionId subscribeHigh(EventListener* listener) = 0;
    virtual SubscriptionId subscribeNormal(EventListener* listener) = 0;
    virtual SubscriptionId subscribeLow(EventListener* listener) = 0;
    
    /**
     * @brief Se désabonne du bus d'événements
     * @param id Identifiant d'abonnement
     * @return true si désabonnement réussi, false sinon
     */
    virtual bool unsubscribe(SubscriptionId id) = 0;
    
    /**
     * @brief Met en pause un abonnement
     * @param id Identifiant d'abonnement
     * @return true si mise en pause réussie, false sinon
     */
    virtual bool pause(SubscriptionId id) = 0;
    
    /**
     * @brief Reprend un abonnement mis en pause
     * @param id Identifiant d'abonnement
     * @return true si reprise réussie, false sinon
     */
    virtual bool resume(SubscriptionId id) = 0;
    
    /**
     * @brief Publie un événement à tous les abonnés actifs
     * @param event Événement à publier
     * @return true si au moins un abonné a traité l'événement, false sinon
     */
    virtual bool publish(Event& event) = 0;
    
    /**
     * @brief Surcharge pour pointeur
     */
    virtual bool publish(Event* event) = 0;
    
    /**
     * @brief Supprime tous les abonnements
     */
    virtual void clear() = 0;
    
    /**
     * @brief Vérifie si un abonnement existe
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement existe, false sinon
     */
    virtual bool exists(SubscriptionId id) const = 0;
    
    /**
     * @brief Vérifie si un abonnement est actif
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement est actif, false sinon
     */
    virtual bool isActive(SubscriptionId id) const = 0;
    
    /**
     * @brief Obtient le nombre d'abonnements
     * @return Nombre d'abonnements
     */
    virtual int getCount() const = 0;
    
    /**
     * @brief Obtient la capacité actuelle du vecteur
     * @return Capacité du vecteur
     */
    virtual int getCapacity() const = 0;
    
    /**
     * @brief Obtient les statistiques de traitement d'événements
     * @param eventType Type d'événement haute priorité
     * @return Nombre d'événements traités de ce type
     */
    virtual uint32_t getEventProcessingCount(EventType eventType) const = 0;
    
    /**
     * @brief Réinitialise les compteurs de traitement d'événements
     */
    virtual void resetEventProcessingCounters() = 0;
};

} // namespace MidiController::Events