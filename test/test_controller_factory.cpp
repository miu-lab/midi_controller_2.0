#include <unity.h>
#include <memory>

#include "core/factories/ControllerFactory.hpp"
#include "app/di/DependencyContainer.hpp"

/**
 * @brief Mock NavigationConfigService pour les tests
 */
class MockNavigationConfigServiceForFactory {
public:
    MockNavigationConfigServiceForFactory() : operational_(true) {}
    
    void setOperational(bool operational) { operational_ = operational; }
    bool isOperational() const { return operational_; }

private:
    bool operational_;
};

/**
 * @brief Tests pour ControllerFactory
 */
class ControllerFactoryTest {
public:
    void setUp() {
        container_ = std::make_shared<DependencyContainer>();
        
        // Configuration par défaut
        config_.validateDependencies = true;
        config_.autoRegisterInContainer = true;
        
        factory_ = std::make_unique<ControllerFactory>(container_, config_);
        
        // Créer et enregistrer un mock NavigationConfigService
        mockNavigationConfigService_ = std::make_shared<MockNavigationConfigServiceForFactory>();
        // Note: Dans les vrais tests, on utiliserait l'interface NavigationConfigService
        container_->registerDependency<NavigationConfigService>(
            std::reinterpret_pointer_cast<NavigationConfigService>(
                std::shared_ptr<void>(reinterpret_cast<void*>(0x1000), [](void*) {})
            )
        );
    }

    void tearDown() {
        factory_.reset();
        mockNavigationConfigService_.reset();
        container_.reset();
    }

    /**
     * @brief Test constructeur avec conteneur valide
     */
    void testConstructorValidContainer() {
        // Arrange & Act
        auto testFactory = std::make_unique<ControllerFactory>(container_);
        
        // Assert
        TEST_ASSERT_NOT_NULL(testFactory.get());
        TEST_ASSERT_TRUE(testFactory->isOperational());
    }

    /**
     * @brief Test constructeur avec conteneur null
     */
    void testConstructorNullContainer() {
        // Arrange & Act
        auto testFactory = std::make_unique<ControllerFactory>(nullptr);
        
        // Assert
        TEST_ASSERT_NOT_NULL(testFactory.get());
        TEST_ASSERT_FALSE(testFactory->isOperational());
    }

    /**
     * @brief Test constructeur avec configuration personnalisée
     */
    void testConstructorCustomConfig() {
        // Arrange
        ControllerFactory::FactoryConfig customConfig;
        customConfig.validateDependencies = false;
        customConfig.autoRegisterInContainer = false;
        
        // Act
        auto testFactory = std::make_unique<ControllerFactory>(container_, customConfig);
        
        // Assert
        TEST_ASSERT_NOT_NULL(testFactory.get());
        TEST_ASSERT_TRUE(testFactory->isOperational());
    }

    /**
     * @brief Test createInputController avec dépendances valides
     */
    void testCreateInputControllerSuccess() {
        // Act
        auto result = factory_->createInputController();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_NOT_NULL(result.value().value().get());
    }

