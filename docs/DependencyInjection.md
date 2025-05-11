# Guide d'utilisation du système d'injection de dépendances

Ce document explique comment utiliser le nouveau système d'injection de dépendances qui remplace l'ancien ServiceLocator. Le système est basé sur deux composants principaux : `DependencyContainer` et `ServiceLocatorAdapter`.

## Concepts clés

### 1. DependencyContainer

Le `DependencyContainer` est un conteneur flexible pour l'enregistrement et la résolution de dépendances. Il permet d'enregistrer des instances de services, d'implémenter des interfaces et des factories pour créer des instances à la demande.

### 2. ServiceLocatorAdapter

Le `ServiceLocatorAdapter` est un adaptateur qui facilite la transition de l'ancien `ServiceLocator` vers le nouveau système d'injection de dépendances. Il implémente l'ancienne API mais utilise le `DependencyContainer` en interne.

## Exemples d'utilisation

### Initialisation du système

```cpp
// Créer le conteneur
auto container = std::make_shared<DependencyContainer>();

// Créer l'adaptateur
auto adapter = std::make_shared<ServiceLocatorAdapter>(container);

// Définir l'instance par défaut pour les appels statiques
ServiceLocatorAdapter::setDefaultInstance(adapter);

// Initialiser avec la configuration
adapter->initialize(appConfig);
```

### Enregistrement de dépendances

```cpp
// Enregistrer une instance simple
auto configService = std::make_shared<ConfigurationService>();
container->registerDependency<ConfigurationService>(configService);

// Enregistrer une implémentation d'interface
auto inputSystem = std::make_shared<InputSubsystem>(container);
container->registerDependency<IInputSystem>(inputSystem);
container->registerDependency<InputSubsystem>(inputSystem);

// Enregistrer une factory
container->registerFactory<UIController>([container]() {
    return std::make_shared<UIController>(
        container->resolve<ViewManager>(),
        container->resolve<MenuController>()
    );
});
```

### Résolution de dépendances

```cpp
// Récupérer une instance simple
auto configService = container->resolve<ConfigurationService>();
if (configService) {
    // Utiliser le service
    configService->loadConfig();
}

// Récupérer via une interface
auto inputSystem = container->resolve<IInputSystem>();
if (inputSystem) {
    // Utiliser l'interface
    inputSystem->update();
}

// La factory sera appelée automatiquement lors de la première résolution
auto uiController = container->resolve<UIController>();
```

### Utilisation de ServiceLocatorAdapter

Pour le code existant qui ne peut pas être modifié immédiatement, utilisez `ServiceLocatorAdapter` comme un remplacement direct de `ServiceLocator`:

```cpp
// Au lieu de:
// auto &controller = ServiceLocator::getUIController();

// Utilisez:
auto &controller = ServiceLocatorAdapter::getUIControllerStatic();
```

## Bonnes pratiques

1. **Préférez l'injection par constructeur**: Passez les dépendances dans le constructeur plutôt que de les résoudre à l'intérieur des méthodes.

```cpp
// Préférez ceci:
MyClass::MyClass(std::shared_ptr<DependencyContainer> container) {
    auto dependency = container->resolve<IDependency>();
    // ...
}

// À ceci:
void MyClass::method() {
    auto dependency = ServiceLocatorAdapter::getDependencyStatic();
    // ...
}
```

2. **Utilisez les interfaces**: Programmez par rapport aux interfaces plutôt qu'aux implémentations concrètes.

```cpp
// Préférez:
container->registerDependency<IService>(serviceImpl);
auto service = container->resolve<IService>();

// À:
container->registerDependency<ConcreteService>(serviceImpl);
auto service = container->resolve<ConcreteService>();
```

3. **Gérez correctement le cycle de vie**: Utilisez les smart pointers appropriés selon le cycle de vie attendu des objets.

4. **Évitez les dépendances circulaires**: Les dépendances circulaires peuvent être difficiles à gérer. Essayez de restructurer le code pour les éviter.

## Migration progressive

La migration vers le nouveau système peut se faire progressivement:

1. Activez `ServiceLocatorAdapter` dans main.cpp
2. Remplacez les appels à `ServiceLocator::getXXX()` par `ServiceLocatorAdapter::getXXXStatic()`
3. Pour les nouvelles classes, utilisez directement `DependencyContainer`
4. Progressivement, refactorisez les classes existantes pour utiliser l'injection par constructeur

## Résolution de problèmes

- **Problème**: Erreur lors de la résolution d'une dépendance
  - **Solution**: Vérifiez que la dépendance a bien été enregistrée avant d'essayer de la résoudre

- **Problème**: Dépendances circulaires
  - **Solution**: Utilisez des `std::weak_ptr` pour l'une des dépendances ou injectez la dépendance après construction

- **Problème**: Performance
  - **Solution**: Si nécessaire, utilisez des factories pour créer les objets à la demande plutôt que de tous les créer au démarrage
