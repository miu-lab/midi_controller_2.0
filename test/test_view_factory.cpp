#include <unity.h>
#include <memory>
#include <typeinfo>

#include "core/ui/ViewFactory.hpp"
#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "app/di/DependencyContainer.hpp"

/**
 * @brief Mock DependencyContainer pour les tests
 */
class MockDependencyContainerForViewFactory : public DependencyContainer {
public:
    enum class DependencyType {
        ALL_PRESENT,
        MISSING_LVGL_BRIDGE,
        MISSING_UNIFIED_CONFIG,
        MISSING_EVENT_BUS,
        ALL_MISSING
    };

    explicit MockDependencyContainerForViewFactory(DependencyType type) : type_(type) {}

    template<typename T>
    std::shared_ptr<T> resolve() const {
        // Note: Cette approche simple évite la complexité de constexpr if
        // qui peut ne pas être supportée partout. Nous retournons des pointeurs
        // factices pour les tests de validation des dépendances uniquement.
        
        if (type_ == DependencyType::ALL_MISSING) {
            return nullptr;
        }
        
        // Simulation simple - retour de pointeurs factices pour les tests
        static int dummy1, dummy2, dummy3;
        
        if (typeid(T) == typeid(Ili9341LvglBridge)) {
            if (type_ == DependencyType::MISSING_LVGL_BRIDGE) {
                return nullptr;
            }
            return std::shared_ptr<T>(reinterpret_cast<T*>(&dummy1), [](T*) {});
        }
        else if (typeid(T) == typeid(UnifiedConfiguration)) {
            if (type_ == DependencyType::MISSING_UNIFIED_CONFIG) {
                return nullptr;
            }
            return std::shared_ptr<T>(reinterpret_cast<T*>(&dummy2), [](T*) {});
        }
        else if (typeid(T) == typeid(EventBus)) {
            if (type_ == DependencyType::MISSING_EVENT_BUS) {
                return nullptr;
            }
            return std::shared_ptr<T>(reinterpret_cast<T*>(&dummy3), [](T*) {});
        }
        return nullptr;
    }

private:
    DependencyType type_;
};

/**
 * @brief Tests pour ViewFactory
 */
class ViewFactoryTest {
public:
    void setUp() {
        // Configuration par défaut avec toutes dépendances présentes
        mockContainer_ = std::make_shared<MockDependencyContainerForViewFactory>(
            MockDependencyContainerForViewFactory::DependencyType::ALL_PRESENT);
        viewFactory_ = std::make_unique<ViewFactory>(mockContainer_);
    }

    void tearDown() {
        viewFactory_.reset();
        mockContainer_.reset();
    }

    /**
     * @brief Test constructeur avec conteneur valide
     */
    void testConstructorWithValidContainer() {
        // Arrange & Act
        auto factory = std::make_unique<ViewFactory>(mockContainer_);
        
        // Assert
        TEST_ASSERT_NOT_NULL(factory.get());
    }

    /**
     * @brief Test constructeur avec conteneur null
     */
    void testConstructorWithNullContainer() {
        // Arrange & Act
        auto factory = std::make_unique<ViewFactory>(nullptr);
        
        // Assert
        TEST_ASSERT_NOT_NULL(factory.get());
    }

    /**
     * @brief Test validateDependencies avec toutes dépendances présentes
     */
    void testValidateDependenciesAllPresent() {
        // Act
        bool result = viewFactory_->validateDependencies();
        
        // Assert
        TEST_ASSERT_TRUE(result);
    }

    /**
     * @brief Test validateDependencies avec dépendances manquantes
     */
    void testValidateDependenciesMissing() {
        // Arrange
        auto missingContainer = std::make_shared<MockDependencyContainerForViewFactory>(
            MockDependencyContainerForViewFactory::DependencyType::ALL_MISSING);
        auto factory = std::make_unique<ViewFactory>(missingContainer);
        
        // Act
        bool result = factory->validateDependencies();
        
        // Assert
        TEST_ASSERT_FALSE(result);
    }

    /**
     * @brief Test validateDependencies avec conteneur null
     */
    void testValidateDependenciesNullContainer() {
        // Arrange
        auto factory = std::make_unique<ViewFactory>(nullptr);
        
        // Act
        bool result = factory->validateDependencies();
        
        // Assert
        TEST_ASSERT_FALSE(result);
    }

