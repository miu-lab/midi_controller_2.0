#pragma once

#include <unity.h>
#include "app/di/DependencyContainer.hpp"
#include <memory>
#include <string>

// Tests pour le DependencyContainer
class TestService {
public:
    int value = 42;
};

class ITestInterface {
public:
    virtual ~ITestInterface() = default;
    virtual int getValue() const = 0;
};

class TestImplementation : public ITestInterface {
public:
    int getValue() const override { return 123; }
};

void test_dependency_container_basic() {
    // Tester l'enregistrement et la résolution
    DependencyContainer container;
    
    // Enregistrer une instance
    auto service = std::make_shared<TestService>();
    container.registerDependency<TestService>(service);
    
    // Résoudre l'instance
    auto resolved = container.resolve<TestService>();
    
    // Vérifier que c'est la même instance
    TEST_ASSERT_NOT_NULL(resolved.get());
    TEST_ASSERT_EQUAL(service.get(), resolved.get());
    TEST_ASSERT_EQUAL(42, resolved->value);
}

void test_dependency_container_interface() {
    // Tester l'enregistrement et la résolution d'interfaces
    DependencyContainer container;
    
    // Enregistrer une implémentation d'interface
    auto impl = std::make_shared<TestImplementation>();
    container.registerDependency<ITestInterface>(impl);
    
    // Résoudre l'interface
    auto resolved = container.resolve<ITestInterface>();
    
    // Vérifier que c'est la même instance
    TEST_ASSERT_NOT_NULL(resolved.get());
    TEST_ASSERT_EQUAL(impl.get(), resolved.get());
    TEST_ASSERT_EQUAL(123, resolved->getValue());
}
