#include <unity.h>
#include <memory>
#include <functional>

#include "core/ui/UISystemCore.hpp"
#include "core/ui/DisplayManager.hpp"
// EventManager non défini dans le codebase

// Forward declarations and minimal implementations for incomplete types
class EventManager {
public:
    virtual ~EventManager() = default;
};

class ViewManager {
public:
    virtual ~ViewManager() = default;
};

class ViewManagerEventListener {
public:
    virtual ~ViewManagerEventListener() = default;
};

/**
 * @brief Mock ViewManager pour les tests
 */
class MockViewManagerForCore {
public:
    MockViewManagerForCore() : updateCalled_(false), showModalCalled_(false), hideModalCalled_(false) {}
    
    void update() { updateCalled_ = true; }
    void showModal(const char* message) { 
        showModalCalled_ = true; 
        lastMessage_ = message ? message : "";
    }
    void hideModal() { hideModalCalled_ = true; }
    
    bool updateCalled() const { return updateCalled_; }
    bool showModalCalled() const { return showModalCalled_; }
    bool hideModalCalled() const { return hideModalCalled_; }
    const std::string& getLastMessage() const { return lastMessage_; }
    
    void reset() {
        updateCalled_ = false;
        showModalCalled_ = false;
        hideModalCalled_ = false;
        lastMessage_.clear();
    }

private:
    bool updateCalled_;
    bool showModalCalled_;
    bool hideModalCalled_;
    std::string lastMessage_;
};

/**
 * @brief Mock DisplayManager pour les tests
 */
class MockDisplayManagerForCore {
public:
    MockDisplayManagerForCore() : updateCalled_(false) {}
    
    void update() { updateCalled_ = true; }
    bool updateCalled() const { return updateCalled_; }
    void reset() { updateCalled_ = false; }

private:
    bool updateCalled_;
};

/**
 * @brief Mock EventManager pour les tests
 */
class MockEventManagerForCore {
public:
    MockEventManagerForCore() : updateCalled_(false) {}
    
    void update() { updateCalled_ = true; }
    bool updateCalled() const { return updateCalled_; }
    void reset() { updateCalled_ = false; }

private:
    bool updateCalled_;
};

/**
 * @brief Tests pour UISystemCore
 */
class UISystemCoreTest {
public:
    void setUp() {
        // Configuration par défaut avec Full UI activé
        config_.enableFullUI = true;
        config_.enableEventProcessing = true;
        config_.enableDisplayRefresh = true;
        
        uiCore_ = std::make_unique<UISystemCore>(config_);
        
        // Créer les mocks
        mockViewManager_ = std::make_shared<MockViewManagerForCore>();
        mockDisplayManager_ = std::make_unique<MockDisplayManagerForCore>();
        mockEventManager_ = std::make_unique<MockEventManagerForCore>();
    }

    void tearDown() {
        uiCore_.reset();
        mockViewManager_.reset();
        mockDisplayManager_.reset();
        mockEventManager_.reset();
    }

    /**
     * @brief Test constructeur avec configuration par défaut
     */
    void testConstructorDefaultConfig() {
        // Arrange & Act
        auto defaultCore = std::make_unique<UISystemCore>();
        
        // Assert
        TEST_ASSERT_NOT_NULL(defaultCore.get());
        TEST_ASSERT_FALSE(defaultCore->isInitialized());
        TEST_ASSERT_FALSE(defaultCore->isOperational());
    }

    /**
     * @brief Test constructeur avec configuration personnalisée
     */
    void testConstructorCustomConfig() {
        // Arrange
        UISystemCore::CoreConfig customConfig;
        customConfig.enableFullUI = true;
        customConfig.enableEventProcessing = false;
        
        // Act
        auto customCore = std::make_unique<UISystemCore>(customConfig);
        
        // Assert
        TEST_ASSERT_NOT_NULL(customCore.get());
        TEST_ASSERT_FALSE(customCore->isInitialized());
    }

    /**
     * @brief Test initialize avec tous les composants valides
     */
    void testInitializeSuccess() {
        // Arrange - utiliser des pointeurs nullptr pour les tests
        auto viewManager = std::shared_ptr<ViewManager>(nullptr);
        auto displayManager = std::unique_ptr<DisplayManager>(nullptr);
        auto eventManager = std::unique_ptr<EventManager>(nullptr);
        
        // Act
        auto result = uiCore_->initialize(std::move(viewManager), std::move(displayManager), std::move(eventManager));
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(uiCore_->isInitialized());
        TEST_ASSERT_TRUE(uiCore_->isOperational());
    }

