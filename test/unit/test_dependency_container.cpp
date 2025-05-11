#include <unity.h>
#include "app/di/DependencyContainer.hpp"

// Classes de test
class TestInterface {
public:
    virtual ~TestInterface() = default;
    virtual int getValue() const = 0;
};

class TestImplementation : public TestInterface {
public:
    TestImplementation(int value) : value_(value) {}
    int getValue() const override { return value_; }
private:
    int value_;
};

// Configuration pour les tests
void setUp(void) {
    // Configuration avant chaque test
}

void tearDown(void) {
    // Nettoyage après chaque test
}

// Tests pour DependencyContainer
void test_register_and_resolve_dependency() {
    DependencyContainer container;
    auto instance = std::make_shared<int>(42);
    
    container.registerDependency<int>(instance);
    auto resolved = container.resolve<int>();
    
    TEST_ASSERT_NOT_NULL(resolved.get());
    TEST_ASSERT_EQUAL_INT(42, *resolved);
}

void test_register_and_resolve_implementation() {
    DependencyContainer container;
    auto implementation = std::make_shared<TestImplementation>(123);
    
    container.registerImplementation<TestInterface, TestImplementation>(implementation);
    auto resolved = container.resolve<TestInterface>();
    
    TEST_ASSERT_NOT_NULL(resolved.get());
    TEST_ASSERT_EQUAL_INT(123, resolved->getValue());
}

void test_resolve_nonexistent_dependency() {
    DependencyContainer container;
    auto resolved = container.resolve<float>();
    
    TEST_ASSERT_NULL(resolved.get());
}

void test_has_dependency() {
    DependencyContainer container;
    
    TEST_ASSERT_FALSE(container.has<int>());
    
    container.registerDependency(std::make_shared<int>(42));
    
    TEST_ASSERT_TRUE(container.has<int>());
}

void test_remove_dependency() {
    DependencyContainer container;
    container.registerDependency(std::make_shared<int>(42));
    
    TEST_ASSERT_TRUE(container.has<int>());
    
    bool removed = container.remove<int>();
    
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_FALSE(container.has<int>());
}

void test_clear_dependencies() {
    DependencyContainer container;
    container.registerDependency(std::make_shared<int>(42));
    container.registerDependency(std::make_shared<float>(3.14f));
    
    TEST_ASSERT_TRUE(container.has<int>());
    TEST_ASSERT_TRUE(container.has<float>());
    
    container.clear();
    
    TEST_ASSERT_FALSE(container.has<int>());
    TEST_ASSERT_FALSE(container.has<float>());
}

void test_register_and_resolve_factory() {
    DependencyContainer container;
    bool factoryCalled = false;
    
    container.registerFactory<TestInterface>([&factoryCalled]() {
        factoryCalled = true;
        return std::make_shared<TestImplementation>(456);
    });
    
    // Factory should not be called until resolve
    TEST_ASSERT_FALSE(factoryCalled);
    
    auto resolved = container.resolve<TestInterface>();
    
    TEST_ASSERT_TRUE(factoryCalled);
    TEST_ASSERT_NOT_NULL(resolved.get());
    TEST_ASSERT_EQUAL_INT(456, resolved->getValue());
    
    // Second resolution should use cached instance, not call factory again
    factoryCalled = false;
    resolved = container.resolve<TestInterface>();
    
    TEST_ASSERT_FALSE(factoryCalled);
    TEST_ASSERT_NOT_NULL(resolved.get());
    TEST_ASSERT_EQUAL_INT(456, resolved->getValue());
}

// Point d'entrée pour les tests
int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_register_and_resolve_dependency);
    RUN_TEST(test_register_and_resolve_implementation);
    RUN_TEST(test_resolve_nonexistent_dependency);
    RUN_TEST(test_has_dependency);
    RUN_TEST(test_remove_dependency);
    RUN_TEST(test_clear_dependencies);
    RUN_TEST(test_register_and_resolve_factory);
    
    return UNITY_END();
}
