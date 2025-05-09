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
        
        // Déplacer les éléments existants pour faire de la place en utilisant memmove
        if (insertIndex < count_) {
            int elementsToMove = count_ - insertIndex;
            
            // memmove est plus efficace qu'une boucle pour déplacer plusieurs éléments consécutifs
            memmove(&listeners_[insertIndex + 1], &listeners_[insertIndex], elementsToMove * sizeof(EventListener*));
            memmove(&priorities_[insertIndex + 1], &priorities_[insertIndex], elementsToMove * sizeof(uint8_t));
            memmove(&ids_[insertIndex + 1], &ids_[insertIndex], elementsToMove * sizeof(SubscriptionId));
            memmove(&active_[insertIndex + 1], &active_[insertIndex], elementsToMove * sizeof(bool));
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
        int index = findSubscriptionIndex(id);
        if (index >= 0) {
            // Calculer le nombre d'éléments à décaler
            int elementsToMove = count_ - index - 1;
            
            // Décaler tous les éléments suivants avec memmove
            if (elementsToMove > 0) {
                memmove(&listeners_[index], &listeners_[index+1], elementsToMove * sizeof(EventListener*));
                memmove(&priorities_[index], &priorities_[index+1], elementsToMove * sizeof(uint8_t));
                memmove(&ids_[index], &ids_[index+1], elementsToMove * sizeof(SubscriptionId));
                memmove(&active_[index], &active_[index+1], elementsToMove * sizeof(bool));
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
            active_[index] = false;
            
            // Débogage
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
            Serial.print(F("EVENT_BUS: Mise en pause ID="));
            Serial.println(id);
#endif
            
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
            active_[index] = true;
            
            // Débogage
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
            Serial.print(F("EVENT_BUS: Reprise ID="));
            Serial.println(id);
#endif
            
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
        return findSubscriptionIndex(id) >= 0;
    }
    
    /**
     * @brief Vérifie si un abonnement est actif
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement est actif, false sinon
     */
    bool isActive(SubscriptionId id) const {
        int index = findSubscriptionIndex(id);
        return (index >= 0) ? active_[index] : false;
    }
    
    /**
     * @brief Obtient le nombre d'abonnements
     * @return Nombre d'abonnements
     */
    int getCount() const {
        return count_;
    }

private:
    /**
     * @brief Recherche l'index d'un abonnement par son ID
     * @param id Identifiant d'abonnement à rechercher
     * @return Index de l'abonnement, ou -1 si non trouvé
     * 
     * Cette fonction optimise la recherche d'abonnements par ID.
     */
    int findSubscriptionIndex(SubscriptionId id) const {
        // Recherche linéaire optimisée
        // Pour de petits tableaux, c'est plus rapide qu'une recherche par hachage
        for (int i = 0; i < count_; i++) {
            if (ids_[i] == id) {
                return i;
            }
        }
        return -1; // Non trouvé
    }
    
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