    /**
     * @brief Test initialize avec ViewManager manquant (Full UI activé)
     */
    void testInitializeMissingViewManager() {
        // Arrange
        auto displayManager = std::unique_ptr<DisplayManager>(nullptr);
        auto eventManager = std::unique_ptr<EventManager>(nullptr);
        
        // Act
        auto result = uiCore_->initialize(nullptr, std::move(displayManager), std::move(eventManager));
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_FALSE(uiCore_->isInitialized());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, result.error().value().code);
    }

    /**
     * @brief Test initialize sans Full UI (composants optionnels)
     */
    void testInitializeWithoutFullUI() {
        // Arrange
        UISystemCore::CoreConfig minimalConfig;
        minimalConfig.enableFullUI = false;
        auto minimalCore = std::make_unique<UISystemCore>(minimalConfig);
        
        // Act
        auto result = minimalCore->initialize(nullptr, nullptr, nullptr);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(minimalCore->isInitialized());
        TEST_ASSERT_FALSE(minimalCore->isOperational()); // Pas opérationnel sans Full UI
    }

    /**
     * @brief Test double initialisation
     */
    void testDoubleInitialization() {
        // Arrange
        auto viewManager = std::shared_ptr<ViewManager>(nullptr);
        auto displayManager = std::unique_ptr<DisplayManager>(nullptr);
        auto eventManager = std::unique_ptr<EventManager>(nullptr);
        
        uiCore_->initialize(viewManager, std::move(displayManager), std::move(eventManager));
        
        // Act - deuxième initialisation
        auto result = uiCore_->initialize(viewManager, nullptr, nullptr);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(uiCore_->isInitialized());
    }

    /**
     * @brief Test showMessage avec système opérationnel
     */
    void testShowMessageSuccess() {
        // Arrange
        initializeWithMocks();
        
        // Act
        auto result = uiCore_->showMessage("Test Message");
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
    }

    /**
     * @brief Test showMessage avec système non opérationnel
     */
    void testShowMessageNotOperational() {
        // Arrange - ne pas initialiser
        
        // Act
        auto result = uiCore_->showMessage("Test Message");
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::OperationFailed, result.error().value().code);
    }

    /**
     * @brief Test clearDisplay avec système opérationnel
     */
    void testClearDisplaySuccess() {
        // Arrange
        initializeWithMocks();
        
        // Act
        auto result = uiCore_->clearDisplay();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
    }

    /**
     * @brief Test clearDisplay avec système non opérationnel
     */
    void testClearDisplayNotOperational() {
        // Arrange - ne pas initialiser
        
        // Act
        auto result = uiCore_->clearDisplay();
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::OperationFailed, result.error().value().code);
    }

    /**
     * @brief Test isOperational dans différents états
     */
    void testIsOperational() {
        // Test 1: Non initialisé
        TEST_ASSERT_FALSE(uiCore_->isOperational());
        
        // Test 2: Initialisé avec Full UI
        initializeWithMocks();
        TEST_ASSERT_TRUE(uiCore_->isOperational());
        
        // Test 3: Configuration sans Full UI
        UISystemCore::CoreConfig noUIConfig;
        noUIConfig.enableFullUI = false;
        auto noUICore = std::make_unique<UISystemCore>(noUIConfig);
        noUICore->initialize(nullptr, nullptr, nullptr);
        TEST_ASSERT_FALSE(noUICore->isOperational());
    }

private:
    UISystemCore::CoreConfig config_;
    std::unique_ptr<UISystemCore> uiCore_;
    std::shared_ptr<MockViewManagerForCore> mockViewManager_;
    std::unique_ptr<MockDisplayManagerForCore> mockDisplayManager_;
    std::unique_ptr<MockEventManagerForCore> mockEventManager_;

    void initializeWithMocks() {
        auto viewManager = std::shared_ptr<ViewManager>(nullptr);
        auto displayManager = std::unique_ptr<DisplayManager>(nullptr);
        auto eventManager = std::unique_ptr<EventManager>(nullptr);
        
        uiCore_->initialize(std::move(viewManager), std::move(displayManager), std::move(eventManager));
    }
};

// === TESTS GLOBAUX ===

UISystemCoreTest* uiSystemCoreTestInstance = nullptr;

void setUpUISystemCore(void) {
    uiSystemCoreTestInstance = new UISystemCoreTest();
    uiSystemCoreTestInstance->setUp();
}

void tearDownUISystemCore(void) {
    if (uiSystemCoreTestInstance) {
        uiSystemCoreTestInstance->tearDown();
        delete uiSystemCoreTestInstance;
        uiSystemCoreTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_ui_system_core_constructor_default_config() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testConstructorDefaultConfig();
    tearDownUISystemCore();
}

void test_ui_system_core_constructor_custom_config() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testConstructorCustomConfig();
    tearDownUISystemCore();
}

void test_ui_system_core_initialize_success() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testInitializeSuccess();
    tearDownUISystemCore();
}

void test_ui_system_core_initialize_missing_view_manager() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testInitializeMissingViewManager();
    tearDownUISystemCore();
}

void test_ui_system_core_initialize_without_full_ui() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testInitializeWithoutFullUI();
    tearDownUISystemCore();
}

void test_ui_system_core_double_initialization() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testDoubleInitialization();
    tearDownUISystemCore();
}

void test_ui_system_core_show_message_success() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testShowMessageSuccess();
    tearDownUISystemCore();
}

void test_ui_system_core_show_message_not_operational() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testShowMessageNotOperational();
    tearDownUISystemCore();
}

void test_ui_system_core_clear_display_success() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testClearDisplaySuccess();
    tearDownUISystemCore();
}

void test_ui_system_core_clear_display_not_operational() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testClearDisplayNotOperational();
    tearDownUISystemCore();
}

void test_ui_system_core_is_operational() {
    setUpUISystemCore();
    uiSystemCoreTestInstance->testIsOperational();
    tearDownUISystemCore();
}