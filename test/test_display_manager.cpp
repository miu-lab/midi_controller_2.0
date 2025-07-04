#include <unity.h>
#include <memory>

#include "core/ui/DisplayManager.hpp"

/**
 * @brief Mock Ili9341LvglBridge pour les tests DisplayManager
 */
class MockIli9341LvglBridge {
public:
    MockIli9341LvglBridge() : refreshCallCount_(0) {}
    
    void refreshDisplay() {
        refreshCallCount_++;
        lastRefreshTime_ = millis();
    }
    
    int getRefreshCallCount() const { return refreshCallCount_; }
    unsigned long getLastRefreshTime() const { return lastRefreshTime_; }
    void resetCallCount() { refreshCallCount_ = 0; }
    
private:
    int refreshCallCount_;
    unsigned long lastRefreshTime_;
};

/**
 * @brief Tests pour DisplayManager
 * 
 * Test la gestion des rafraîchissements d'écran avec timing optimal.
 */
class DisplayManagerTest {
public:
    void setUp() {
        mockBridge_ = std::make_shared<MockIli9341LvglBridge>();
        // Cast le mock vers le type attendu pour les tests
        auto bridgePtr = std::reinterpret_pointer_cast<Ili9341LvglBridge>(
            std::static_pointer_cast<void>(mockBridge_));
        displayManager_ = std::make_unique<DisplayManager>(bridgePtr);
    }

    void tearDown() {
        displayManager_.reset();
        mockBridge_.reset();
    }

    /**
     * @brief Test constructor avec bridge valide
     */
    void testConstructorWithValidBridge() {
        // Arrange & Act déjà fait dans setUp
        
        // Assert
        TEST_ASSERT_NOT_NULL(displayManager_.get());
        TEST_ASSERT_EQUAL(16, displayManager_->getRefreshInterval()); // Default 60 FPS
    }

    /**
     * @brief Test constructor avec bridge null
     */
    void testConstructorWithNullBridge() {
        // Arrange
        std::shared_ptr<Ili9341LvglBridge> nullBridge = nullptr;
        
        // Act
        auto nullDisplayManager = std::make_unique<DisplayManager>(nullBridge);
        
        // Assert
        TEST_ASSERT_NOT_NULL(nullDisplayManager.get());
        // Ne devrait pas crasher même avec un bridge null
    }

    /**
     * @brief Test update sans timing (premier appel)
     */
    void testUpdateFirstCall() {
        // Arrange
        mockBridge_->resetCallCount();
        
        // Act
        displayManager_->update();
        
        // Assert
        TEST_ASSERT_EQUAL(1, mockBridge_->getRefreshCallCount());
    }

    /**
     * @brief Test update avec timing - appels rapides
     */
    void testUpdateFastCalls() {
        // Arrange
        displayManager_->setRefreshInterval(100); // 100ms interval
        mockBridge_->resetCallCount();
        
        // Act - Premier appel
        displayManager_->update();
        TEST_ASSERT_EQUAL(1, mockBridge_->getRefreshCallCount());
        
        // Act - Appel immédiat (devrait être ignoré)
        displayManager_->update();
        
        // Assert
        TEST_ASSERT_EQUAL(1, mockBridge_->getRefreshCallCount()); // Pas d'appel supplémentaire
    }

    /**
     * @brief Test forceRefresh ignore le timing
     */
    void testForceRefreshIgnoresTiming() {
        // Arrange
        displayManager_->setRefreshInterval(1000); // 1 seconde
        mockBridge_->resetCallCount();
        
        // Act
        displayManager_->forceRefresh();
        displayManager_->forceRefresh(); // Immédiatement après
        
        // Assert
        TEST_ASSERT_EQUAL(2, mockBridge_->getRefreshCallCount()); // Les deux appels sont exécutés
    }

    /**
     * @brief Test setRefreshInterval modifie l'intervalle
     */
    void testSetRefreshInterval() {
        // Arrange
        unsigned long newInterval = 33; // ~30 FPS
        
        // Act
        displayManager_->setRefreshInterval(newInterval);
        
        // Assert
        TEST_ASSERT_EQUAL(newInterval, displayManager_->getRefreshInterval());
    }

    /**
     * @brief Test getRefreshInterval retourne la valeur par défaut
     */
    void testGetRefreshIntervalDefault() {
        // Act & Assert
        TEST_ASSERT_EQUAL(16, displayManager_->getRefreshInterval()); // 60 FPS par défaut
    }

    /**
     * @brief Test update avec bridge null ne crash pas
     */
    void testUpdateWithNullBridge() {
        // Arrange
        std::shared_ptr<Ili9341LvglBridge> nullBridge = nullptr;
        auto nullDisplayManager = std::make_unique<DisplayManager>(nullBridge);
        
        // Act & Assert - Ne devrait pas crasher
        nullDisplayManager->update();
        nullDisplayManager->forceRefresh();
        
        // Si on arrive ici, le test a réussi
        TEST_ASSERT_TRUE(true);
    }

private:
    std::shared_ptr<MockIli9341LvglBridge> mockBridge_;
    std::unique_ptr<DisplayManager> displayManager_;
};

// === TESTS GLOBAUX ===

DisplayManagerTest* displayManagerTestInstance = nullptr;

void setUpDisplayManager(void) {
    displayManagerTestInstance = new DisplayManagerTest();
    displayManagerTestInstance->setUp();
}

void tearDownDisplayManager(void) {
    if (displayManagerTestInstance) {
        displayManagerTestInstance->tearDown();
        delete displayManagerTestInstance;
        displayManagerTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_display_manager_constructor_valid_bridge() {
    setUpDisplayManager();
    displayManagerTestInstance->testConstructorWithValidBridge();
    tearDownDisplayManager();
}

void test_display_manager_constructor_null_bridge() {
    setUpDisplayManager();
    displayManagerTestInstance->testConstructorWithNullBridge();
    tearDownDisplayManager();
}

void test_display_manager_update_first_call() {
    setUpDisplayManager();
    displayManagerTestInstance->testUpdateFirstCall();
    tearDownDisplayManager();
}

void test_display_manager_update_fast_calls() {
    setUpDisplayManager();
    displayManagerTestInstance->testUpdateFastCalls();
    tearDownDisplayManager();
}

void test_display_manager_force_refresh() {
    setUpDisplayManager();
    displayManagerTestInstance->testForceRefreshIgnoresTiming();
    tearDownDisplayManager();
}

void test_display_manager_set_refresh_interval() {
    setUpDisplayManager();
    displayManagerTestInstance->testSetRefreshInterval();
    tearDownDisplayManager();
}

void test_display_manager_get_refresh_interval_default() {
    setUpDisplayManager();
    displayManagerTestInstance->testGetRefreshIntervalDefault();
    tearDownDisplayManager();
}

void test_display_manager_update_with_null_bridge() {
    setUpDisplayManager();
    displayManagerTestInstance->testUpdateWithNullBridge();
    tearDownDisplayManager();
}