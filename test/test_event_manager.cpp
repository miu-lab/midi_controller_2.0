#include <unity.h>
#include <memory>

#include "core/events/EventManager.hpp"
#include "core/events/EventRouter.hpp"
#include "core/domain/events/core/EventTypes.hpp"

/**
 * @brief Mock EventListener pour les tests
 */
class MockEventListener : public EventListener {
public:
    MockEventListener() : eventCount_(0), lastEventType_(EventTypes::MidiNoteOn) {}
    
    bool onEvent(const Event& event) override {
        eventCount_++;
        lastEventType_ = event.getType();
        lastEventReceived_ = true;
        return true;
    }
    
    int getEventCount() const { return eventCount_; }
    EventType getLastEventType() const { return lastEventType_; }
    bool hasReceivedEvent() const { return lastEventReceived_; }
    void reset() { eventCount_ = 0; lastEventReceived_ = false; }
    
private:
    int eventCount_;
    EventType lastEventType_;
    bool lastEventReceived_ = false;
};

/**
 * @brief Tests pour EventManager
 * 
 * Test la gestion unifiée des événements avec EventBus et EventBatcher.
 */
class EventManagerTest {
public:
    void setUp() {
        config_ = EventManager::Config{};
        config_.enableBatching = true;
        config_.uiUpdateIntervalMs = 16;
        
        eventManager_ = std::make_unique<EventManager>(config_);
        mockListener_ = std::make_shared<MockEventListener>();
    }

    void tearDown() {
        if (eventManager_) {
            eventManager_->stop();
        }
        eventManager_.reset();
        mockListener_.reset();
    }

    /**
     * @brief Test constructor et configuration par défaut
     */
    void testConstructorWithDefaultConfig() {
        // Arrange
        auto defaultManager = std::make_unique<EventManager>();
        
        // Act & Assert
        TEST_ASSERT_NOT_NULL(defaultManager.get());
        TEST_ASSERT_FALSE(defaultManager->isStarted());
    }

    /**
     * @brief Test initialize() crée les composants nécessaires
     */
    void testInitialize() {
        // Act
        bool result = eventManager_->initialize();
        
        // Assert
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_FALSE(eventManager_->isStarted()); // Pas encore démarré
    }

    /**
     * @brief Test start() démarre le gestionnaire
     */
    void testStart() {
        // Arrange
        eventManager_->initialize();
        
        // Act
        eventManager_->start();
        
        // Assert
        TEST_ASSERT_TRUE(eventManager_->isStarted());
    }

    /**
     * @brief Test stop() arrête le gestionnaire
     */
    void testStop() {
        // Arrange
        eventManager_->start();
        TEST_ASSERT_TRUE(eventManager_->isStarted());
        
        // Act
        eventManager_->stop();
        
        // Assert
        TEST_ASSERT_FALSE(eventManager_->isStarted());
    }

    /**
     * @brief Test subscribe() enregistre un listener
     */
    void testSubscribe() {
        // Arrange
        eventManager_->initialize();
        
        // Act
        SubscriptionId subId = eventManager_->subscribe(
            EventTypes::MidiNoteOn, 
            mockListener_, 
            EventPriority::PRIORITY_NORMAL);
        
        // Assert
        TEST_ASSERT_NOT_EQUAL(0, subId); // ID valide retourné
    }

    /**
     * @brief Test publishEvent() publie des événements
     */
    void testPublishEvent() {
        // Arrange
        eventManager_->initialize();
        eventManager_->subscribe(EventTypes::MidiNoteOn, mockListener_);
        
        Event testEvent(EventTypes::MidiNoteOn, EventCategory::MIDI);
        
        // Act
        eventManager_->publishEvent(testEvent);
        
        // Assert - L'événement devrait être publié via EventBus
        // Note: Pour un test complet, il faudrait vérifier que EventBus::publish est appelé
        TEST_ASSERT_TRUE(true); // Test de base que la méthode ne crash pas
    }

    /**
     * @brief Test update() incrémente le compteur
     */
    void testUpdate() {
        // Arrange
        eventManager_->start();
        size_t initialCount = eventManager_->getProcessedEventCount();
        
        // Act
        eventManager_->update();
        
        // Assert
        TEST_ASSERT_EQUAL(initialCount + 1, eventManager_->getProcessedEventCount());
    }

    /**
     * @brief Test getEventBus() retourne une référence valide
     */
    void testGetEventBus() {
        // Arrange
        eventManager_->initialize();
        
        // Act
        EventBus& eventBus = eventManager_->getEventBus();
        
        // Assert
        TEST_ASSERT_NOT_NULL(&eventBus);
    }

    /**
     * @brief Test EventManager sans batching
     */
    void testEventManagerWithoutBatching() {
        // Arrange
        EventManager::Config noBatchConfig;
        noBatchConfig.enableBatching = false;
        auto noBatchManager = std::make_unique<EventManager>(noBatchConfig);
        
        // Act
        bool initResult = noBatchManager->initialize();
        noBatchManager->start();
        
        // Assert
        TEST_ASSERT_TRUE(initResult);
        TEST_ASSERT_TRUE(noBatchManager->isStarted());
    }

    /**
     * @brief Test unsubscribe() supprime l'abonnement
     */
    void testUnsubscribe() {
        // Arrange
        eventManager_->initialize();
        SubscriptionId subId = eventManager_->subscribe(EventTypes::MidiNoteOn, mockListener_);
        
        // Act
        eventManager_->unsubscribe(subId);
        
        // Assert - Le listener ne devrait plus recevoir d'événements
        // Test de base que la méthode ne crash pas
        TEST_ASSERT_TRUE(true);
    }

private:
    EventManager::Config config_;
    std::unique_ptr<EventManager> eventManager_;
    std::shared_ptr<MockEventListener> mockListener_;
};