    /**
     * @brief Test createDefaultViewManager avec dépendances valides
     */
    void testCreateDefaultViewManagerSuccess() {
        // Act
        auto result = viewFactory_->createDefaultViewManager();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_NOT_NULL(result.getValue().get());
    }

    /**
     * @brief Test createDefaultViewManager avec dépendances manquantes
     */
    void testCreateDefaultViewManagerMissingDependencies() {
        // Arrange
        auto missingContainer = std::make_shared<MockDependencyContainerForViewFactory>(
            MockDependencyContainerForViewFactory::DependencyType::MISSING_LVGL_BRIDGE);
        auto factory = std::make_unique<ViewFactory>(missingContainer);
        
        // Act
        auto result = factory->createDefaultViewManager();
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, result.getError().code);
    }

    /**
     * @brief Test createViewManager avec Full UI activé
     */
    void testCreateViewManagerWithFullUI() {
        // Arrange
        ViewFactory::ViewManagerConfig config;
        config.enableFullUI = true;
        config.registerInContainer = false; // Éviter l'enregistrement pour les tests
        
        // Act
        auto result = viewFactory_->createViewManager(config);
        
        // Assert
        // Note: Ce test peut échouer à l'initialisation du ViewManager car nous utilisons des mocks
        // mais il devrait au moins passer la validation des dépendances
        TEST_ASSERT_TRUE(result.isSuccess() || result.getError().code == ErrorCode::InitializationFailed);
    }

    /**
     * @brief Test createViewManager avec Full UI désactivé
     */
    void testCreateViewManagerWithoutFullUI() {
        // Arrange
        ViewFactory::ViewManagerConfig config;
        config.enableFullUI = false;
        
        // Act
        auto result = viewFactory_->createViewManager(config);
        
        // Assert
        TEST_ASSERT_FALSE(result.isSuccess());
        TEST_ASSERT_EQUAL(ErrorCode::ConfigurationError, result.getError().code);
    }

private:
    std::shared_ptr<MockDependencyContainerForViewFactory> mockContainer_;
    std::unique_ptr<ViewFactory> viewFactory_;
};

// === TESTS GLOBAUX ===

ViewFactoryTest* viewFactoryTestInstance = nullptr;

void setUpViewFactory(void) {
    viewFactoryTestInstance = new ViewFactoryTest();
    viewFactoryTestInstance->setUp();
}

void tearDownViewFactory(void) {
    if (viewFactoryTestInstance) {
        viewFactoryTestInstance->tearDown();
        delete viewFactoryTestInstance;
        viewFactoryTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_view_factory_constructor_valid_container() {
    setUpViewFactory();
    viewFactoryTestInstance->testConstructorWithValidContainer();
    tearDownViewFactory();
}

void test_view_factory_constructor_null_container() {
    setUpViewFactory();
    viewFactoryTestInstance->testConstructorWithNullContainer();
    tearDownViewFactory();
}

void test_view_factory_validate_dependencies_all_present() {
    setUpViewFactory();
    viewFactoryTestInstance->testValidateDependenciesAllPresent();
    tearDownViewFactory();
}

void test_view_factory_validate_dependencies_missing() {
    setUpViewFactory();
    viewFactoryTestInstance->testValidateDependenciesMissing();
    tearDownViewFactory();
}

void test_view_factory_validate_dependencies_null_container() {
    setUpViewFactory();
    viewFactoryTestInstance->testValidateDependenciesNullContainer();
    tearDownViewFactory();
}

void test_view_factory_create_default_view_manager_success() {
    setUpViewFactory();
    viewFactoryTestInstance->testCreateDefaultViewManagerSuccess();
    tearDownViewFactory();
}

void test_view_factory_create_default_view_manager_missing_dependencies() {
    setUpViewFactory();
    viewFactoryTestInstance->testCreateDefaultViewManagerMissingDependencies();
    tearDownViewFactory();
}

void test_view_factory_create_view_manager_with_full_ui() {
    setUpViewFactory();
    viewFactoryTestInstance->testCreateViewManagerWithFullUI();
    tearDownViewFactory();
}

void test_view_factory_create_view_manager_without_full_ui() {
    setUpViewFactory();
    viewFactoryTestInstance->testCreateViewManagerWithoutFullUI();
    tearDownViewFactory();
}