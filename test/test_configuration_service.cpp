#include <unity.h>
#include <memory>

#include "core/configuration/ConfigurationService.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "mocks/MockConfiguration.hpp"
#include "fixtures/test_configurations.hpp"

/**
 * @brief Mock ApplicationConfiguration for testing ConfigurationService
 * 
 * This mock provides controlled test data for configuration service tests.
 */
class MockApplicationConfigurationForService {
public:
    MockApplicationConfigurationForService(MockConfiguration::ConfigurationType type) 
        : type_(type) {
        // Initialize controls based on type
        switch (type) {
            case MockConfiguration::ConfigurationType::MINIMAL:
                controls_ = TestConfigurations::getMinimalConfiguration();
                break;
            case MockConfiguration::ConfigurationType::COMPLEX:
                controls_ = TestConfigurations::getComplexConfiguration();
                break;
            case MockConfiguration::ConfigurationType::EMPTY:
                controls_ = TestConfigurations::getEmptyConfiguration();
                break;
            case MockConfiguration::ConfigurationType::ENCODER_WITH_CHILD_BUTTONS:
                controls_ = TestConfigurations::getComplexConfiguration(); // Fallback
                break;
            case MockConfiguration::ConfigurationType::INVALID:
                controls_ = TestConfigurations::getEmptyConfiguration(); // Fallback
                break;
        }
    }
    
    // Mock UnifiedConfiguration that returns our test controls
    class MockUnifiedConfiguration {
    public:
        MockUnifiedConfiguration(const std::vector<ControlDefinition>& controls) 
            : controls_(controls) {}
        
        const std::vector<ControlDefinition>& getAllControls() const {
            return controls_;
        }
        
        bool validate() const {
            return !controls_.empty(); // Simple validation for testing
        }
        
    private:
        std::vector<ControlDefinition> controls_;
    };
    
    const MockUnifiedConfiguration& getUnifiedConfiguration() const {
        if (!unifiedConfig_) {
            unifiedConfig_ = std::make_unique<MockUnifiedConfiguration>(controls_);
        }
        return *unifiedConfig_;
    }
    
private:
    MockConfiguration::ConfigurationType type_;
    std::vector<ControlDefinition> controls_;
    mutable std::unique_ptr<MockUnifiedConfiguration> unifiedConfig_;
};

/**
 * @brief Tests pour ConfigurationService
 * 
 * Test la logique métier de configuration extraite de ConfigurationSubsystem.
 */
class ConfigurationServiceTest {
public:
    void setUp() {
        // Will be initialized in each test
    }

    void tearDown() {
        service_.reset();
        mockAppConfig_.reset();
    }

