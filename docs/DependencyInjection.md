# Injection de Dépendances

Ce document explique l'architecture d'injection de dépendances utilisée dans le projet MIDI Controller.

## Vue d'ensemble

Le projet utilise un système d'injection de dépendances (DI) moderne pour découpler les composants et faciliter les tests. Cette architecture remplace l'ancien système basé sur le patron de conception ServiceLocator.

![Architecture DI](../assets/di_architecture.png)

## Composants principaux

### DependencyContainer

Le `DependencyContainer` est le cœur du système d'injection de dépendances:

```cpp
// Enregistrement d'une dépendance
container->registerDependency<IInputSystem>(inputSystem);

// Résolution d'une dépendance
auto inputSystem = container->resolve<IInputSystem>();
```

Caractéristiques:
- Gestion du cycle de vie des objets avec smart pointers
- Support pour les interfaces et les classes concrètes
- Création paresseuse via des factories
- Thread-safe

### Interfaces et Abstractions

Le système repose fortement sur les interfaces pour découpler les composants:

- `IConfiguration` - Configuration et paramètres
- `IInputSystem` - Système d'entrée
- `IMidiSystem` - Système MIDI
- `IUISystem` - Système d'interface utilisateur

### InitializationScript

Le fichier `InitializationScript.hpp` centralise la configuration des dépendances:

```cpp
static bool initializeContainer(std::shared_ptr<DependencyContainer> container,
                              const ApplicationConfiguration& config) {
    // Enregistrement des dépendances
    auto configSystem = std::make_shared<ConfigurationSubsystem>(container);
    container->registerDependency<IConfiguration>(configSystem);
    
    // ...
}
```

## Gestion des dépendances circulaires

Les dépendances circulaires sont gérées de deux façons:

1. **Interfaces**: L'utilisation d'interfaces réduit naturellement les cycles
2. **std::weak_ptr**: Pour les relations qui doivent être bidirectionnelles

```cpp
// Exemple de gestion de dépendances circulaires
class ComponentA {
private:
    std::shared_ptr<ComponentB> b_;
};

class ComponentB {
private:
    std::weak_ptr<ComponentA> a_; // Utilisation de weak_ptr pour éviter les cycles
};
```

## Utilisation dans les tests

Le système d'injection de dépendances facilite les tests unitaires:

```cpp
// Test avec mock
auto mockInput = std::make_shared<MockInputSystem>();
container->registerDependency<IInputSystem>(mockInput);

// L'objet testé recevra automatiquement le mock
auto app = std::make_shared<MidiControllerApp>(container);
```

## Migration depuis ServiceLocator

### Avant
```cpp
// Ancien code utilisant ServiceLocator
auto& inputSystem = ServiceLocator::getInputSystem();
inputSystem.process();
```

### Après
```cpp
// Avec injection de dépendances
explicit MyClass(std::shared_ptr<IInputSystem> inputSystem)
  : inputSystem_(inputSystem) {
}

void process() {
    inputSystem_->process();
}
```

## Meilleures pratiques

1. **Injecter les interfaces** plutôt que les implémentations concrètes
2. **Préférer l'injection par constructeur** aux autres formes d'injection
3. **Nommer clairement les dépendances** avec des suffixes `_`
4. **Minimiser les dépendances** de chaque classe
5. **Utiliser des smart pointers** pour clarifier l'intention
   - `std::shared_ptr` - Durée de vie partagée
   - `std::unique_ptr` - Propriété exclusive
   - `std::weak_ptr` - Référence non-propriétaire

## FAQ

**Q: Pourquoi utiliser l'injection de dépendances plutôt que ServiceLocator?**
R: L'injection de dépendances rend les dépendances explicites, facilite les tests et améliore la modularité.

**Q: Comment éviter la "soupe de paramètres" dans les constructeurs?**
R: Regrouper les paramètres logiquement liés dans des structures, et suivre le principe de responsabilité unique.

**Q: Comment faire pour les plateformes embarquées avec ressources limitées?**
R: Notre implémentation est légère et adaptée à la plateforme Teensy avec des optimisations pour la performance.
