#include <vector>
#include <functional>
#include <Arduino.h>
#include "config/debug/DebugConfig.hpp"

template<typename Event>
class EventBus {
public:
    using Callback = std::function<void(const Event&)>;

    static void subscribe(Callback cb) {
        getSubscribers().emplace_back(std::move(cb));
        
        // Débogage du bus d'événements - fonction subscribe
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
        Serial.print("EVENT_BUS: Nouvelle souscription - Total: ");
        Serial.println(getSubscribers().size());
#endif
    }

    static void publish(const Event& event) {
        // Débogage du bus d'événements - fonction publish
#if defined(DEBUG) && defined(DEBUG_EVENT_BUS) && DEBUG_EVENT_BUS
        Serial.print("EVENT_BUS: Publication d'un événement - Destinataires: ");
        Serial.println(getSubscribers().size());
#endif

        for (const auto& cb : getSubscribers()) {
            cb(event);
        }
    }

private:
    static std::vector<Callback>& getSubscribers() {
        static std::vector<Callback> subscribers;
        return subscribers;
    }
};