    /**
     * @brief Test createInputController sans conteneur
     */
    void testCreateInputControllerNoContainer() {
        // Arrange
        auto factoryWithoutContainer = std::make_unique<ControllerFactory>(nullptr);
        
        // Act
        auto result = factoryWithoutContainer->createInputController();
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, result.error().value().code);
    }

    /**
     * @brief Test createInputController sans NavigationConfigService
     */
    void testCreateInputControllerMissingNavigationConfig() {
        // Arrange
        auto containerWithoutNavConfig = std::make_shared<DependencyContainer>();
        auto factoryWithoutNavConfig = std::make_unique<ControllerFactory>(containerWithoutNavConfig);
        
        // Act
        auto result = factoryWithoutNavConfig->createInputController();
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, result.error().value().code);
    }

    /**
     * @brief Test validateDependencies avec dépendances présentes
     */
    void testValidateDependenciesSuccess() {
        // Act
        auto result = factory_->validateDependencies();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
    }

    /**
     * @brief Test validateDependencies sans conteneur
     */
    void testValidateDependenciesNoContainer() {
        // Arrange
        auto factoryWithoutContainer = std::make_unique<ControllerFactory>(nullptr);
        
        // Act
        auto result = factoryWithoutContainer->validateDependencies();
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, result.error().value().code);
    }

    /**
     * @brief Test validateDependencies sans NavigationConfigService
     */
    void testValidateDependenciesMissingNavigationConfig() {
        // Arrange
        auto containerWithoutNavConfig = std::make_shared<DependencyContainer>();
        auto factoryWithoutNavConfig = std::make_unique<ControllerFactory>(containerWithoutNavConfig);
        
        // Act
        auto result = factoryWithoutNavConfig->validateDependencies();
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, result.error().value().code);
    }

    /**
     * @brief Test isOperational dans différents états
     */
    void testIsOperational() {
        // Test 1: Avec conteneur valide
        TEST_ASSERT_TRUE(factory_->isOperational());
        
        // Test 2: Sans conteneur
        auto factoryWithoutContainer = std::make_unique<ControllerFactory>(nullptr);
        TEST_ASSERT_FALSE(factoryWithoutContainer->isOperational());
    }

    /**
     * @brief Test configuration sans validation des dépendances
     */
    void testConfigurationWithoutValidation() {
        // Arrange
        ControllerFactory::FactoryConfig noValidationConfig;
        noValidationConfig.validateDependencies = false;
        noValidationConfig.autoRegisterInContainer = true;
        
        auto factoryNoValidation = std::make_unique<ControllerFactory>(container_, noValidationConfig);
        
        // Act
        auto result = factoryNoValidation->createInputController();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_NOT_NULL(result.value().value().get());
    }

    /**
     * @brief Test configuration sans enregistrement automatique
     */
    void testConfigurationWithoutAutoRegister() {
        // Arrange
        ControllerFactory::FactoryConfig noAutoRegisterConfig;
        noAutoRegisterConfig.validateDependencies = true;
        noAutoRegisterConfig.autoRegisterInContainer = false;
        
        auto factoryNoAutoRegister = std::make_unique<ControllerFactory>(container_, noAutoRegisterConfig);
        
        // Act
        auto result = factoryNoAutoRegister->createInputController();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_NOT_NULL(result.value().value().get());
        
        // Vérifier que le contrôleur n'a pas été enregistré automatiquement
        // (cette vérification dépendrait de l'implémentation de DependencyContainer)
    }

private:
    std::shared_ptr<DependencyContainer> container_;
    std::unique_ptr<ControllerFactory> factory_;
    std::shared_ptr<MockNavigationConfigServiceForFactory> mockNavigationConfigService_;
    ControllerFactory::FactoryConfig config_;
};

// === TESTS GLOBAUX ===

ControllerFactoryTest* controllerFactoryTestInstance = nullptr;

void setUpControllerFactory(void) {
    controllerFactoryTestInstance = new ControllerFactoryTest();
    controllerFactoryTestInstance->setUp();
}

void tearDownControllerFactory(void) {
    if (controllerFactoryTestInstance) {
        controllerFactoryTestInstance->tearDown();
        delete controllerFactoryTestInstance;
        controllerFactoryTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_controller_factory_constructor_valid_container() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testConstructorValidContainer();
    tearDownControllerFactory();
}

void test_controller_factory_constructor_null_container() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testConstructorNullContainer();
    tearDownControllerFactory();
}

void test_controller_factory_constructor_custom_config() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testConstructorCustomConfig();
    tearDownControllerFactory();
}

void test_controller_factory_create_input_controller_success() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testCreateInputControllerSuccess();
    tearDownControllerFactory();
}

void test_controller_factory_create_input_controller_no_container() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testCreateInputControllerNoContainer();
    tearDownControllerFactory();
}

void test_controller_factory_create_input_controller_missing_navigation_config() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testCreateInputControllerMissingNavigationConfig();
    tearDownControllerFactory();
}

void test_controller_factory_validate_dependencies_success() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testValidateDependenciesSuccess();
    tearDownControllerFactory();
}

void test_controller_factory_validate_dependencies_no_container() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testValidateDependenciesNoContainer();
    tearDownControllerFactory();
}

void test_controller_factory_validate_dependencies_missing_navigation_config() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testValidateDependenciesMissingNavigationConfig();
    tearDownControllerFactory();
}

void test_controller_factory_is_operational() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testIsOperational();
    tearDownControllerFactory();
}

void test_controller_factory_configuration_without_validation() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testConfigurationWithoutValidation();
    tearDownControllerFactory();
}

void test_controller_factory_configuration_without_auto_register() {
    setUpControllerFactory();
    controllerFactoryTestInstance->testConfigurationWithoutAutoRegister();
    tearDownControllerFactory();
}