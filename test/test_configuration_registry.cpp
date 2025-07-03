#include <unity.h>
#include <memory>

#include "core/configuration/ConfigurationRegistry.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "mocks/MockConfiguration.hpp"

/**
 * @brief Tests pour ConfigurationRegistry
 * 
 * Test l'enregistrement des configurations dans le conteneur DI.
 */
class ConfigurationRegistryTest {
public:
    void setUp() {
        container_ = std::make_shared<DependencyContainer>();
        registry_ = std::make_unique<ConfigurationRegistry>(container_);
    }

    void tearDown() {
        registry_.reset();
        container_.reset();
    }

    /**
     * @brief Test registerConfigurationSubsystem avec objet valide
     */
    void testRegisterConfigurationSubsystemValid() {
        // Arrange
        auto mockSubsystem = std::make_shared<MockConfiguration>(
            MockConfiguration::ConfigurationType::MINIMAL);
        
        // Act
        bool result = registry_->registerConfigurationSubsystem(
            std::static_pointer_cast<void>(mockSubsystem));
        
        // Assert
        TEST_ASSERT_TRUE(result);
        
        // Vérifier que l'enregistrement a fonctionné
        auto retrieved = container_->resolve<IConfiguration>();
        TEST_ASSERT_NOT_NULL(retrieved.get());
    }

    /**
     * @brief Test registerConfigurationSubsystem avec pointeur null
     */
    void testRegisterConfigurationSubsystemNull() {
        // Arrange
        std::shared_ptr<void> nullSubsystem = nullptr;
        
        // Act
        bool result = registry_->registerConfigurationSubsystem(nullSubsystem);
        
        // Assert
        TEST_ASSERT_FALSE(result);
    }

    /**
     * @brief Test registerUnifiedConfiguration avec configuration valide
     */
    void testRegisterUnifiedConfigurationValid() {
        // Arrange
        auto appConfig = std::make_shared<ApplicationConfiguration>();
        
        // Act
        bool result = registry_->registerUnifiedConfiguration(appConfig);
        
        // Assert
        TEST_ASSERT_TRUE(result);
        
        // Vérifier que l'enregistrement a fonctionné
        auto retrieved = container_->resolve<UnifiedConfiguration>();
        TEST_ASSERT_NOT_NULL(retrieved.get());
    }

    /**
     * @brief Test registerUnifiedConfiguration avec pointeur null
     */
    void testRegisterUnifiedConfigurationNull() {
        // Arrange
        std::shared_ptr<ApplicationConfiguration> nullConfig = nullptr;
        
        // Act
        bool result = registry_->registerUnifiedConfiguration(nullConfig);
        
        // Assert
        TEST_ASSERT_FALSE(result);
    }

    /**
     * @brief Test registry avec conteneur null
     */
    void testRegistryWithNullContainer() {
        // Arrange
        auto nullRegistry = std::make_unique<ConfigurationRegistry>(nullptr);
        auto mockSubsystem = std::make_shared<MockConfiguration>(
            MockConfiguration::ConfigurationType::MINIMAL);
        
        // Act
        bool result = nullRegistry->registerConfigurationSubsystem(
            std::static_pointer_cast<void>(mockSubsystem));
        
        // Assert
        TEST_ASSERT_FALSE(result);
    }

    /**
     * @brief Test enregistrement multiple du même type
     */
    void testMultipleRegistrations() {
        // Arrange
        auto mockSubsystem1 = std::make_shared<MockConfiguration>(
            MockConfiguration::ConfigurationType::MINIMAL);
        auto mockSubsystem2 = std::make_shared<MockConfiguration>(
            MockConfiguration::ConfigurationType::COMPLEX);
        
        // Act
        bool result1 = registry_->registerConfigurationSubsystem(
            std::static_pointer_cast<void>(mockSubsystem1));
        bool result2 = registry_->registerConfigurationSubsystem(
            std::static_pointer_cast<void>(mockSubsystem2));
        
        // Assert
        TEST_ASSERT_TRUE(result1);
        TEST_ASSERT_TRUE(result2);
        
        // Le dernier enregistré devrait être accessible
        auto retrieved = container_->resolve<IConfiguration>();
        TEST_ASSERT_NOT_NULL(retrieved.get());
    }

private:
    std::shared_ptr<DependencyContainer> container_;
    std::unique_ptr<ConfigurationRegistry> registry_;
};

// === TESTS GLOBAUX ===

ConfigurationRegistryTest* configRegistryTestInstance = nullptr;

void setUpConfigRegistry(void) {
    configRegistryTestInstance = new ConfigurationRegistryTest();
    configRegistryTestInstance->setUp();
}

void tearDownConfigRegistry(void) {
    if (configRegistryTestInstance) {
        configRegistryTestInstance->tearDown();
        delete configRegistryTestInstance;
        configRegistryTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_config_registry_register_subsystem_valid() {
    setUpConfigRegistry();
    configRegistryTestInstance->testRegisterConfigurationSubsystemValid();
    tearDownConfigRegistry();
}

void test_config_registry_register_subsystem_null() {
    setUpConfigRegistry();
    configRegistryTestInstance->testRegisterConfigurationSubsystemNull();
    tearDownConfigRegistry();
}

void test_config_registry_register_unified_config_valid() {
    setUpConfigRegistry();
    configRegistryTestInstance->testRegisterUnifiedConfigurationValid();
    tearDownConfigRegistry();
}

void test_config_registry_register_unified_config_null() {
    setUpConfigRegistry();
    configRegistryTestInstance->testRegisterUnifiedConfigurationNull();
    tearDownConfigRegistry();
}

void test_config_registry_with_null_container() {
    setUpConfigRegistry();
    configRegistryTestInstance->testRegistryWithNullContainer();
    tearDownConfigRegistry();
}

void test_config_registry_multiple_registrations() {
    setUpConfigRegistry();
    configRegistryTestInstance->testMultipleRegistrations();
    tearDownConfigRegistry();
}