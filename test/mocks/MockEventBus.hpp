#pragma once

#include "core/domain/events/core/Event.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include <vector>
#include <map>
#include <memory>

/**
 * @brief Mock pour EventBus permettant d'isoler les tests événements
 * 
 * Capture les événements publiés et permet de simuler des abonnements
 * sans dépendre du système d'événements réel.
 */
class MockEventBus {
public:
    struct CapturedEvent {
        std::unique_ptr<Event> event;
        EventPriority priority;
        unsigned long timestamp;
    };

    struct MockSubscription {
        EventListener* listener;
        EventPriority priority;
        SubscriptionId id;
    };

    MockEventBus() : next_subscription_id_(1) {}

    // Interface EventBus simulée
    SubscriptionId subscribe(EventListener* listener, EventPriority priority = EventPriority::PRIORITY_NORMAL) {
        MockSubscription sub;
        sub.listener = listener;
        sub.priority = priority;
        sub.id = next_subscription_id_++;
        
        subscriptions_[sub.id] = sub;
        return sub.id;
    }

    SubscriptionId subscribeHigh(EventListener* listener) {
        return subscribe(listener, EventPriority::PRIORITY_HIGH);
    }

    SubscriptionId subscribeNormal(EventListener* listener) {
        return subscribe(listener, EventPriority::PRIORITY_NORMAL);
    }

    SubscriptionId subscribeLow(EventListener* listener) {
        return subscribe(listener, EventPriority::PRIORITY_LOW);
    }

    bool unsubscribe(SubscriptionId id) {
        auto it = subscriptions_.find(id);
        if (it != subscriptions_.end()) {
            subscriptions_.erase(it);
            return true;
        }
        return false;
    }

    void publish(std::unique_ptr<Event> event, EventPriority priority = EventPriority::PRIORITY_NORMAL) {
        // Capturer l'événement pour validation
        CapturedEvent captured;
        captured.event = std::move(event);
        captured.priority = priority;
        captured.timestamp = millis();
        
        captured_events_.push_back(std::move(captured));
        publish_count_++;

        // Simuler la distribution aux listeners si demandé
        if (should_deliver_events_) {
            deliverToListeners(captured_events_.back().event.get(), priority);
        }
    }

    void publishHigh(std::unique_ptr<Event> event) {
        publish(std::move(event), EventPriority::PRIORITY_HIGH);
    }

    void publishNormal(std::unique_ptr<Event> event) {
        publish(std::move(event), EventPriority::PRIORITY_NORMAL);
    }

    void publishLow(std::unique_ptr<Event> event) {
        publish(std::move(event), EventPriority::PRIORITY_LOW);
    }

    // Méthodes de test
    void reset() {
        captured_events_.clear();
        subscriptions_.clear();
        publish_count_ = 0;
        next_subscription_id_ = 1;
        should_deliver_events_ = false;
    }

    void setShouldDeliverEvents(bool deliver) {
        should_deliver_events_ = deliver;
    }

    size_t getPublishCount() const {
        return publish_count_;
    }

    size_t getSubscriptionCount() const {
        return subscriptions_.size();
    }

    size_t getCapturedEventCount() const {
        return captured_events_.size();
    }

    const std::vector<CapturedEvent>& getCapturedEvents() const {
        return captured_events_;
    }

    bool hasEventOfType(EventType eventType) const {
        for (const auto& captured : captured_events_) {
            if (captured.event && captured.event->getType() == eventType) {
                return true;
            }
        }
        return false;
    }

    const Event* getLastEventOfType(EventType eventType) const {
        for (auto it = captured_events_.rbegin(); it != captured_events_.rend(); ++it) {
            if (it->event && it->event->getType() == eventType) {
                return it->event.get();
            }
        }
        return nullptr;
    }

    bool isListenerSubscribed(EventListener* listener) const {
        for (const auto& pair : subscriptions_) {
            if (pair.second.listener == listener) {
                return true;
            }
        }
        return false;
    }

    // Simulation manuelle d'événements pour les tests
    void simulateEventToListener(EventListener* listener, const Event& event) {
        if (listener) {
            listener->onEvent(event);
        }
    }

private:
    std::vector<CapturedEvent> captured_events_;
    std::map<SubscriptionId, MockSubscription> subscriptions_;
    size_t publish_count_ = 0;
    SubscriptionId next_subscription_id_;
    bool should_deliver_events_ = false;

    void deliverToListeners(const Event* event, EventPriority priority) {
        for (const auto& pair : subscriptions_) {
            const auto& sub = pair.second;
            if (static_cast<uint8_t>(sub.priority) <= static_cast<uint8_t>(priority) && sub.listener) {
                sub.listener->onEvent(*event);
            }
        }
    }
};