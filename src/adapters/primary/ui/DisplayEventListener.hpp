#pragma once

#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/UIEvent.hpp"
#include "core/tasks/DisplayUpdateTask.hpp"
#include <memory>

/**
 * @brief Écouteur pour les événements liés à l'affichage
 * 
 * Cet écouteur intercepte les événements de demande de mise à jour de l'affichage
 * et les transmet à la tâche d'affichage pour exécution asynchrone.
 */
class DisplayEventListener : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param displayTask Référence à la tâche d'affichage
     */
    explicit DisplayEventListener(std::shared_ptr<DisplayUpdateTask> displayTask)
        : displayTask_(displayTask), subscriptionId_(0) {}

    /**
     * @brief S'abonne au bus d'événements
     */
    void subscribe() {
        if (subscriptionId_ == 0) { // Éviter les abonnements multiples
            subscriptionId_ = EventBus::getInstance().subscribe(this);
            Serial.println(F("DisplayEventListener: Abonné au bus d'événements"));
        }
    }

    /**
     * @brief Traite un événement
     * @param event Événement à traiter
     */
    bool onEvent(const Event& event) override {
        // Vérifier si c'est un événement de mise à jour d'affichage
        if (event.getType() == UIDisplayEvents::DisplayUpdateRequested) {
            displayTask_->requestUpdate();
            return true;
        }
        return false;
    }

private:
    std::shared_ptr<DisplayUpdateTask> displayTask_;
    SubscriptionId subscriptionId_;
};
