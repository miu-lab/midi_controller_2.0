#include "EventRouter.hpp"

EventRouter::EventRouter(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager)
    , started_(false) {
}

EventRouter::~EventRouter() {
    stop();
}

bool EventRouter::registerHandler(EventType eventType, 
                                 EventHandler handler,
                                 EventPriority priority) {
    if (!eventManager_ || !handler) {
        return false;
    }

    // Stocker le handler
    handlers_[eventType] = handler;

    // Si déjà démarré, s'abonner immédiatement
    if (started_) {
        auto selfPtr = std::shared_ptr<EventRouter>(this, [](EventRouter*) {
            // Custom deleter qui ne fait rien
        });
        
        SubscriptionId subId = eventManager_->subscribe(eventType, selfPtr, priority);
        if (subId != 0) {
            subscriptions_[eventType] = subId;
            return true;
        }
        return false;
    }

    return true;
}

void EventRouter::unregisterHandler(EventType eventType) {
    // Supprimer le handler
    handlers_.erase(eventType);

    // Se désabonner si nécessaire
    auto it = subscriptions_.find(eventType);
    if (it != subscriptions_.end()) {
        if (eventManager_) {
            eventManager_->unsubscribe(it->second);
        }
        subscriptions_.erase(it);
    }
}

void EventRouter::start() {
    if (started_) {
        return;
    }

    subscribeToEvents();
    started_ = true;
}

void EventRouter::stop() {
    if (!started_) {
        return;
    }

    unsubscribeFromEvents();
    started_ = false;
}

bool EventRouter::onEvent(const Event& event) {
    auto it = handlers_.find(event.getType());
    if (it != handlers_.end()) {
        return it->second(event);
    }
    return false; // Événement non traité
}

size_t EventRouter::getHandlerCount() const {
    return handlers_.size();
}

bool EventRouter::hasHandler(EventType eventType) const {
    return handlers_.find(eventType) != handlers_.end();
}

void EventRouter::subscribeToEvents() {
    if (!eventManager_) {
        return;
    }

    auto selfPtr = std::shared_ptr<EventRouter>(this, [](EventRouter*) {
        // Custom deleter qui ne fait rien
    });

    // S'abonner à tous les types d'événements enregistrés
    for (const auto& pair : handlers_) {
        EventType eventType = pair.first;
        SubscriptionId subId = eventManager_->subscribe(eventType, selfPtr);
        if (subId != 0) {
            subscriptions_[eventType] = subId;
        }
    }
}

void EventRouter::unsubscribeFromEvents() {
    if (!eventManager_) {
        return;
    }

    // Se désabonner de tous les événements
    for (const auto& pair : subscriptions_) {
        eventManager_->unsubscribe(pair.second);
    }
    subscriptions_.clear();
}