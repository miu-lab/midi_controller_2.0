#pragma once

#include <unity.h>
#include "app/di/DependencyContainer.hpp"
#include "app/InitializationScript.hpp"
#include "app/MidiControllerApp.hpp"
#include "config/ApplicationConfiguration.hpp"
#include <memory>

// Forward declarations des fonctions de test
void test_pure_di_initialization();
void test_pure_di_subsystem_resolution();
void test_pure_di_component_lifecycle();
void test_pure_di_cyclic_dependencies();

/**
 * @brief Test de l'initialisation du conteneur sans ServiceLocatorAdapter
 */
void test_pure_di_initialization() {
    // Créer une configuration
    ApplicationConfiguration appConfig;
    
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Initialiser le conteneur avec le script d'initialisation
    bool initSuccess = InitializationScript::initializeContainer(container, appConfig);
    
    // Vérifier que l'initialisation a réussi
    TEST_ASSERT_TRUE(initSuccess);
    
    // Vérifier que les principales dépendances sont présentes
    TEST_ASSERT_TRUE(container->has<IConfiguration>());
    TEST_ASSERT_TRUE(container->has<IInputSystem>());
    TEST_ASSERT_TRUE(container->has<IMidiSystem>());
    TEST_ASSERT_TRUE(container->has<IUISystem>());
}

/**
 * @brief Test de la résolution des sous-systèmes dans l'architecture pure
 */
void test_pure_di_subsystem_resolution() {
    // Créer une configuration
    ApplicationConfiguration appConfig;
    
    // Créer un conteneur de dépendances
    auto container = std::make_shared<DependencyContainer>();
    
    // Initialiser le conteneur
    InitializationScript::initializeContainer(container, appConfig);
    
    // Résoudre les sous-systèmes
    auto configSystem = container->resolve<IConfiguration>();
    auto inputSystem = container->resolve<IInputSystem>();
    auto midiSystem = container->resolve<IMidiSystem>();
    auto uiSystem = container->resolve<IUISystem>();
    
    // Vérifier que les sous-systèmes ont été résolus correctement
    TEST_ASSERT_NOT_NULL(configSystem.get());
    TEST_ASSERT_NOT_NULL(inputSystem.get());
    TEST_ASSERT_NOT_NULL(midiSystem.get());
    TEST_ASSERT_NOT_NULL(uiSystem.get());
}

/**
 * @brief Test du cycle de vie des composants dans l'architecture pure
 */
void test_pure_di_component_lifecycle() {
    // Créer un conteneur et une application dans un scope
    {
        // Créer une configuration
        ApplicationConfiguration appConfig;
        
        // Créer un conteneur de dépendances
        auto container = std::make_shared<DependencyContainer>();
        
        // Initialiser le conteneur
        InitializationScript::initializeContainer(container, appConfig);
        
        // Créer l'application avec le conteneur
        auto app = std::make_shared<MidiControllerApp>(container);
        
        // Initialiser l'application
        auto result = app->init();
        TEST_ASSERT_TRUE(result.isSuccess());
        
        // Ici, l'application et le conteneur sont valides
        
        // Appeler update pour s'assurer qu'il n'y a pas d'erreurs
        app->update();
    }
    // À la fin du scope, l'application et le conteneur sont détruits
    // Aucun test explicite ici, mais si des destructeurs lancent des erreurs,
    // le test échouera
}

/**
 * @brief Test de la gestion des dépendances cycliques dans l'architecture pure
 */
void test_pure_di_cyclic_dependencies() {
    // Créer un conteneur avec des dépendances cycliques simulées
    auto container = std::make_shared<DependencyContainer>();
    
    // Classes pour tester les dépendances cycliques
    class A; // Forward declaration
    class B; // Forward declaration
    
    class A {
    public:
        explicit A(std::shared_ptr<B> b = nullptr) : b_(b) {}
        void setB(std::shared_ptr<B> b) { b_ = b; }
    private:
        std::shared_ptr<B> b_;
    };
    
    class B {
    public:
        explicit B(std::shared_ptr<A> a = nullptr) : a_(a) {}
        void setA(std::shared_ptr<A> a) { a_ = a; }
    private:
        std::shared_ptr<A> a_;
    };
    
    // Créer les instances
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();
    
    // Établir les références cycliques
    a->setB(b);
    b->setA(a);
    
    // Enregistrer dans le conteneur
    container->registerDependency<A>(a);
    container->registerDependency<B>(b);
    
    // Résoudre les instances
    auto resolvedA = container->resolve<A>();
    auto resolvedB = container->resolve<B>();
    
    // Vérifier que les instances ont été résolues correctement
    TEST_ASSERT_NOT_NULL(resolvedA.get());
    TEST_ASSERT_NOT_NULL(resolvedB.get());
    
    // Vérifier que ce sont les mêmes instances
    TEST_ASSERT_EQUAL(a.get(), resolvedA.get());
    TEST_ASSERT_EQUAL(b.get(), resolvedB.get());
    
    // Les objets seront correctement nettoyés à la fin du test
    // Aucun test explicite ici, mais si destructeur lance des erreurs,
    // le test échouera
}