    /**
     * @brief Test getAllControlDefinitions() avec configuration minimale
     */
    void testGetAllControlDefinitionsWithMinimalConfig() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::MINIMAL);
        
        // Act
        const auto& controls = service_->getAllControlDefinitions();
        
        // Assert
        TEST_ASSERT_TRUE(controls.size() >= 2); // Au moins encoder + button
        
        // Vérifier qu'on a bien un encodeur et un bouton
        bool hasEncoder = false, hasButton = false;
        for (const auto& control : controls) {
            if (control.hardware.type == InputType::ENCODER) hasEncoder = true;
            if (control.hardware.type == InputType::BUTTON) hasButton = true;
        }
        TEST_ASSERT_TRUE(hasEncoder);
        TEST_ASSERT_TRUE(hasButton);
    }

    /**
     * @brief Test getControlDefinitionsByType() pour encodeurs
     */
    void testGetControlDefinitionsByTypeEncoders() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::COMPLEX);
        
        // Act
        auto encoders = service_->getControlDefinitionsByType(InputType::ENCODER);
        
        // Assert
        TEST_ASSERT_EQUAL(4, encoders.size()); // Configuration complexe a 4 encodeurs
        
        for (const auto& encoder : encoders) {
            TEST_ASSERT_EQUAL(InputType::ENCODER, encoder.hardware.type);
        }
    }

    /**
     * @brief Test getControlDefinitionsByType() pour boutons
     */
    void testGetControlDefinitionsByTypeButtons() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::COMPLEX);
        
        // Act
        auto buttons = service_->getControlDefinitionsByType(InputType::BUTTON);
        
        // Assert
        TEST_ASSERT_EQUAL(4, buttons.size()); // Configuration complexe a 4 boutons
        
        for (const auto& button : buttons) {
            TEST_ASSERT_EQUAL(InputType::BUTTON, button.hardware.type);
        }
    }

    /**
     * @brief Test getControlDefinitionById() avec ID existant
     */
    void testGetControlDefinitionByIdExisting() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::MINIMAL);
        
        // Act
        auto control = service_->getControlDefinitionById(71); // ID d'encodeur minimal
        
        // Assert
        TEST_ASSERT_TRUE(control.has_value());
        TEST_ASSERT_EQUAL(71, control->id);
        TEST_ASSERT_EQUAL(InputType::ENCODER, control->hardware.type);
    }

    /**
     * @brief Test getControlDefinitionById() avec ID inexistant
     */
    void testGetControlDefinitionByIdNonExisting() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::MINIMAL);
        
        // Act
        auto control = service_->getControlDefinitionById(999); // ID inexistant
        
        // Assert
        TEST_ASSERT_FALSE(control.has_value());
    }

    /**
     * @brief Test getControlDefinitionsByGroup()
     */
    void testGetControlDefinitionsByGroup() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::COMPLEX);
        
        // Act
        auto encodersGroup = service_->getControlDefinitionsByGroup("encoders");
        auto buttonsGroup = service_->getControlDefinitionsByGroup("buttons");
        
        // Assert
        TEST_ASSERT_EQUAL(4, encodersGroup.size()); // 4 encodeurs dans le groupe "encoders"
        TEST_ASSERT_EQUAL(4, buttonsGroup.size());  // 4 boutons dans le groupe "buttons"
        
        // Vérifier que tous les controls du groupe "encoders" sont bien des encodeurs
        for (const auto& control : encodersGroup) {
            TEST_ASSERT_EQUAL_STRING("encoders", control.group.c_str());
            TEST_ASSERT_EQUAL(InputType::ENCODER, control.hardware.type);
        }
    }

    /**
     * @brief Test getAvailableGroups()
     */
    void testGetAvailableGroups() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::COMPLEX);
        
        // Act
        auto groups = service_->getAvailableGroups();
        
        // Assert
        TEST_ASSERT_TRUE(groups.size() >= 2); // Au moins "encoders" et "buttons"
        
        // Vérifier que les groupes attendus sont présents
        bool hasEncodersGroup = false, hasButtonsGroup = false;
        for (const auto& group : groups) {
            if (group == "encoders") hasEncodersGroup = true;
            if (group == "buttons") hasButtonsGroup = true;
        }
        TEST_ASSERT_TRUE(hasEncodersGroup);
        TEST_ASSERT_TRUE(hasButtonsGroup);
    }

    /**
     * @brief Test getInputCountByType()
     */
    void testGetInputCountByType() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::COMPLEX);
        
        // Act
        auto encoderCount = service_->getInputCountByType(InputType::ENCODER);
        auto buttonCount = service_->getInputCountByType(InputType::BUTTON);
        
        // Assert
        TEST_ASSERT_EQUAL(4, encoderCount); // Configuration complexe a 4 encodeurs
        TEST_ASSERT_EQUAL(4, buttonCount);  // Configuration complexe a 4 boutons
    }

    /**
     * @brief Test validateAllConfigurations() avec configuration valide
     */
    void testValidateAllConfigurationsValid() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::MINIMAL);
        
        // Act
        bool isValid = service_->validateAllConfigurations();
        
        // Assert
        TEST_ASSERT_TRUE(isValid);
    }

    /**
     * @brief Test validateAllConfigurations() avec configuration vide
     */
    void testValidateAllConfigurationsEmpty() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::EMPTY);
        
        // Act
        bool isValid = service_->validateAllConfigurations();
        
        // Assert
        TEST_ASSERT_FALSE(isValid); // Configuration vide devrait être invalide
    }

    /**
     * @brief Test setApplicationConfiguration()
     */
    void testSetApplicationConfiguration() {
        // Arrange
        setupWithConfigType(MockConfiguration::ConfigurationType::MINIMAL);
        auto initialCount = service_->getAllControlDefinitions().size();
        
        // Act - Changer pour une configuration plus complexe
        auto newMockConfig = std::make_shared<MockApplicationConfigurationForService>(
            MockConfiguration::ConfigurationType::COMPLEX);
        auto newAppConfig = std::reinterpret_pointer_cast<ApplicationConfiguration>(
            std::static_pointer_cast<void>(newMockConfig));
        service_->setApplicationConfiguration(newAppConfig);
        
        // Assert
        auto newCount = service_->getAllControlDefinitions().size();
        TEST_ASSERT_TRUE(newCount > initialCount); // Devrait avoir plus de contrôles
    }

