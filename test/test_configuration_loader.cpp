#include <unity.h>
#include <memory>

#include "core/configuration/ConfigurationLoader.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/utils/Error.hpp"

/**
 * @brief Tests pour ConfigurationLoader
 * 
 * Test la logique de chargement et validation des configurations
 * extraite de ConfigurationSubsystem.
 */
class ConfigurationLoaderTest {
public:
    void setUp() {
        loader_ = std::make_unique<ConfigurationLoader>();
    }

    void tearDown() {
        loader_.reset();
    }

    /**
     * @brief Test loadUnifiedConfigurations() avec configuration null
     */
    void testLoadWithNullConfiguration() {
        // Arrange - configuration nulle
        std::shared_ptr<ApplicationConfiguration> nullConfig = nullptr;
        
        // Act
        auto result = loader_->loadUnifiedConfigurations(nullConfig);
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, errorOpt->code);
    }

    /**
     * @brief Test loadUnifiedConfigurations() avec configuration valide
     */
    void testLoadWithValidConfiguration() {
        // Arrange
        auto appConfig = std::make_shared<ApplicationConfiguration>();
        
        // Act
        auto result = loader_->loadUnifiedConfigurations(appConfig);
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(result.value().value_or(false));
    }

    /**
     * @brief Test validateConfigurations() sans chargement préalable
     */
    void testValidateWithoutLoading() {
        // Arrange - pas de chargement
        
        // Act
        auto result = loader_->validateConfigurations();
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::OperationFailed, errorOpt->code);
    }

    /**
     * @brief Test validateConfigurations() après chargement valide
     */
    void testValidateAfterValidLoading() {
        // Arrange
        auto appConfig = std::make_shared<ApplicationConfiguration>();
        loader_->loadUnifiedConfigurations(appConfig);
        
        // Act
        auto result = loader_->validateConfigurations();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(result.value().value_or(false));
    }

    /**
     * @brief Test séquence complète load -> validate
     */
    void testCompleteLoadAndValidateSequence() {
        // Arrange
        auto appConfig = std::make_shared<ApplicationConfiguration>();
        
        // Act - Charger
        auto loadResult = loader_->loadUnifiedConfigurations(appConfig);
        
        // Assert - Chargement réussi
        TEST_ASSERT_TRUE(loadResult.isSuccess());
        
        // Act - Valider
        auto validateResult = loader_->validateConfigurations();
        
        // Assert - Validation réussie
        TEST_ASSERT_TRUE(validateResult.isSuccess());
    }

    /**
     * @brief Test chargement multiple de configurations
     */
    void testMultipleLoadOperations() {
        // Arrange
        auto appConfig1 = std::make_shared<ApplicationConfiguration>();
        auto appConfig2 = std::make_shared<ApplicationConfiguration>();
        
        // Act - Premier chargement
        auto result1 = loader_->loadUnifiedConfigurations(appConfig1);
        
        // Act - Deuxième chargement (devrait remplacer le premier)
        auto result2 = loader_->loadUnifiedConfigurations(appConfig2);
        
        // Act - Validation (devrait utiliser la dernière config)
        auto validateResult = loader_->validateConfigurations();
        
        // Assert
        TEST_ASSERT_TRUE(result1.isSuccess());
        TEST_ASSERT_TRUE(result2.isSuccess());
        TEST_ASSERT_TRUE(validateResult.isSuccess());
    }

private:
    std::unique_ptr<ConfigurationLoader> loader_;
};

// === TESTS GLOBAUX ===

ConfigurationLoaderTest* configLoaderTestInstance = nullptr;

void setUpConfigLoader(void) {
    configLoaderTestInstance = new ConfigurationLoaderTest();
    configLoaderTestInstance->setUp();
}

void tearDownConfigLoader(void) {
    if (configLoaderTestInstance) {
        configLoaderTestInstance->tearDown();
        delete configLoaderTestInstance;
        configLoaderTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_config_loader_load_with_null_configuration() {
    setUpConfigLoader();
    configLoaderTestInstance->testLoadWithNullConfiguration();
    tearDownConfigLoader();
}

void test_config_loader_load_with_valid_configuration() {
    setUpConfigLoader();
    configLoaderTestInstance->testLoadWithValidConfiguration();
    tearDownConfigLoader();
}

void test_config_loader_validate_without_loading() {
    setUpConfigLoader();
    configLoaderTestInstance->testValidateWithoutLoading();
    tearDownConfigLoader();
}

void test_config_loader_validate_after_valid_loading() {
    setUpConfigLoader();
    configLoaderTestInstance->testValidateAfterValidLoading();
    tearDownConfigLoader();
}

void test_config_loader_complete_load_and_validate_sequence() {
    setUpConfigLoader();
    configLoaderTestInstance->testCompleteLoadAndValidateSequence();
    tearDownConfigLoader();
}

void test_config_loader_multiple_load_operations() {
    setUpConfigLoader();
    configLoaderTestInstance->testMultipleLoadOperations();
    tearDownConfigLoader();
}