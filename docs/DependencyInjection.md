# Architecture d'injection de dépendances

Ce document explique l'architecture d'injection de dépendances utilisée dans le projet MIDI Controller.

## Principes fondamentaux

L'architecture du MIDI Controller est basée sur les principes SOLID, et particulièrement sur l'inversion de dépendance (le 'D' de SOLID). Cela signifie que:

1. Les modules de haut niveau ne dépendent pas des modules de bas niveau. Les deux dépendent d'abstractions.
2. Les abstractions ne dépendent pas des détails. Les détails dépendent des abstractions.

## Structure du système

### Conteneur de dépendances

Le cœur de notre système d'injection de dépendances est la classe `DependencyContainer`. Cette classe:

- Permet d'enregistrer des instances partagées
- Supporte l'enregistrement d'implémentations d'interfaces
- Permet la création paresseuse d'instances via des factories
- Gère le cycle de vie des objets avec des smart pointers

```cpp
// Exemple d'utilisation du conteneur de dépendances
auto container = std::make_shared<DependencyContainer>();

// Enregistrer une instance
auto config = std::make_shared<ApplicationConfiguration>();
container->registerDependency<ApplicationConfiguration>(config);

// Enregistrer une implémentation d'interface
auto midiOut = std::make_shared<TeensyUsbMidiOut>();
container->registerDependency<IMidiOut>(midiOut);

// Résoudre une dépendance
auto resolvedConfig = container->resolve<ApplicationConfiguration>();
```

### Script d'initialisation

Le script d'initialisation (`InitializationScript`) configure le conteneur de dépendances avec toutes les instances nécessaires à l'application.

```cpp
// Exemple d'utilisation du script d'initialisation
ApplicationConfiguration config;
auto container = std::make_shared<DependencyContainer>();
InitializationScript::initializeContainer(container, config);
```

### Interfaces sous-systèmes

L'application est organisée en sous-systèmes, chacun ayant une interface bien définie:

- `IConfiguration` - Gestion des paramètres et configurations
- `IInputSystem` - Gestion des entrées utilisateur
- `IMidiSystem` - Communication MIDI
- `IUISystem` - Interface utilisateur

Ces interfaces permettent un couplage faible entre les composants et facilitent les tests unitaires.

## Cycle de vie des objets

### Initialisation

Les objets sont initialisés dans un ordre qui respecte leurs dépendances:

1. Configuration (pas de dépendances)
2. Input (dépend de Configuration)
3. MIDI (peut dépendre de Configuration)
4. UI (peut dépendre de tous les autres)

### Destruction

Les objets sont détruits dans l'ordre inverse de leur création:

1. UI
2. MIDI
3. Input
4. Configuration

## Gestion des erreurs

Le projet utilise le pattern `Result<T, E>` pour la gestion des erreurs sans exceptions:

```cpp
// Exemple d'utilisation du pattern Result
Result<bool, std::string> init() {
    if (!configSystem_) {
        return Result<bool, std::string>::error("ConfigSystem not initialized");
    }
    return Result<bool, std::string>::success(true);
}
```

## Tests

L'architecture d'injection de dépendances facilite les tests unitaires en permettant de remplacer les dépendances par des mocks:

```cpp
// Exemple de test avec mocks
auto container = std::make_shared<DependencyContainer>();
auto displayMock = std::make_shared<IDisplayMock>();
container->registerDependency<IDisplay>(displayMock);

auto uiSystem = std::make_shared<UISubsystem>(container);
uiSystem->init();

// Vérifier que le display a été correctement initialisé
assert(displayMock->initialized);
```

## Bonnes pratiques

- Utilisez l'injection par constructeur plutôt que l'accès direct au conteneur
- Préférez les interfaces aux implémentations concrètes
- Utilisez `std::shared_ptr` pour les dépendances partagées
- Utilisez `std::weak_ptr` pour briser les cycles de dépendances
- Documentez clairement les dépendances de chaque classe
