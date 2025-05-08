#pragma once

#include "config/common/CommonIncludes.hpp"
#include "core/domain/events/core/Event.hpp"
#include "config/debug/DebugConfig.hpp"

// Identifiant d'abonnement
using SubscriptionId = uint16_t;

// Nombre maximum d'abonnements par bus d'événements
#ifndef MAX_EVENT_LISTENERS
#define MAX_EVENT_LISTENERS 16
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
 * @brief Bus d'événements simplifié pour l'embarqué
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
     * @brief S'abonne au bus d'événements
     * @param listener Écouteur d'événements
     * @param priority Priorité (0-255, plus élevé = plus prioritaire)
     * @return Identifiant d'abonnement, 0 si échec
     */
    SubscriptionId subscribe(EventListener* listener, uint8_t priority = 0) {
        if (!listener || count_ >= MAX_EVENT_LISTENERS) {
            return 0;  // Échec
        }
        
        // Trouver un emplacement approprié basé sur la priorité
        int insertIndex = count_;
        for (int i = 0; i < count_; i++) {
            if (priorities_[i] < priority) {
                insertIndex = i;
                break;
            }
        }
        
        // Déplacer les éléments existants pour faire de la place
        if (insertIndex < count_) {
            for (int i = count_; i > insertIndex; i--) {
                listeners_[i] = listeners_[i-1];
                priorities_[i] = priorities_[i-1];
                ids_[i] = ids_[i-1];
                active_[i] = active_[i-1];
            }
        }
        
        // Insérer le nouvel abonnement
        listeners_[insertIndex] = listener;
        priorities_[insertIndex] = priority;
        ids_[insertIndex] = nextId_;
        active_[insertIndex] = true;
        
        // Incrémenter le compteur et l'ID
        count_++;
        
        // Débogage
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
        Serial.print(F("EVENT_BUS: Nouvelle souscription ID="));
        Serial.print(nextId_);
        Serial.print(F(" - Total: "));
        Serial.println(count_);
#endif
        
        return nextId_++;  // Retourner l'ID actuel et incrémenter pour le prochain
    }
    
    /**
     * @brief Se désabonne du bus d'événements
     * @param id Identifiant d'abonnement
     * @return true si désabonnement réussi, false sinon
     */
    bool unsubscribe(SubscriptionId id) {
        for (int i = 0; i < count_; i++) {
            if (ids_[i] == id) {
                // Décaler tous les éléments suivants
                for (int j = i; j < count_-1; j++) {
                    listeners_[j] = listeners_[j+1];
                    priorities_[j] = priorities_[j+1];
                    ids_[j] = ids_[j+1];
                    active_[j] = active_[j+1];
                }
                
                // Réduire le compteur
                count_--;
                
                // Débogage
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
                Serial.print(F("EVENT_BUS: Désinscription ID="));
                Serial.print(id);
                Serial.print(F(" - Restants: "));
                Serial.println(count_);
#endif
                
                return true;
            }
        }
        
        return false;  // ID non trouvé
    }
    
    /**
     * @brief Met en pause un abonnement
     * @param id Identifiant d'abonnement
     * @return true si mise en pause réussie, false sinon
     */
    bool pause(SubscriptionId id) {
        for (int i = 0; i < count_; i++) {
            if (ids_[i] == id) {
                active_[i] = false;
                
                // Débogage
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
                Serial.print(F("EVENT_BUS: Mise en pause ID="));
                Serial.println(id);
#endif
                
                return true;
            }
        }
        
        return false;  // ID non trouvé
    }
    
    /**
     * @brief Reprend un abonnement mis en pause
     * @param id Identifiant d'abonnement
     * @return true si reprise réussie, false sinon
     */
    bool resume(SubscriptionId id) {
        for (int i = 0; i < count_; i++) {
            if (ids_[i] == id) {
                active_[i] = true;
                
                // Débogage
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
                Serial.print(F("EVENT_BUS: Reprise ID="));
                Serial.println(id);
#endif
                
                return true;
            }
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
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
        Serial.print(F("EVENT_BUS: Publication d'un événement de type "));
        Serial.print(event.getEventName());
        Serial.print(F(" - Destinataires: "));
        Serial.println(count_);
#endif
        
        bool handled = false;
        
        // Parcourir tous les abonnements (déjà triés par priorité)
        for (int i = 0; i < count_; i++) {
            if (active_[i] && listeners_[i]) {
                if (listeners_[i]->onEvent(event)) {
                    handled = true;
                    event.setHandled();
                    // On continue de propager l'événement même s'il est traité
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
        
        // Débogage
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
        Serial.println(F("EVENT_BUS: Effacement de tous les abonnements"));
#endif
    }
    
    /**
     * @brief Vérifie si un abonnement existe
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement existe, false sinon
     */
    bool exists(SubscriptionId id) const {
        for (int i = 0; i < count_; i++) {
            if (ids_[i] == id) {
                return true;
            }
        }
        
        return false;  // ID non trouvé
    }
    
    /**
     * @brief Vérifie si un abonnement est actif
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement est actif, false sinon
     */
    bool isActive(SubscriptionId id) const {
        for (int i = 0; i < count_; i++) {
            if (ids_[i] == id) {
                return active_[i];
            }
        }
        
        return false;  // ID non trouvé
    }
    
    /**
     * @brief Obtient le nombre d'abonnements
     * @return Nombre d'abonnements
     */
    int getCount() const {
        return count_;
    }

private:
    // Constructeur privé (singleton)
    EventBus() : count_(0), nextId_(1) {}
    
    // Empêcher la copie
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    EventListener* listeners_[MAX_EVENT_LISTENERS] = {nullptr};  // Tableau des écouteurs
    uint8_t priorities_[MAX_EVENT_LISTENERS] = {0};              // Priorités
    SubscriptionId ids_[MAX_EVENT_LISTENERS] = {0};              // IDs d'abonnement
    bool active_[MAX_EVENT_LISTENERS] = {false};                 // États actifs
    int count_;                                                 // Nombre d'abonnements
    SubscriptionId nextId_;                                     // Prochain ID d'abonnement
};
