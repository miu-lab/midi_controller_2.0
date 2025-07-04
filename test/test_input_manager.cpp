#include <unity.h>
#include <memory>
#include <vector>

#include "core/input/InputManager.hpp"
#include "config/unified/ControlDefinition.hpp"

/**
 * @brief Mock InputController pour les tests
 */
class MockInputControllerForInputManager {
public:
    MockInputControllerForInputManager() : connected_(false) {}
    
    void setConnected(bool connected) { connected_ = connected; }
    bool isConnected() const { return connected_; }

private:
    bool connected_;
};

/**
 * @brief Tests pour InputManager
 */
class InputManagerTest {
public:
    void setUp() {
        // Configuration par défaut
        config_.enableEncoders = true;
        config_.enableButtons = true;
        config_.enableEventProcessing = true;
        
        inputManager_ = std::make_unique<InputManager>(config_);
        mockInputController_ = std::make_shared<InputController>();
        
        // Créer des définitions de contrôles de test
        createTestControlDefinitions();
    }

    void tearDown() {
        inputManager_.reset();
        mockInputController_.reset();
        testControlDefinitions_.clear();
    }

    /**
     * @brief Test constructeur avec configuration par défaut
     */
    void testConstructorDefaultConfig() {
        // Arrange & Act
        auto defaultManager = std::make_unique<InputManager>();
        
        // Assert
        TEST_ASSERT_NOT_NULL(defaultManager.get());
        TEST_ASSERT_FALSE(defaultManager->isOperational());
    }

    /**
     * @brief Test constructeur avec configuration personnalisée
     */
    void testConstructorCustomConfig() {
        // Arrange
        InputManager::ManagerConfig customConfig;
        customConfig.enableEncoders = false;
        customConfig.enableButtons = true;
        customConfig.enableEventProcessing = false;
        
        // Act
        auto customManager = std::make_unique<InputManager>(customConfig);
        
        // Assert
        TEST_ASSERT_NOT_NULL(customManager.get());
        TEST_ASSERT_FALSE(customManager->isOperational());
    }

    /**
     * @brief Test initialize avec définitions valides
     */
    void testInitializeSuccess() {
        // Arrange - utiliser des pointeurs factices pour éviter dépendances
        auto inputController = std::shared_ptr<InputController>(reinterpret_cast<InputController*>(0x1000), [](InputController*) {});
        
        // Act
        auto result = inputManager_->initialize(testControlDefinitions_, inputController);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(inputManager_->isOperational());
    }

    /**
     * @brief Test initialize sans contrôleur d'entrée
     */
    void testInitializeWithoutInputController() {
        // Act
        auto result = inputManager_->initialize(testControlDefinitions_, nullptr);
        
        // Assert - Devrait réussir même sans InputController
        TEST_ASSERT_TRUE(result.isSuccess());
    }

    /**
     * @brief Test initialize avec définitions vides
     */
    void testInitializeEmptyDefinitions() {
        // Arrange
        std::vector<ControlDefinition> emptyDefinitions;
        auto inputController = std::shared_ptr<InputController>(reinterpret_cast<InputController*>(0x1000), [](InputController*) {});
        
        // Act
        auto result = inputManager_->initialize(emptyDefinitions, inputController);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        // Opérationnel mais sans gestionnaires
        TEST_ASSERT_TRUE(inputManager_->isOperational());
        TEST_ASSERT_NULL(inputManager_->getEncoderManager().get());
        TEST_ASSERT_NULL(inputManager_->getButtonManager().get());
    }

    /**
     * @brief Test double initialisation
     */
    void testDoubleInitialization() {
        // Arrange
        auto inputController = std::shared_ptr<InputController>(reinterpret_cast<InputController*>(0x1000), [](InputController*) {});
        inputManager_->initialize(testControlDefinitions_, inputController);
        
        // Act - deuxième initialisation
        auto result = inputManager_->initialize(testControlDefinitions_, inputController);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(inputManager_->isOperational());
    }

