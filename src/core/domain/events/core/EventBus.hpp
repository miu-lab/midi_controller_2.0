#pragma once

#include "core/domain/events/core/Event.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include <vector>
#include <atomic>
#include <algorithm>

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

/**
 * @brief Bus d'événements unifié - Architecture simplifiée
 * Combine EventBus et OptimizedEventBus en une seule classe moderne
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
    ~EventBus() = default;
    
    /**
     * @brief S'abonne au bus d'événements avec priorité
     * @param listener Écouteur d'événements
     * @param priority Niveau de priorité
     * @return Identifiant d'abonnement, 0 si échec
     */
    SubscriptionId subscribe(EventListener* listener, EventPriority priority = EventPriority::PRIORITY_NORMAL) {
        if (!listener) {
            return 0;
        }
        
        Subscription sub;
        sub.listener = listener;
        sub.id = nextId_++;
        sub.priority = priority;
        sub.active = true;
        
        subscriptions_.push_back(sub);
        
        // Trier par priorité pour optimiser la publication
        sortByPriority();
        
        return sub.id;
    }
    
    /**
     * @brief API de compatibilité avec l'ancien EventBus
     * @param listener Écouteur d'événements
     * @param priority Priorité numérique (0-255)
     * @return Identifiant d'abonnement
     */
    SubscriptionId subscribe(EventListener* listener, uint8_t priority) {
        EventPriority eventPriority = EventPriority::PRIORITY_NORMAL;
        if (priority >= 200) {
            eventPriority = EventPriority::PRIORITY_HIGH;
        } else if (priority < 50) {
            eventPriority = EventPriority::PRIORITY_LOW;
        }
        
        return subscribe(listener, eventPriority);
    }
    
    /**
     * @brief Méthodes utilitaires pour un usage simplifié
     */
    SubscriptionId subscribeHigh(EventListener* listener) {
        return subscribe(listener, EventPriority::PRIORITY_HIGH);
    }
    
    SubscriptionId subscribeNormal(EventListener* listener) {
        return subscribe(listener, EventPriority::PRIORITY_NORMAL);
    }
    
    SubscriptionId subscribeLow(EventListener* listener) {
        return subscribe(listener, EventPriority::PRIORITY_LOW);
    }
    
    /**
     * @brief Se désabonne du bus d'événements
     * @param id Identifiant d'abonnement
     * @return true si désabonnement réussi, false sinon
     */
    bool unsubscribe(SubscriptionId id) {
        auto it = std::find_if(subscriptions_.begin(), subscriptions_.end(),
                              [id](const Subscription& sub) { return sub.id == id; });
        
        if (it != subscriptions_.end()) {
            subscriptions_.erase(it);
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief Met en pause un abonnement
     * @param id Identifiant d'abonnement
     * @return true si mise en pause réussie, false sinon
     */
    bool pause(SubscriptionId id) {
        return setSubscriptionActive(id, false);
    }
    
    /**
     * @brief Reprend un abonnement mis en pause
     * @param id Identifiant d'abonnement
     * @return true si reprise réussie, false sinon
     */
    bool resume(SubscriptionId id) {
        return setSubscriptionActive(id, true);
    }
    
    /**
     * @brief Publie un événement à tous les abonnés actifs
     * @param event Événement à publier
     * @return true si au moins un abonné a traité l'événement, false sinon
     */
    bool publish(Event& event) {
        bool handled = false;
        
        // Traiter tous les abonnements (déjà triés par priorité)
        for (auto& subscription : subscriptions_) {
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
        
        // Incrémenter les compteurs de performance pour les événements haute priorité
        if (event.getType() >= EventTypes::HighPriorityEncoderChanged && 
            event.getType() <= EventTypes::HighPriorityButtonPress) {
            uint8_t index = event.getType() - EventTypes::HighPriorityEncoderChanged;
            if (index < eventCounters_.size()) {
                eventCounters_[index].fetch_add(1, std::memory_order_relaxed);
            }
        }
        
        return handled;
    }
    
    /**
     * @brief Surcharge pour pointeur
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
    }
    
    /**
     * @brief Vérifie si un abonnement existe
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement existe, false sinon
     */
    bool exists(SubscriptionId id) const {
        return std::any_of(subscriptions_.begin(), subscriptions_.end(),
                          [id](const Subscription& sub) { return sub.id == id; });
    }
    
    /**
     * @brief Vérifie si un abonnement est actif
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement est actif, false sinon
     */
    bool isActive(SubscriptionId id) const {
        auto it = std::find_if(subscriptions_.begin(), subscriptions_.end(),
                              [id](const Subscription& sub) { return sub.id == id; });
        
        return (it != subscriptions_.end()) ? it->active : false;
    }
    
    /**
     * @brief Obtient le nombre d'abonnements
     * @return Nombre d'abonnements
     */
    int getCount() const {
        return subscriptions_.size();
    }
    
    /**
     * @brief Obtient la capacité actuelle du vecteur
     * @return Capacité du vecteur
     */
    int getCapacity() const {
        return subscriptions_.capacity();
    }
    
    /**
     * @brief Obtient les statistiques de traitement d'événements
     * @param eventType Type d'événement haute priorité
     * @return Nombre d'événements traités de ce type
     */
    uint32_t getEventProcessingCount(EventType eventType) const {
        if (eventType >= EventTypes::HighPriorityEncoderChanged && 
            eventType <= EventTypes::HighPriorityButtonPress) {
            uint8_t index = eventType - EventTypes::HighPriorityEncoderChanged;
            if (index < eventCounters_.size()) {
                return eventCounters_[index].load(std::memory_order_relaxed);
            }
        }
        return 0;
    }
    
    /**
     * @brief Réinitialise les compteurs de traitement d'événements
     */
    void resetEventProcessingCounters() {
        for (auto& counter : eventCounters_) {
            counter.store(0, std::memory_order_relaxed);
        }
    }

private:
    /**
     * @brief Structure compacte pour stocker les informations d'abonnement
     */
    struct Subscription {
        EventListener* listener;
        SubscriptionId id;
        EventPriority priority;
        bool active;
    };
    
    // Constructeur privé (singleton)
    EventBus() : nextId_(1) {
        // Réserver de l'espace pour éviter les réallocations fréquentes
        subscriptions_.reserve(24);
        
        // Initialiser les compteurs
        for (auto& counter : eventCounters_) {
            counter.store(0, std::memory_order_relaxed);
        }
    }
    
    // Empêcher la copie
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    /**
     * @brief Trie les abonnements par priorité
     */
    void sortByPriority() {
        std::sort(subscriptions_.begin(), subscriptions_.end(),
                 [](const Subscription& a, const Subscription& b) {
                     return static_cast<uint8_t>(a.priority) < static_cast<uint8_t>(b.priority);
                 });
    }
    
    /**
     * @brief Active/désactive un abonnement
     */
    bool setSubscriptionActive(SubscriptionId id, bool active) {
        auto it = std::find_if(subscriptions_.begin(), subscriptions_.end(),
                              [id](const Subscription& sub) { return sub.id == id; });
        
        if (it != subscriptions_.end()) {
            it->active = active;
            return true;
        }
        
        return false;
    }
    
    std::vector<Subscription> subscriptions_;
    SubscriptionId nextId_;
    
    // Compteurs atomiques pour le suivi des événements traités (diagnostics)
    std::array<std::atomic<uint32_t>, 3> eventCounters_; // Un compteur par type d'événement haute priorité
};