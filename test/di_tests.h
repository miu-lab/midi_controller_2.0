#ifndef DI_TESTS_H
#define DI_TESTS_H

#include <unity.h>
#include "app/di/DependencyContainer.hpp"

// Classes de test pour les injections de dépendances
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

// Tests pour DependencyContainer
void test_dependency_container_basic() {
    DependencyContainer container;
    auto instance = std::make_shared<int>(42);
    
    container.registerDependency<int>(instance);
    auto resolved = container.resolve<int>();
    
    TEST_ASSERT_NOT_NULL(resolved.get());
    TEST_ASSERT_EQUAL_INT(42, *resolved);
}

void test_dependency_container_interface() {
    DependencyContainer container;
    auto impl = std::make_shared<TestImplementation>(123);
    
    container.registerImplementation<TestInterface, TestImplementation>(impl);
    auto resolved = container.resolve<TestInterface>();
    
    TEST_ASSERT_NOT_NULL(resolved.get());
    TEST_ASSERT_EQUAL_INT(123, resolved->getValue());
}

#endif // DI_TESTS_H