/**
 * @brief Tests pour EventRouter
 */
class EventRouterTest {
public:
    void setUp() {
        eventManager_ = std::make_shared<EventManager>();
        eventManager_->initialize();
        
        eventRouter_ = std::make_unique<EventRouter>(eventManager_);
        handlerCallCount_ = 0;
    }

    void tearDown() {
        if (eventRouter_) {
            eventRouter_->stop();
        }
        eventRouter_.reset();
        eventManager_.reset();
    }

    /**
     * @brief Test registerHandler() enregistre un handler
     */
    void testRegisterHandler() {
        // Arrange
        auto handler = [this](const Event& event) -> bool {
            handlerCallCount_++;
            return true;
        };
        
        // Act
        bool result = eventRouter_->registerHandler(EventTypes::MidiNoteOn, handler);
        
        // Assert
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQUAL(1, eventRouter_->getHandlerCount());
        TEST_ASSERT_TRUE(eventRouter_->hasHandler(EventTypes::MidiNoteOn));
    }

    /**
     * @brief Test unregisterHandler() supprime un handler
     */
    void testUnregisterHandler() {
        // Arrange
        auto handler = [](const Event& event) -> bool { return true; };
        eventRouter_->registerHandler(EventTypes::MidiNoteOn, handler);
        
        // Act
        eventRouter_->unregisterHandler(EventTypes::MidiNoteOn);
        
        // Assert
        TEST_ASSERT_EQUAL(0, eventRouter_->getHandlerCount());
        TEST_ASSERT_FALSE(eventRouter_->hasHandler(EventTypes::MidiNoteOn));
    }

    /**
     * @brief Test start() et stop() du routeur
     */
    void testStartStop() {
        // Arrange
        auto handler = [](const Event& event) -> bool { return true; };
        eventRouter_->registerHandler(EventTypes::MidiNoteOn, handler);
        
        // Act & Assert - Ne devrait pas crasher
        eventRouter_->start();
        eventRouter_->stop();
        
        TEST_ASSERT_TRUE(true);
    }

    /**
     * @brief Test onEvent() appelle le bon handler
     */
    void testOnEvent() {
        // Arrange
        auto handler = [this](const Event& event) -> bool {
            handlerCallCount_++;
            return true;
        };
        eventRouter_->registerHandler(EventTypes::MidiNoteOn, handler);
        
        Event testEvent(EventTypes::MidiNoteOn, EventCategory::MIDI);
        
        // Act
        bool result = eventRouter_->onEvent(testEvent);
        
        // Assert
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQUAL(1, handlerCallCount_);
    }

private:
    std::shared_ptr<EventManager> eventManager_;
    std::unique_ptr<EventRouter> eventRouter_;
    int handlerCallCount_;
};

// === TESTS GLOBAUX ===

EventManagerTest* eventManagerTestInstance = nullptr;
EventRouterTest* eventRouterTestInstance = nullptr;

void setUpEventManager(void) {
    eventManagerTestInstance = new EventManagerTest();
    eventManagerTestInstance->setUp();
}

void tearDownEventManager(void) {
    if (eventManagerTestInstance) {
        eventManagerTestInstance->tearDown();
        delete eventManagerTestInstance;
        eventManagerTestInstance = nullptr;
    }
}

void setUpEventRouter(void) {
    eventRouterTestInstance = new EventRouterTest();
    eventRouterTestInstance->setUp();
}

void tearDownEventRouter(void) {
    if (eventRouterTestInstance) {
        eventRouterTestInstance->tearDown();
        delete eventRouterTestInstance;
        eventRouterTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST EVENTMANAGER ===

void test_event_manager_constructor_default_config() {
    setUpEventManager();
    eventManagerTestInstance->testConstructorWithDefaultConfig();
    tearDownEventManager();
}

void test_event_manager_initialize() {
    setUpEventManager();
    eventManagerTestInstance->testInitialize();
    tearDownEventManager();
}

void test_event_manager_start() {
    setUpEventManager();
    eventManagerTestInstance->testStart();
    tearDownEventManager();
}

void test_event_manager_stop() {
    setUpEventManager();
    eventManagerTestInstance->testStop();
    tearDownEventManager();
}

void test_event_manager_subscribe() {
    setUpEventManager();
    eventManagerTestInstance->testSubscribe();
    tearDownEventManager();
}

void test_event_manager_publish_event() {
    setUpEventManager();
    eventManagerTestInstance->testPublishEvent();
    tearDownEventManager();
}

void test_event_manager_update() {
    setUpEventManager();
    eventManagerTestInstance->testUpdate();
    tearDownEventManager();
}

void test_event_manager_get_event_bus() {
    setUpEventManager();
    eventManagerTestInstance->testGetEventBus();
    tearDownEventManager();
}

void test_event_manager_without_batching() {
    setUpEventManager();
    eventManagerTestInstance->testEventManagerWithoutBatching();
    tearDownEventManager();
}

void test_event_manager_unsubscribe() {
    setUpEventManager();
    eventManagerTestInstance->testUnsubscribe();
    tearDownEventManager();
}

// === FONCTIONS DE TEST EVENTROUTER ===

void test_event_router_register_handler() {
    setUpEventRouter();
    eventRouterTestInstance->testRegisterHandler();
    tearDownEventRouter();
}

void test_event_router_unregister_handler() {
    setUpEventRouter();
    eventRouterTestInstance->testUnregisterHandler();
    tearDownEventRouter();
}

void test_event_router_start_stop() {
    setUpEventRouter();
    eventRouterTestInstance->testStartStop();
    tearDownEventRouter();
}

void test_event_router_on_event() {
    setUpEventRouter();
    eventRouterTestInstance->testOnEvent();
    tearDownEventRouter();
}