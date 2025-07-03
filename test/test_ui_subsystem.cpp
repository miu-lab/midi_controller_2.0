#include <unity.h>
#include <memory>

#include "app/subsystems/UISubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "mocks/MockConfiguration.hpp"
#include "mocks/MockUIComponents.hpp"
#include "mocks/MockEventBus.hpp"
#include "fixtures/test_configurations.hpp"

/**
 * @brief Tests pour UISubsystem::init()
 * 
 * Test la logique d'initialisation du sous-système UI
 * avec différents scénarios de dépendances et configurations.
 */
class UISubsystemTest {
public:
    void setUp() {
        container_ = std::make_shared<DependencyContainer>();
        mockBridge_ = std::make_shared<MockIli9341LvglBridge>();
        mockEventBus_ = std::make_shared<MockEventBus>();
    }

    void tearDown() {
        container_.reset();
        mockBridge_.reset();
        mockEventBus_.reset();
    }

    /**
     * @brief Test init() avec dépendances manquantes
     * 
     * Vérifie que l'initialisation échoue gracieusement quand
     * IConfiguration n'est pas disponible dans le conteneur.
     */
    void testInitWithMissingConfiguration() {
        // Arrange - conteneur vide (pas d'IConfiguration)
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        
        // Act
        auto result = uiSubsystem->init(false);
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, errorOpt->code);
    }

    /**
     * @brief Test init() avec UI simple (enableFullUI = false)
     * 
     * Vérifie que l'initialisation réussit avec une configuration
     * minimale sans interface complète.
     */
    void testInitWithSimpleUI() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        // Note: Pour UI simple, pas besoin de ViewManager etc.
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        
        // Act
        auto result = uiSubsystem->init(false);  // UI simple
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(result.value().value_or(false));
    }

    /**
     * @brief Test init() avec UI complète mais dépendances manquantes
     * 
     * Vérifie que l'initialisation échoue quand les dépendances LVGL
     * ne sont pas disponibles pour l'UI complète.
     */
    void testInitWithFullUIButMissingDependencies() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        // Pas de bridge LVGL, UnifiedConfiguration, ou EventBus
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        
        // Act
        auto result = uiSubsystem->init(true);  // UI complète
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, errorOpt->code);
    }

    /**
     * @brief Test init() avec UI complète et toutes les dépendances
     * 
     * Vérifie que l'initialisation réussit avec une configuration
     * complète et toutes les dépendances requises.
     */
    void testInitWithFullUIAndValidDependencies() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        // Enregistrer les dépendances LVGL nécessaires
        auto mockBridgePtr = std::reinterpret_pointer_cast<Ili9341LvglBridge>(
            std::static_pointer_cast<void>(mockBridge_));
        container_->registerDependency<Ili9341LvglBridge>(mockBridgePtr);
        
        auto mockEventBusPtr = std::reinterpret_pointer_cast<EventBus>(
            std::static_pointer_cast<void>(mockEventBus_));
        container_->registerDependency<EventBus>(mockEventBusPtr);
        
        // Pour UnifiedConfiguration, créer un mock simple
        auto unifiedConfig = std::make_shared<UnifiedConfiguration>();
        container_->registerDependency<UnifiedConfiguration>(unifiedConfig);
        
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        
        // Act
        auto result = uiSubsystem->init(true);  // UI complète
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(result.value().value_or(false));
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
        
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        
        // Act - première initialisation
        auto result1 = uiSubsystem->init(false);
        
        // Act - deuxième initialisation
        auto result2 = uiSubsystem->init(false);
        
        // Assert
        TEST_ASSERT_TRUE(result1.isSuccess());
        TEST_ASSERT_TRUE(result2.isSuccess());
        TEST_ASSERT_TRUE(result1.value().value_or(false));
        TEST_ASSERT_TRUE(result2.value().value_or(false));
    }

    /**
     * @brief Test showMessage() sans initialisation
     * 
     * Vérifie que showMessage() échoue si le système n'est pas initialisé.
     */
    void testShowMessageWithoutInitialization() {
        // Arrange
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        
        // Act
        auto result = uiSubsystem->showMessage("Test message");
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::OperationFailed, errorOpt->code);
    }

    /**
     * @brief Test clearDisplay() sans initialisation
     * 
     * Vérifie que clearDisplay() échoue si le système n'est pas initialisé.
     */
    void testClearDisplayWithoutInitialization() {
        // Arrange
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        
        // Act
        auto result = uiSubsystem->clearDisplay();
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::OperationFailed, errorOpt->code);
    }

    /**
     * @brief Test update() avec UI simple
     * 
     * Vérifie que update() ne fait rien avec UI simple
     * mais ne provoque pas d'erreur.
     */
    void testUpdateWithSimpleUI() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        uiSubsystem->init(false);
        
        // Act - update() ne devrait rien faire mais ne pas crasher
        uiSubsystem->update();
        uiSubsystem->update();
        uiSubsystem->update();
        
        // Assert - pas de crash, c'est déjà un succès
        TEST_ASSERT_TRUE(true);
    }

    /**
     * @brief Test configuration complexe
     * 
     * Vérifie que l'initialisation fonctionne avec une configuration
     * plus complexe contenant plusieurs contrôles.
     */
    void testInitWithComplexConfiguration() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::COMPLEX);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto uiSubsystem = std::make_shared<UISubsystem>(container_);
        
        // Act
        auto result = uiSubsystem->init(false);  // UI simple avec config complexe
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(result.value().value_or(false));
    }

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<MockIli9341LvglBridge> mockBridge_;
    std::shared_ptr<MockEventBus> mockEventBus_;
};

// === TESTS GLOBAUX ===

UISubsystemTest* uiTestInstance = nullptr;

void setUpUI(void) {
    uiTestInstance = new UISubsystemTest();
    uiTestInstance->setUp();
}

void tearDownUI(void) {
    if (uiTestInstance) {
        uiTestInstance->tearDown();
        delete uiTestInstance;
        uiTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_ui_init_with_missing_configuration() {
    setUpUI();
    uiTestInstance->testInitWithMissingConfiguration();
    tearDownUI();
}

void test_ui_init_with_simple_ui() {
    setUpUI();
    uiTestInstance->testInitWithSimpleUI();
    tearDownUI();
}

void test_ui_init_with_full_ui_but_missing_dependencies() {
    setUpUI();
    uiTestInstance->testInitWithFullUIButMissingDependencies();
    tearDownUI();
}

void test_ui_init_with_full_ui_and_valid_dependencies() {
    setUpUI();
    uiTestInstance->testInitWithFullUIAndValidDependencies();
    tearDownUI();
}

void test_ui_double_initialization() {
    setUpUI();
    uiTestInstance->testDoubleInitialization();
    tearDownUI();
}

void test_ui_show_message_without_initialization() {
    setUpUI();
    uiTestInstance->testShowMessageWithoutInitialization();
    tearDownUI();
}

void test_ui_clear_display_without_initialization() {
    setUpUI();
    uiTestInstance->testClearDisplayWithoutInitialization();
    tearDownUI();
}

void test_ui_update_with_simple_ui() {
    setUpUI();
    uiTestInstance->testUpdateWithSimpleUI();
    tearDownUI();
}

void test_ui_init_with_complex_configuration() {
    setUpUI();
    uiTestInstance->testInitWithComplexConfiguration();
    tearDownUI();
}