    /**
     * @brief Test reconfigure avec nouvelles définitions
     */
    void testReconfigureSuccess() {
        // Arrange
        auto inputController = std::shared_ptr<InputController>(reinterpret_cast<InputController*>(0x1000), [](InputController*) {});
        inputManager_->initialize(testControlDefinitions_, inputController);
        
        // Créer nouvelles définitions
        std::vector<ControlDefinition> newDefinitions;
        createEncoderDefinition(newDefinitions, 200, 10, 11);
        
        // Act
        auto result = inputManager_->reconfigure(newDefinitions);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(inputManager_->isOperational());
    }

    /**
     * @brief Test reconfigure sans initialisation
     */
    void testReconfigureNotInitialized() {
        // Arrange
        std::vector<ControlDefinition> newDefinitions;
        
        // Act
        auto result = inputManager_->reconfigure(newDefinitions);
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::OperationFailed, result.error().value().code);
    }

    /**
     * @brief Test isOperational dans différents états
     */
    void testIsOperational() {
        // Test 1: Non initialisé
        TEST_ASSERT_FALSE(inputManager_->isOperational());
        
        // Test 2: Initialisé avec composants
        auto inputController = std::shared_ptr<InputController>(reinterpret_cast<InputController*>(0x1000), [](InputController*) {});
        inputManager_->initialize(testControlDefinitions_, inputController);
        TEST_ASSERT_TRUE(inputManager_->isOperational());
        
        // Test 3: Configuration minimale
        InputManager::ManagerConfig minimalConfig;
        minimalConfig.enableEncoders = false;
        minimalConfig.enableButtons = false;
        minimalConfig.enableEventProcessing = false;
        auto minimalManager = std::make_unique<InputManager>(minimalConfig);
        
        std::vector<ControlDefinition> emptyDefinitions;
        minimalManager->initialize(emptyDefinitions, inputController);
        TEST_ASSERT_TRUE(minimalManager->isOperational());
    }

    /**
     * @brief Test configuration avec encodeurs seulement
     */
    void testConfigurationEncodersOnly() {
        // Arrange
        InputManager::ManagerConfig encoderOnlyConfig;
        encoderOnlyConfig.enableEncoders = true;
        encoderOnlyConfig.enableButtons = false;
        encoderOnlyConfig.enableEventProcessing = true;
        
        auto encoderOnlyManager = std::make_unique<InputManager>(encoderOnlyConfig);
        
        std::vector<ControlDefinition> encoderDefinitions;
        createEncoderDefinition(encoderDefinitions, 100, 2, 3);
        
        auto inputController = std::shared_ptr<InputController>(reinterpret_cast<InputController*>(0x1000), [](InputController*) {});
        
        // Act
        auto result = encoderOnlyManager->initialize(encoderDefinitions, inputController);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(encoderOnlyManager->isOperational());
        TEST_ASSERT_NOT_NULL(encoderOnlyManager->getEncoderManager().get());
        TEST_ASSERT_NULL(encoderOnlyManager->getButtonManager().get());
    }

    /**
     * @brief Test configuration avec boutons seulement
     */
    void testConfigurationButtonsOnly() {
        // Arrange
        InputManager::ManagerConfig buttonOnlyConfig;
        buttonOnlyConfig.enableEncoders = false;
        buttonOnlyConfig.enableButtons = true;
        buttonOnlyConfig.enableEventProcessing = true;
        
        auto buttonOnlyManager = std::make_unique<InputManager>(buttonOnlyConfig);
        
        std::vector<ControlDefinition> buttonDefinitions;
        createButtonDefinition(buttonDefinitions, 101, 5);
        
        auto inputController = std::shared_ptr<InputController>(reinterpret_cast<InputController*>(0x1000), [](InputController*) {});
        
        // Act
        auto result = buttonOnlyManager->initialize(buttonDefinitions, inputController);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(buttonOnlyManager->isOperational());
        TEST_ASSERT_NULL(buttonOnlyManager->getEncoderManager().get());
        TEST_ASSERT_NOT_NULL(buttonOnlyManager->getButtonManager().get());
    }

