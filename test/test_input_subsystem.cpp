#include <unity.h>
#include <memory>

#include "app/subsystems/InputSubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "core/controllers/InputController.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "mocks/MockConfiguration.hpp"
#include "fixtures/test_configurations.hpp"

/**
 * @brief Mock simple pour InputController
 * 
 * Mock minimal pour isoler les tests d'InputSubsystem
 * sans dépendre de la logique complète d'InputController.
 */
class MockInputController {
public:
    MockInputController() = default;
    ~MockInputController() = default;

    // Mock methods pour les tests
    void processEncoderTurn(EncoderId id, int32_t absolutePosition, int8_t relativeChange) {
        encoder_calls_++;
        last_encoder_id_ = id;
        last_absolute_position_ = absolutePosition;
        last_relative_change_ = relativeChange;
    }

    void processButtonPress(ButtonId id, bool pressed) {
        button_calls_++;
        last_button_id_ = id;
        last_button_pressed_ = pressed;
    }

    // Méthodes de test
    void reset() {
        encoder_calls_ = 0;
        button_calls_ = 0;
        last_encoder_id_ = 0;
        last_button_id_ = 0;
        last_absolute_position_ = 0;
        last_relative_change_ = 0;
        last_button_pressed_ = false;
    }

    size_t getEncoderCallCount() const { return encoder_calls_; }
    size_t getButtonCallCount() const { return button_calls_; }
    EncoderId getLastEncoderId() const { return last_encoder_id_; }
    ButtonId getLastButtonId() const { return last_button_id_; }

private:
    size_t encoder_calls_ = 0;
    size_t button_calls_ = 0;
    EncoderId last_encoder_id_ = 0;
    ButtonId last_button_id_ = 0;
    int32_t last_absolute_position_ = 0;
    int8_t last_relative_change_ = 0;
    bool last_button_pressed_ = false;
};

/**
 * @brief Tests pour InputSubsystem::init()
 * 
 * Test la logique d'initialisation du sous-système d'entrée
 * avec différents scénarios de dépendances et configurations.
 */
class InputSubsystemTest {
public:
    void setUp() {
        container_ = std::make_shared<DependencyContainer>();
        mockInputController_ = std::make_shared<MockInputController>();
    }

    void tearDown() {
        container_.reset();
        mockInputController_.reset();
    }

