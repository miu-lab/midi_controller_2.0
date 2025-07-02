#pragma once

#include "config/common/CommonIncludes.hpp"
#include "core/domain/events/core/Event.hpp"
#include <vector>

// Identifiant d'abonnement
using SubscriptionId = uint16_t;

// Nombre initial et maximum d'abonnements (configurable)
#ifndef INITIAL_EVENT_LISTENERS
#define INITIAL_EVENT_LISTENERS 8
#endif

#ifndef MAX_EVENT_LISTENERS
#define MAX_EVENT_LISTENERS 24
#endif

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
 * @brief Structure compacte pour stocker les informations d'abonnement
 * Optimisée pour l'utilisation de la mémoire
 */
struct Subscription {
    EventListener* listener;  // Pointeur vers l'écouteur
    SubscriptionId id;        // ID d'abonnement
    uint8_t priority;         // Priorité (0-255)
    bool active;              // État actif/inactif
};

/**
 * @brief Bus d'événements optimisé pour l'embarqué avec allocation dynamique
 */
class EventBus {
public:
    /**
     * @brief Récupère l'instance unique du bus d'événements
     * @return Instance du bus d'événements
     */
    static EventBus& getInstance() {
        static EventBus instance;
        return instance;
    }
    
    /**
     * @brief Destructeur
     */
    ~EventBus() {
        // Le vecteur subscriptions_ sera automatiquement libéré
    }
    
    /**
     * @brief S'abonne au bus d'événements
     * @param listener Écouteur d'événements
     * @param priority Priorité (0-255, plus élevé = plus prioritaire)
     * @return Identifiant d'abonnement, 0 si échec
     */
    SubscriptionId subscribe(EventListener* listener, uint8_t priority = 0) {
        if (!listener) {
            return 0;  // Listener invalide
        }
        
        // Vérifier si on a atteint la limite maximale
        if (subscriptions_.size() >= MAX_EVENT_LISTENERS) {
            return 0;  // Trop d'abonnements
        }
        
        // Créer un nouvel abonnement
        Subscription newSubscription;
        newSubscription.listener = listener;
        newSubscription.priority = priority;
        newSubscription.id = nextId_;
        newSubscription.active = true;
        
        // Ajouter l'abonnement au vecteur
        subscriptions_.push_back(newSubscription);
        
        // Trier par priorité pour que les plus prioritaires soient traités en premier
        sortByPriority();
        
        return nextId_++;  // Retourner l'ID actuel et incrémenter pour le prochain
    }
    
    /**
     * @brief Se désabonne du bus d'événements
     * @param id Identifiant d'abonnement
     * @return true si désabonnement réussi, false sinon
     */
    bool unsubscribe(SubscriptionId id) {
        int index = findSubscriptionIndex(id);
        if (index >= 0) {
            // Supprimer l'élément à l'index spécifié
            subscriptions_.erase(subscriptions_.begin() + index);
            return true;
        }
        
        return false;  // ID non trouvé
    }
    
    /**
     * @brief Met en pause un abonnement
     * @param id Identifiant d'abonnement
     * @return true si mise en pause réussie, false sinon
     */
    bool pause(SubscriptionId id) {
        int index = findSubscriptionIndex(id);
        if (index >= 0) {
            subscriptions_[index].active = false;
            return true;
        }
        
        return false;  // ID non trouvé
    }
    
    /**
     * @brief Reprend un abonnement mis en pause
     * @param id Identifiant d'abonnement
     * @return true si reprise réussie, false sinon
     */
    bool resume(SubscriptionId id) {
        int index = findSubscriptionIndex(id);
        if (index >= 0) {
            subscriptions_[index].active = true;
            return true;
        }
        
        return false;  // ID non trouvé
    }
    
    /**
     * @brief Publie un événement à tous les abonnés actifs
     * @param event Événement à publier
     * @return true si au moins un abonné a traité l'événement, false sinon
     */
    bool publish(Event& event) {

        bool handled = false;
        
        // Parcourir tous les abonnements (déjà triés par priorité)
        for (const auto& subscription : subscriptions_) {
            if (subscription.active && subscription.listener) {
                if (subscription.listener->onEvent(event)) {
                    handled = true;
                    event.setHandled();
                }
                
                // Arrêter la propagation si demandé
                if (!event.shouldPropagate()) {
                    break;
                }
            }
        }
        
        return handled;
    }
    
    /**
     * @brief Publie un événement à tous les abonnés actifs
     * @param event Événement à publier
     * @return true si au moins un abonné a traité l'événement, false sinon
     */
    bool publish(Event* event) {
        if (event) {
            return publish(*event);
        }
        return false;
    }
    
    /**
     * @brief Supprime tous les abonnements
     */
    void clear() {
        subscriptions_.clear();
        
        // Remettre à la capacité initiale
        subscriptions_.reserve(INITIAL_EVENT_LISTENERS);
    }
    
    /**
     * @brief Vérifie si un abonnement existe
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement existe, false sinon
     */
    bool exists(SubscriptionId id) const {
        return findSubscriptionIndex(id) >= 0;
    }
    
    /**
     * @brief Vérifie si un abonnement est actif
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement est actif, false sinon
     */
    bool isActive(SubscriptionId id) const {
        int index = findSubscriptionIndex(id);
        return (index >= 0) ? subscriptions_[index].active : false;
    }
    
    /**
     * @brief Obtient le nombre d'abonnements
     * @return Nombre d'abonnements
     */
    int getCount() const {
        return subscriptions_.size();
    }
    
    /**
     * @brief Obtient la capacité actuelle du tableau
     * @return Capacité du tableau
     */
    int getCapacity() const {
        return subscriptions_.capacity();
    }

private:
    /**
     * @brief Recherche l'index d'un abonnement par son ID
     * @param id Identifiant d'abonnement à rechercher
     * @return Index de l'abonnement, ou -1 si non trouvé
     */
    int findSubscriptionIndex(SubscriptionId id) const {
        // Pour de petits tableaux, une recherche linéaire est souvent plus efficace
        for (size_t i = 0; i < subscriptions_.size(); i++) {
            if (subscriptions_[i].id == id) {
                return static_cast<int>(i);
            }
        }
        return -1; // Non trouvé
    }
    
    /**
     * @brief Trie les abonnements par priorité
     */
    void sortByPriority() {
        std::sort(subscriptions_.begin(), subscriptions_.end(), [](const Subscription& a, const Subscription& b) {
            return a.priority > b.priority; // Plus grande priorité en premier
        });
    }
    
    // Constructeur privé (singleton)
    EventBus() : nextId_(1) {
        // Initialiser avec la capacité de départ
        subscriptions_.reserve(INITIAL_EVENT_LISTENERS);
    }
    
    // Empêcher la copie
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    std::vector<Subscription> subscriptions_;  // Vecteur des abonnements
    SubscriptionId nextId_;       // Prochain ID d'abonnement
};