private:
    InputManager::ManagerConfig config_;
    std::unique_ptr<InputManager> inputManager_;
    std::shared_ptr<InputController> mockInputController_;
    std::vector<ControlDefinition> testControlDefinitions_;

    void createTestControlDefinitions() {
        testControlDefinitions_.clear();
        
        // Ajouter un encodeur de test
        createEncoderDefinition(testControlDefinitions_, 100, 2, 3);
        
        // Ajouter un bouton de test
        createButtonDefinition(testControlDefinitions_, 101, 4);
    }

    void createEncoderDefinition(std::vector<ControlDefinition>& definitions, InputId id, uint8_t pinA, uint8_t pinB) {
        ControlDefinition encoderDef;
        encoderDef.id = id;
        encoderDef.enabled = true;
        encoderDef.hardware.type = InputType::ENCODER;
        
        ControlDefinition::EncoderConfig encConfig;
        encConfig.pinA = pinA;
        encConfig.pinB = pinB;
        encConfig.ppr = 24;
        encConfig.sensitivity = 1.0f;
        encConfig.enableAcceleration = false;
        encConfig.stepsPerDetent = 4;
        
        encoderDef.hardware.config = encConfig;
        definitions.push_back(encoderDef);
    }

    void createButtonDefinition(std::vector<ControlDefinition>& definitions, InputId id, uint8_t pin) {
        ControlDefinition buttonDef;
        buttonDef.id = id;
        buttonDef.enabled = true;
        buttonDef.hardware.type = InputType::BUTTON;
        
        ControlDefinition::ButtonConfig btnConfig;
        btnConfig.pin = pin;
        btnConfig.activeLow = true;
        btnConfig.mode = ButtonMode::PULLUP;
        btnConfig.debounceMs = 50;
        btnConfig.longPressMs = std::nullopt;
        
        buttonDef.hardware.config = btnConfig;
        definitions.push_back(buttonDef);
    }
};

// === TESTS GLOBAUX ===

InputManagerTest* inputManagerTestInstance = nullptr;

void setUpInputManager(void) {
    inputManagerTestInstance = new InputManagerTest();
    inputManagerTestInstance->setUp();
}

void tearDownInputManager(void) {
    if (inputManagerTestInstance) {
        inputManagerTestInstance->tearDown();
        delete inputManagerTestInstance;
        inputManagerTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_input_manager_constructor_default_config() {
    setUpInputManager();
    inputManagerTestInstance->testConstructorDefaultConfig();
    tearDownInputManager();
}

void test_input_manager_constructor_custom_config() {
    setUpInputManager();
    inputManagerTestInstance->testConstructorCustomConfig();
    tearDownInputManager();
}

void test_input_manager_initialize_success() {
    setUpInputManager();
    inputManagerTestInstance->testInitializeSuccess();
    tearDownInputManager();
}

void test_input_manager_initialize_without_input_controller() {
    setUpInputManager();
    inputManagerTestInstance->testInitializeWithoutInputController();
    tearDownInputManager();
}

void test_input_manager_initialize_empty_definitions() {
    setUpInputManager();
    inputManagerTestInstance->testInitializeEmptyDefinitions();
    tearDownInputManager();
}

void test_input_manager_double_initialization() {
    setUpInputManager();
    inputManagerTestInstance->testDoubleInitialization();
    tearDownInputManager();
}

void test_input_manager_reconfigure_success() {
    setUpInputManager();
    inputManagerTestInstance->testReconfigureSuccess();
    tearDownInputManager();
}

void test_input_manager_reconfigure_not_initialized() {
    setUpInputManager();
    inputManagerTestInstance->testReconfigureNotInitialized();
    tearDownInputManager();
}

void test_input_manager_is_operational() {
    setUpInputManager();
    inputManagerTestInstance->testIsOperational();
    tearDownInputManager();
}

void test_input_manager_configuration_encoders_only() {
    setUpInputManager();
    inputManagerTestInstance->testConfigurationEncodersOnly();
    tearDownInputManager();
}

void test_input_manager_configuration_buttons_only() {
    setUpInputManager();
    inputManagerTestInstance->testConfigurationButtonsOnly();
    tearDownInputManager();
}