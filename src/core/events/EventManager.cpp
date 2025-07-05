#include "EventManager.hpp"

EventManager::EventManager(const Config& config)
    : config_(config)
    , initialized_(false)
    , started_(false)
    , processedEventCount_(0) {
}

EventManager::~EventManager() {
    stop();
}

bool EventManager::initialize() {
    if (initialized_) {
        return true;
    }

    // Obtenir l'instance EventBus
    eventBus_ = EventBus::getSharedInstance();

    // Créer et configurer l'EventBatcher si activé
    if (config_.enableBatching) {
        configureEventBatcher();
    }

    initialized_ = true;
    return true;
}

void EventManager::start() {
    if (!initialized_) {
        initialize();
    }

    if (started_) {
        return;
    }

    // Démarrer l'EventBatcher si activé
    if (eventBatcher_) {
        eventBatcher_->start();
    }

    started_ = true;
}

void EventManager::stop() {
    if (!started_) {
        return;
    }

    // Arrêter l'EventBatcher
    if (eventBatcher_) {
        eventBatcher_->stop();
    }

    started_ = false;
}

void EventManager::update() {
    if (!started_) {
        return;
    }

    // Traiter les batchs en attente
    if (eventBatcher_) {
        eventBatcher_->processPendingBatches();
    }

    processedEventCount_++;
}

void EventManager::publishEvent(const Event& event) {
    if (!initialized_ || !eventBus_) {
        return;
    }

    // EventBus::publish() requires non-const Event&
    Event mutableEvent = event;
    eventBus_->publish(mutableEvent);
}

SubscriptionId EventManager::subscribe(EventType eventType, 
                                     std::shared_ptr<EventListener> listener,
                                     EventPriority priority) {
    if (!initialized_ || !eventBus_) {
        return 0; // Invalid subscription ID
    }

    // EventBus::subscribe() takes raw pointer, not shared_ptr
    // and doesn't filter by EventType (filtering happens in EventRouter)
    return eventBus_->subscribe(listener.get(), priority);
}

void EventManager::unsubscribe(SubscriptionId subscriptionId) {
    if (!initialized_ || !eventBus_) {
        return;
    }

    eventBus_->unsubscribe(subscriptionId);
}

EventBus& EventManager::getEventBus() {
    if (!eventBus_) {
        // Fallback au singleton si pas encore initialisé
        return EventBus::getInstance();
    }
    return *eventBus_;
}

bool EventManager::isStarted() const {
    return started_;
}

size_t EventManager::getProcessedEventCount() const {
    return processedEventCount_;
}

void EventManager::configureEventBatcher() {
    EventBatcher::BatchConfig batchConfig;
    batchConfig.ui_update_interval_ms = config_.uiUpdateIntervalMs;
    batchConfig.status_update_interval_ms = config_.statusUpdateIntervalMs;
    batchConfig.coalesce_identical_values = config_.coalesceIdenticalValues;

    eventBatcher_ = std::make_unique<EventBatcher>(batchConfig);
}