private:
    std::unique_ptr<ConfigurationService> service_;
    std::shared_ptr<MockApplicationConfigurationForService> mockAppConfig_;
    
    void setupWithConfigType(MockConfiguration::ConfigurationType type) {
        mockAppConfig_ = std::make_shared<MockApplicationConfigurationForService>(type);
        
        // Cast to ApplicationConfiguration for the service
        auto appConfig = std::reinterpret_pointer_cast<ApplicationConfiguration>(
            std::static_pointer_cast<void>(mockAppConfig_));
        
        service_ = std::make_unique<ConfigurationService>(appConfig);
    }
};

// === TESTS GLOBAUX ===

ConfigurationServiceTest* configServiceTestInstance = nullptr;

void setUpConfigService(void) {
    configServiceTestInstance = new ConfigurationServiceTest();
    configServiceTestInstance->setUp();
}

void tearDownConfigService(void) {
    if (configServiceTestInstance) {
        configServiceTestInstance->tearDown();
        delete configServiceTestInstance;
        configServiceTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_config_service_get_all_control_definitions_minimal() {
    setUpConfigService();
    configServiceTestInstance->testGetAllControlDefinitionsWithMinimalConfig();
    tearDownConfigService();
}

void test_config_service_get_by_type_encoders() {
    setUpConfigService();
    configServiceTestInstance->testGetControlDefinitionsByTypeEncoders();
    tearDownConfigService();
}

void test_config_service_get_by_type_buttons() {
    setUpConfigService();
    configServiceTestInstance->testGetControlDefinitionsByTypeButtons();
    tearDownConfigService();
}

void test_config_service_get_by_id_existing() {
    setUpConfigService();
    configServiceTestInstance->testGetControlDefinitionByIdExisting();
    tearDownConfigService();
}

void test_config_service_get_by_id_non_existing() {
    setUpConfigService();
    configServiceTestInstance->testGetControlDefinitionByIdNonExisting();
    tearDownConfigService();
}

void test_config_service_get_by_group() {
    setUpConfigService();
    configServiceTestInstance->testGetControlDefinitionsByGroup();
    tearDownConfigService();
}

void test_config_service_get_available_groups() {
    setUpConfigService();
    configServiceTestInstance->testGetAvailableGroups();
    tearDownConfigService();
}

void test_config_service_get_input_count_by_type() {
    setUpConfigService();
    configServiceTestInstance->testGetInputCountByType();
    tearDownConfigService();
}

void test_config_service_validate_all_configurations_valid() {
    setUpConfigService();
    configServiceTestInstance->testValidateAllConfigurationsValid();
    tearDownConfigService();
}

void test_config_service_validate_all_configurations_empty() {
    setUpConfigService();
    configServiceTestInstance->testValidateAllConfigurationsEmpty();
    tearDownConfigService();
}

void test_config_service_set_application_configuration() {
    setUpConfigService();
    configServiceTestInstance->testSetApplicationConfiguration();
    tearDownConfigService();
}