#pragma once

#include <Arduino.h>

#include <functional>
#include <unordered_map>
#include <vector>

#include "config/debug/DebugConfig.hpp"

/**
 * @brief ID pour les abonnements aux événements
 */
using SubscriptionId = uint16_t;

/**
 * @brief Bus d'événements implémentant le pattern Observer
 *
 * Cette classe template permet de créer des bus d'événements pour différents types,
 * avec des fonctionnalités avancées comme la désinscription et la pause/reprise.
 *
 * @tparam Event Type d'événement à publier/recevoir
 */
template <typename Event>
class EventBus {
public:
    using Callback = std::function<void(const Event&)>;

    /**
     * @brief S'abonne à un événement
     * @param cb Fonction de callback à appeler lors de la publication
     * @return Un ID d'abonnement unique pour désinscription ultérieure
     */
    static SubscriptionId subscribe(Callback cb) {
        auto& instance = getInstance();
        SubscriptionId id = instance.nextId++;
        instance.callbacks[id] = std::move(cb);
        instance.active[id] = true;

        // Débogage du bus d'événements - fonction subscribe
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
        Serial.print(F("EVENT_BUS: Nouvelle souscription ID="));
        Serial.print(id);
        Serial.print(F(" - Total: "));
        Serial.println(instance.callbacks.size());
#endif

        return id;
    }

    /**
     * @brief Se désabonne d'un événement
     * @param id ID d'abonnement retourné par subscribe()
     * @return true si désabonnement réussi, false sinon
     */
    static bool unsubscribe(SubscriptionId id) {
        auto& instance = getInstance();
        auto it = instance.callbacks.find(id);
        if (it != instance.callbacks.end()) {
            instance.callbacks.erase(it);
            instance.active.erase(id);

            // Débogage du bus d'événements - fonction unsubscribe
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
            Serial.print(F("EVENT_BUS: Désinscription ID="));
            Serial.print(id);
            Serial.print(F(" - Restants: "));
            Serial.println(instance.callbacks.size());
#endif

            return true;
        }
        return false;
    }

    /**
     * @brief Met en pause un abonnement
     * @param id ID d'abonnement à mettre en pause
     * @return true si mise en pause réussie, false sinon
     */
    static bool pause(SubscriptionId id) {
        auto& instance = getInstance();
        auto it = instance.active.find(id);
        if (it != instance.active.end()) {
            it->second = false;

            // Débogage du bus d'événements - fonction pause
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
            Serial.print(F("EVENT_BUS: Mise en pause ID="));
            Serial.println(id);
#endif

            return true;
        }
        return false;
    }

    /**
     * @brief Reprend un abonnement mis en pause
     * @param id ID d'abonnement à réactiver
     * @return true si reprise réussie, false sinon
     */
    static bool resume(SubscriptionId id) {
        auto& instance = getInstance();
        auto it = instance.active.find(id);
        if (it != instance.active.end()) {
            it->second = true;

            // Débogage du bus d'événements - fonction resume
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
            Serial.print(F("EVENT_BUS: Reprise ID="));
            Serial.println(id);
#endif

            return true;
        }
        return false;
    }

    /**
     * @brief Publie un événement à tous les abonnés actifs
     * @param event L'événement à publier
     */
    static void publish(const Event& event) {
        auto& instance = getInstance();

        // Débogage du bus d'événements - fonction publish
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
        Serial.print(F("EVENT_BUS: Publication d'un événement - Destinataires: "));
        Serial.println(instance.callbacks.size());
#endif

        for (const auto& entry : instance.callbacks) {
            SubscriptionId id = entry.first;
            const auto& callback = entry.second;

            // Vérifier si l'abonnement est actif
            auto activeIt = instance.active.find(id);
            if (activeIt != instance.active.end() && activeIt->second) {
                callback(event);
            }
        }
    }

    /**
     * @brief Supprime tous les abonnements
     */
    static void clear() {
        auto& instance = getInstance();
        instance.callbacks.clear();
        instance.active.clear();

        // Débogage du bus d'événements - fonction clear
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
        Serial.println(F("EVENT_BUS: Effacement de tous les abonnements"));
#endif
    }

    /**
     * @brief Obtient le nombre d'abonnements
     * @return Nombre d'abonnements
     */
    static size_t count() {
        return getInstance().callbacks.size();
    }

    /**
     * @brief Vérifie si un abonnement existe
     * @param id ID d'abonnement à vérifier
     * @return true si l'abonnement existe, false sinon
     */
    static bool exists(SubscriptionId id) {
        auto& instance = getInstance();
        return instance.callbacks.find(id) != instance.callbacks.end();
    }

    /**
     * @brief Vérifie si un abonnement est actif
     * @param id ID d'abonnement à vérifier
     * @return true si l'abonnement est actif, false sinon
     */
    static bool isActive(SubscriptionId id) {
        auto& instance = getInstance();
        auto it = instance.active.find(id);
        return it != instance.active.end() && it->second;
    }

private:
    static EventBus<Event>& getInstance() {
        static EventBus<Event> instance;
        return instance;
    }

    std::unordered_map<SubscriptionId, Callback> callbacks;
    std::unordered_map<SubscriptionId, bool> active;
    SubscriptionId nextId = 0;
};
