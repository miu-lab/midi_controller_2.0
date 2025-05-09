#pragma once

#include "config/common/CommonIncludes.hpp"
#include "core/domain/events/core/Event.hpp"
#include "config/debug/DebugConfig.hpp"

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
        // Libérer la mémoire allouée
        if (subscriptions_) {
            delete[] subscriptions_;
            subscriptions_ = nullptr;
        }
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
        
        // Si on a atteint la capacité actuelle, agrandir le tableau
        if (count_ >= capacity_) {
            if (!growCapacity()) {
                return 0;  // Impossible d'agrandir la capacité
            }
        }
        
        // Trouver un emplacement approprié basé sur la priorité
        int insertIndex = count_;
        for (int i = 0; i < count_; i++) {
            if (subscriptions_[i].priority < priority) {
                insertIndex = i;
                break;
            }
        }
        
        // Déplacer les éléments existants pour faire de la place
        if (insertIndex < count_) {
            memmove(&subscriptions_[insertIndex + 1], 
                    &subscriptions_[insertIndex], 
                    (count_ - insertIndex) * sizeof(Subscription));
        }
        
        // Insérer le nouvel abonnement
        subscriptions_[insertIndex].listener = listener;
        subscriptions_[insertIndex].priority = priority;
        subscriptions_[insertIndex].id = nextId_;
        subscriptions_[insertIndex].active = true;
        
        // Incrémenter le compteur
        count_++;
        
        // Débogage
        DEBUG_EVENT_BUS("Nouvelle souscription ID=%d - Total: %d (Capacité: %d)\n", 
                       nextId_, count_, capacity_);
        
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
            // Décaler tous les éléments suivants
            if (index < count_ - 1) {
                memmove(&subscriptions_[index], 
                        &subscriptions_[index + 1], 
                        (count_ - index - 1) * sizeof(Subscription));
            }
            
            // Réduire le compteur
            count_--;
            
            // Réduire la capacité si nécessaire pour économiser de la mémoire
            shrinkCapacityIfNeeded();
            
            // Débogage
            DEBUG_EVENT_BUS("Désinscription ID=%d - Restants: %d (Capacité: %d)\n", 
                           id, count_, capacity_);
            
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
            
            // Débogage
            DEBUG_EVENT_BUS("Mise en pause ID=%d\n", id);
            
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
            
            // Débogage
            DEBUG_EVENT_BUS("Reprise ID=%d\n", id);
            
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
        // Débogage
        DEBUG_EVENT_BUS("Publication d'un événement de type %s - Destinataires: %d\n", 
                      event.getEventName(), count_);
        
        bool handled = false;
        
        // Parcourir tous les abonnements (déjà triés par priorité)
        for (int i = 0; i < count_; i++) {
            if (subscriptions_[i].active && subscriptions_[i].listener) {
                if (subscriptions_[i].listener->onEvent(event)) {
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
        count_ = 0;
        
        // Réduire la capacité au minimum
        resizeCapacity(INITIAL_EVENT_LISTENERS);
        
        // Débogage
        DEBUG_EVENT_BUS("Effacement de tous les abonnements\n");
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
        return count_;
    }
    
    /**
     * @brief Obtient la capacité actuelle du tableau
     * @return Capacité du tableau
     */
    int getCapacity() const {
        return capacity_;
    }

private:
    /**
     * @brief Recherche l'index d'un abonnement par son ID
     * @param id Identifiant d'abonnement à rechercher
     * @return Index de l'abonnement, ou -1 si non trouvé
     */
    int findSubscriptionIndex(SubscriptionId id) const {
        // Recherche binaire pour les performances
        // (le tableau est maintenu trié par priorité, mais les IDs sont attribués séquentiellement)
        // Pour de petits tableaux, une recherche linéaire est souvent plus rapide qu'une recherche binaire
        for (int i = 0; i < count_; i++) {
            if (subscriptions_[i].id == id) {
                return i;
            }
        }
        return -1; // Non trouvé
    }
    
    /**
     * @brief Agrandit la capacité du tableau
     * @return true si l'opération a réussi, false sinon
     */
    bool growCapacity() {
        // Nouvelle capacité = capacité actuelle * 1.5, mais limitée au maximum
        int newCapacity = min(static_cast<int>(capacity_ * 1.5f), MAX_EVENT_LISTENERS);
        
        // Si déjà à la capacité maximale, échouer
        if (newCapacity <= capacity_) {
            return false;
        }
        
        return resizeCapacity(newCapacity);
    }
    
    /**
     * @brief Réduit la capacité si nécessaire pour économiser de la mémoire
     */
    void shrinkCapacityIfNeeded() {
        // Si le tableau est utilisé à moins de 50% et qu'on a plus que la capacité initiale
        if (count_ < capacity_ / 2 && capacity_ > INITIAL_EVENT_LISTENERS) {
            // Nouvelle capacité = max(capacité initiale, count_ * 2)
            int newCapacity = max(INITIAL_EVENT_LISTENERS, count_ * 2);
            resizeCapacity(newCapacity);
        }
    }
    
    /**
     * @brief Redimensionne la capacité du tableau
     * @param newCapacity Nouvelle capacité
     * @return true si l'opération a réussi, false sinon
     */
    bool resizeCapacity(int newCapacity) {
        if (newCapacity < count_) {
            // Ne pas réduire en dessous du nombre d'éléments actuel
            newCapacity = count_;
        }
        
        if (newCapacity == capacity_) {
            return true;  // Rien à faire
        }
        
        // Allouer un nouveau tableau
        Subscription* newSubscriptions = new (std::nothrow) Subscription[newCapacity];
        if (!newSubscriptions) {
            return false;  // Échec d'allocation
        }
        
        // Copier les abonnements existants
        if (count_ > 0 && subscriptions_) {
            memcpy(newSubscriptions, subscriptions_, count_ * sizeof(Subscription));
        }
        
        // Libérer l'ancien tableau
        if (subscriptions_) {
            delete[] subscriptions_;
        }
        
        // Mettre à jour les membres
        subscriptions_ = newSubscriptions;
        capacity_ = newCapacity;
        
        // Débogage
        DEBUG_EVENT_BUS("Redimensionnement de la capacité à %d\n", capacity_);
        
        return true;
    }
    
    // Constructeur privé (singleton)
    EventBus() : subscriptions_(nullptr), count_(0), capacity_(0), nextId_(1) {
        // Initialiser avec la capacité de départ
        resizeCapacity(INITIAL_EVENT_LISTENERS);
    }
    
    // Empêcher la copie
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    Subscription* subscriptions_;  // Tableau dynamique des abonnements
    int count_;                   // Nombre d'abonnements
    int capacity_;                // Capacité du tableau
    SubscriptionId nextId_;       // Prochain ID d'abonnement
};
