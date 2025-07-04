#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include "core/domain/events/core/Event.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "EventManager.hpp"

/**
 * @brief Routeur d'événements pour dispatch typé
 * 
 * Permet d'enregistrer des handlers spécifiques par type d'événement
 * avec une API type-safe et performance optimisée.
 */
class EventRouter : public EventListener {
public:
    /**
     * @brief Type de handler d'événement
     */
    using EventHandler = std::function<bool(const Event&)>;

    /**
     * @brief Constructeur avec EventManager
     * @param eventManager Gestionnaire d'événements à utiliser
     */
    explicit EventRouter(std::shared_ptr<EventManager> eventManager);

    /**
     * @brief Destructeur
     */
    ~EventRouter() override;

    /**
     * @brief Enregistre un handler pour un type d'événement
     * @param eventType Type d'événement
     * @param handler Fonction de gestion d'événement
     * @param priority Priorité du handler
     * @return true si l'enregistrement a réussi
     */
    bool registerHandler(EventType eventType, 
                        EventHandler handler,
                        EventPriority priority = EventPriority::PRIORITY_NORMAL);

    /**
     * @brief Désenregistre un handler pour un type d'événement
     * @param eventType Type d'événement
     */
    void unregisterHandler(EventType eventType);

    /**
     * @brief Démarre le routage des événements
     */
    void start();

    /**
     * @brief Arrête le routage des événements
     */
    void stop();

    /**
     * @brief Méthode EventListener - traite les événements
     * @param event Événement reçu
     * @return true si l'événement a été traité
     */
    bool onEvent(const Event& event) override;

    /**
     * @brief Obtient le nombre de handlers enregistrés
     * @return Nombre de handlers
     */
    size_t getHandlerCount() const;

    /**
     * @brief Vérifie si un handler existe pour un type d'événement
     * @param eventType Type d'événement
     * @return true si un handler existe
     */
    bool hasHandler(EventType eventType) const;

private:
    std::shared_ptr<EventManager> eventManager_;
    std::unordered_map<EventType, EventHandler> handlers_;
    std::unordered_map<EventType, SubscriptionId> subscriptions_;
    bool started_;

    /**
     * @brief S'abonne aux événements pour les types enregistrés
     */
    void subscribeToEvents();

    /**
     * @brief Se désabonne de tous les événements
     */
    void unsubscribeFromEvents();
};