    /**
     * @brief Test init() avec dépendances manquantes
     * 
     * Vérifie que l'initialisation échoue gracieusement quand
     * IConfiguration n'est pas disponible dans le conteneur.
     */
    void testInitWithMissingConfiguration() {
        // Arrange - conteneur vide (pas d'IConfiguration)
        auto inputSubsystem = std::make_shared<InputSubsystem>(container_);
        
        // Act
        auto result = inputSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, errorOpt->code);
    }

    /**
     * @brief Test init() avec configuration vide
     * 
     * Vérifie que l'initialisation échoue quand la configuration
     * ne contient aucune définition de contrôle.
     */
    void testInitWithEmptyConfiguration() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::EMPTY);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto inputSubsystem = std::make_shared<InputSubsystem>(container_);
        
        // Act
        auto result = inputSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::ConfigError, errorOpt->code);
    }

    /**
     * @brief Test init() avec InputController manquant
     * 
     * Vérifie que l'initialisation échoue quand InputController
     * n'est pas disponible dans le conteneur.
     */
    void testInitWithMissingInputController() {
        // Arrange - configuration valide mais pas d'InputController
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto inputSubsystem = std::make_shared<InputSubsystem>(container_);
        
        // Act
        auto result = inputSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, errorOpt->code);
    }

    /**
     * @brief Test init() avec toutes les dépendances disponibles
     * 
     * Vérifie que l'initialisation réussit avec une configuration
     * minimale et toutes les dépendances requises.
     */
    void testInitWithValidDependencies() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        // Note: On utilise un cast vers InputController pour le mock
        // Ceci fonctionne pour les tests car on ne fait que vérifier l'initialisation
        auto mockInputControllerPtr = std::reinterpret_pointer_cast<InputController>(
            std::static_pointer_cast<void>(mockInputController_));
        container_->registerDependency<InputController>(mockInputControllerPtr);
        
        auto inputSubsystem = std::make_shared<InputSubsystem>(container_);
        
        // Act
        auto result = inputSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(result.value().value_or(false));
        
        // Vérifier que IInputSystem a été enregistrée
        auto iInputSystem = container_->resolve<IInputSystem>();
        TEST_ASSERT_NOT_NULL(iInputSystem);
    }

    /**
     * @brief Test double initialisation
     * 
     * Vérifie que l'initialisation multiple retourne succès
     * sans recréer les composants.
     */
    void testDoubleInitialization() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto mockInputControllerPtr = std::reinterpret_pointer_cast<InputController>(
            std::static_pointer_cast<void>(mockInputController_));
        container_->registerDependency<InputController>(mockInputControllerPtr);
        
        auto inputSubsystem = std::make_shared<InputSubsystem>(container_);
        
        // Act - première initialisation
        auto result1 = inputSubsystem->init();
        
        // Act - deuxième initialisation
        auto result2 = inputSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result1.isSuccess());
        TEST_ASSERT_TRUE(result2.isSuccess());
        TEST_ASSERT_TRUE(result1.value().value_or(false));
        TEST_ASSERT_TRUE(result2.value().value_or(false));
    }

    /**
     * @brief Test validation du statut après init()
     * 
     * Vérifie que les méthodes de validation fonctionnent
     * correctement après une initialisation réussie.
     */
    void testValidationAfterInit() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto mockInputControllerPtr = std::reinterpret_pointer_cast<InputController>(
            std::static_pointer_cast<void>(mockInputController_));
        container_->registerDependency<InputController>(mockInputControllerPtr);
        
        auto inputSubsystem = std::make_shared<InputSubsystem>(container_);
        
        // Act
        auto result = inputSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        
        // Vérifier les méthodes de validation
        TEST_ASSERT_TRUE(inputSubsystem->validateInputsStatus());
        
        // Vérifier les méthodes de consultation
        auto activeControls = inputSubsystem->getAllActiveControlDefinitions();
        TEST_ASSERT_TRUE(activeControls.size() > 0);
        
        // Vérifier le comptage par type
        auto encoderCount = inputSubsystem->getActiveInputCountByType(InputType::ENCODER);
        auto buttonCount = inputSubsystem->getActiveInputCountByType(InputType::BUTTON);
        TEST_ASSERT_TRUE(encoderCount > 0);
        TEST_ASSERT_TRUE(buttonCount > 0);
    }

    /**
     * @brief Test configuration complexe
     * 
     * Vérifie que l'initialisation fonctionne avec une configuration
     * plus complexe contenant plusieurs encodeurs et boutons.
     */
    void testInitWithComplexConfiguration() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::COMPLEX);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto mockInputControllerPtr = std::reinterpret_pointer_cast<InputController>(
            std::static_pointer_cast<void>(mockInputController_));
        container_->registerDependency<InputController>(mockInputControllerPtr);
        
        auto inputSubsystem = std::make_shared<InputSubsystem>(container_);
        
        // Act
        auto result = inputSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        
        // Vérifier que plusieurs contrôles sont configurés
        auto activeControls = inputSubsystem->getAllActiveControlDefinitions();
        TEST_ASSERT_TRUE(activeControls.size() >= 8); // 4 encodeurs + 4 boutons
        
        // Vérifier les comptages par type
        auto encoderCount = inputSubsystem->getActiveInputCountByType(InputType::ENCODER);
        auto buttonCount = inputSubsystem->getActiveInputCountByType(InputType::BUTTON);
        TEST_ASSERT_EQUAL(4, encoderCount);
        TEST_ASSERT_EQUAL(4, buttonCount);
    }

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<MockInputController> mockInputController_;
};

// === TESTS GLOBAUX ===

InputSubsystemTest* inputTestInstance = nullptr;

void setUpInput(void) {
    inputTestInstance = new InputSubsystemTest();
    inputTestInstance->setUp();
}

void tearDownInput(void) {
    if (inputTestInstance) {
        inputTestInstance->tearDown();
        delete inputTestInstance;
        inputTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_input_init_with_missing_configuration() {
    setUpInput();
    inputTestInstance->testInitWithMissingConfiguration();
    tearDownInput();
}

void test_input_init_with_empty_configuration() {
    setUpInput();
    inputTestInstance->testInitWithEmptyConfiguration();
    tearDownInput();
}

void test_input_init_with_missing_input_controller() {
    setUpInput();
    inputTestInstance->testInitWithMissingInputController();
    tearDownInput();
}

void test_input_init_with_valid_dependencies() {
    setUpInput();
    inputTestInstance->testInitWithValidDependencies();
    tearDownInput();
}

void test_input_double_initialization() {
    setUpInput();
    inputTestInstance->testDoubleInitialization();
    tearDownInput();
}

void test_input_validation_after_init() {
    setUpInput();
    inputTestInstance->testValidationAfterInit();
    tearDownInput();
}

void test_input_init_with_complex_configuration() {
    setUpInput();
    inputTestInstance->testInitWithComplexConfiguration();
    tearDownInput();
}