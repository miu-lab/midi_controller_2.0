#pragma once

#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include <array>
#include <atomic>

/**
 * @brief Constantes pour OptimizedEventBus
 */
namespace OptimizedEventBusConfig {
    // Nombre maximum d'écouteurs par niveau de priorité
    constexpr uint8_t MAX_HIGH_PRIORITY_LISTENERS = 4;
    constexpr uint8_t MAX_NORMAL_PRIORITY_LISTENERS = 8;
    constexpr uint8_t MAX_LOW_PRIORITY_LISTENERS = 4;
    
    // Nombres maximum d'événements dans le pool
    constexpr uint8_t EVENT_POOL_SIZE = 8;
}

/**
 * @brief Niveau de priorité pour les écouteurs d'événements
 */
enum class EventPriority : uint8_t {
    PRIORITY_HIGH = 0,    // Haute priorité (chemin critique MIDI)
    PRIORITY_NORMAL = 1,  // Priorité normale
    PRIORITY_LOW = 2      // Basse priorité
};

/**
 * @brief Bus d'événements optimisé pour les chemins critiques
 * Utilise la composition au lieu de l'héritage pour éviter les problèmes avec le constructeur privé d'EventBus
 */
class OptimizedEventBus {
public:
    /**
     * @brief Constructeur
     */
    OptimizedEventBus() : highPriorityCount_(0), normalPriorityCount_(0), lowPriorityCount_(0) {
        // Initialiser les compteurs atomiques
        for (auto& counter : eventProcessingCounters_) {
            counter.store(0, std::memory_order_relaxed);
        }
        
        // Obtenir l'instance du bus d'événements standard
        eventBus_ = &EventBus::getInstance();
    }
    
    /**
     * @brief S'abonne au bus d'événements optimisé avec priorité
     * @param listener Écouteur d'événements
     * @param priority Niveau de priorité
     * @return Identifiant d'abonnement, 0 si échec
     */
    SubscriptionId subscribeWithPriority(EventListener* listener, EventPriority priority) {
        if (!listener) {
            return 0; // Écouteur invalide
        }
        
        // Vérifier et ajouter à la liste de priorité appropriée
        switch (priority) {
            case EventPriority::PRIORITY_HIGH:
                if (highPriorityCount_ >= OptimizedEventBusConfig::MAX_HIGH_PRIORITY_LISTENERS) {
                    return 0; // Liste de haute priorité pleine
                }
                highPriorityListeners_[highPriorityCount_++] = listener;
                break;
                
            case EventPriority::PRIORITY_NORMAL:
                if (normalPriorityCount_ >= OptimizedEventBusConfig::MAX_NORMAL_PRIORITY_LISTENERS) {
                    return 0; // Liste de priorité normale pleine
                }
                normalPriorityListeners_[normalPriorityCount_++] = listener;
                break;
                
            case EventPriority::PRIORITY_LOW:
                if (lowPriorityCount_ >= OptimizedEventBusConfig::MAX_LOW_PRIORITY_LISTENERS) {
                    return 0; // Liste de basse priorité pleine
                }
                lowPriorityListeners_[lowPriorityCount_++] = listener;
                break;
        }
        
        // Également s'abonner au bus d'événements standard pour la compatibilité descendante
        return eventBus_->subscribe(listener, static_cast<uint8_t>(priority) * 50); // Multiplier pour espacer les priorités
    }
    
    /**
     * @brief Publie un événement de haute priorité directement aux écouteurs appropriés
     * @param event Événement à publier
     * @return true si au moins un écouteur a traité l'événement, false sinon
     */
    bool publishHighPriority(Event& event) {
        // Incrémenter le compteur atomique pour ce type d'événement
        uint16_t eventType = event.getType();
        if (eventType >= EventTypes::HighPriorityEncoderChanged && 
            eventType <= EventTypes::HighPriorityButtonPress) {
            uint8_t index = eventType - EventTypes::HighPriorityEncoderChanged;
            if (index < eventProcessingCounters_.size()) {
                eventProcessingCounters_[index].fetch_add(1, std::memory_order_relaxed);
            }
        }
        
        bool handled = false;
        
        // Traiter d'abord par les écouteurs de haute priorité
        for (uint8_t i = 0; i < highPriorityCount_; ++i) {
            if (highPriorityListeners_[i]->onEvent(event)) {
                handled = true;
                event.setHandled();
            }
            
            if (!event.shouldPropagate()) {
                return handled;
            }
        }
        
        // Publier normalement pour les autres écouteurs si nécessaire
        if (propagateHighPriorityEvents_ && !handled) {
            handled = eventBus_->publish(event);
        }
        
        return handled;
    }
    
    /**
     * @brief Configure si les événements haute priorité doivent être propagés aux écouteurs normaux
     * @param propagate true pour propager, false sinon
     */
    void setPropagateHighPriorityEvents(bool propagate) {
        propagateHighPriorityEvents_ = propagate;
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
            if (index < eventProcessingCounters_.size()) {
                return eventProcessingCounters_[index].load(std::memory_order_relaxed);
            }
        }
        return 0;
    }
    
    /**
     * @brief Réinitialise les compteurs de traitement d'événements
     */
    void resetEventProcessingCounters() {
        for (auto& counter : eventProcessingCounters_) {
            counter.store(0, std::memory_order_relaxed);
        }
    }
    
    /**
     * @brief Méthodes déléguées vers l'EventBus standard pour la compatibilité
     */
    SubscriptionId subscribe(EventListener* listener, uint8_t priority = 0) {
        return eventBus_->subscribe(listener, priority);
    }
    
    bool unsubscribe(SubscriptionId id) {
        return eventBus_->unsubscribe(id);
    }
    
    bool pause(SubscriptionId id) {
        return eventBus_->pause(id);
    }
    
    bool resume(SubscriptionId id) {
        return eventBus_->resume(id);
    }
    
    bool publish(Event& event) {
        return eventBus_->publish(event);
    }
    
    bool publish(Event* event) {
        return eventBus_->publish(event);
    }
    
    void clear() {
        eventBus_->clear();
    }
    
    bool exists(SubscriptionId id) const {
        return eventBus_->exists(id);
    }
    
    bool isActive(SubscriptionId id) const {
        return eventBus_->isActive(id);
    }
    
    int getCount() const {
        return eventBus_->getCount();
    }
    
    int getCapacity() const {
        return eventBus_->getCapacity();
    }

private:
    // Tableaux statiques d'écouteurs par niveau de priorité
    std::array<EventListener*, OptimizedEventBusConfig::MAX_HIGH_PRIORITY_LISTENERS> highPriorityListeners_;
    std::array<EventListener*, OptimizedEventBusConfig::MAX_NORMAL_PRIORITY_LISTENERS> normalPriorityListeners_;
    std::array<EventListener*, OptimizedEventBusConfig::MAX_LOW_PRIORITY_LISTENERS> lowPriorityListeners_;
    
    // Référence vers l'EventBus standard
    EventBus* eventBus_;
    
    // Nombre actuel d'écouteurs par niveau de priorité
    uint8_t highPriorityCount_;
    uint8_t normalPriorityCount_;
    uint8_t lowPriorityCount_;
    
    // Compteurs atomiques pour le suivi des événements traités (pour diagnostics)
    std::array<std::atomic<uint32_t>, 3> eventProcessingCounters_; // Un compteur par type d'événement haute priorité
    
    // Configuration
    bool propagateHighPriorityEvents_ = true; // Par défaut, propager aux écouteurs normaux
};
