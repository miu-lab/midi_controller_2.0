# Architecture d'Injection de Dépendances Pure

Ce document décrit la nouvelle architecture d'injection de dépendances pure, qui a complètement éliminé ServiceLocator et ServiceLocatorAdapter.

## Objectifs de l'architecture DI Pure

1. **Simplicité**: Une seule méthode de résolution des dépendances
2. **Testabilité**: Faciliter les tests en permettant l'injection de mocks
3. **Explicitation des dépendances**: Rendre les dépendances explicites dans les signatures des constructeurs
4. **Élimination des singletons**: Supprimer tout accès global à l'état
5. **Gestion cohérente du cycle de vie**: Utilisation systématique de smart pointers

## Principaux composants

### 1. DependencyContainer

Le conteneur centralise toutes les dépendances et fournit les méthodes pour les enregistrer et les résoudre:

```cpp
// Enregistrement d'une dépendance
container->registerDependency<IService>(serviceImpl);

// Résolution d'une dépendance
auto service = container->resolve<IService>();
```

### 2. InitializationScript

Script qui centralise la logique d'initialisation de toutes les dépendances:

```cpp
// Dans main.cpp
bool initSuccess = InitializationScript::initializeContainer(container, appConfig);
```

Ce script crée et enregistre tous les composants du système dans l'ordre correct.

### 3. Interfaces clairement définies

Interfaces qui définissent les contrats entre les composants:

- `IConfiguration`
- `IInputSystem`
- `IMidiSystem`
- `IUISystem`

### 4. Sous-systèmes qui implémentent ces interfaces

- `ConfigurationSubsystem`
- `InputSubsystem`
- `MidiSubsystem`
- `UISubsystem`

## Cycle de vie de l'application

1. Création du conteneur de dépendances
2. Initialisation du conteneur via InitializationScript
3. Création de l'application avec le conteneur
4. Initialisation de l'application
5. Boucle de mise à jour

## Exemple d'utilisation

```cpp
// Création du conteneur
auto container = std::make_shared<DependencyContainer>();

// Initialisation
InitializationScript::initializeContainer(container, appConfig);

// Création de l'application
auto app = std::make_shared<MidiControllerApp>(container);

// Initialisation et utilisation
app->init();
app->update();
```

## Constructeurs avec injection explicite

Exemple de classe avec injection explicite de dépendances:

```cpp
class InputController {
public:
    // Constructeur avec injection explicite
    explicit InputController(std::shared_ptr<NavigationConfigService> navigationConfig);
    
    // Injecteur supplémentaire (setter)
    void setUIController(std::shared_ptr<UIController> uiController);
    
private:
    std::shared_ptr<NavigationConfigService> m_navigationConfig;
    std::shared_ptr<UIController> m_uiController;
};
```

## Migration depuis l'ancienne architecture

Pour migrer une classe depuis l'ancienne architecture:

1. Modifier les constructeurs pour accepter des `std::shared_ptr` au lieu de références
2. Utiliser des membres de type `std::shared_ptr` au lieu de références ou pointeurs
3. Éliminer les références à ServiceLocator et ServiceLocatorAdapter
4. Mettre à jour InitializationScript pour enregistrer la nouvelle implémentation

## Bonnes pratiques

1. **Utiliser des interfaces**: Programmer par rapport aux interfaces, pas aux implémentations
2. **Dépendances explicites**: Toutes les dépendances doivent être visibles dans les constructeurs
3. **Éviter les singletons**: Ne pas utiliser de singletons ou d'état global
4. **Smart pointers**: Utiliser systématiquement des smart pointers pour la gestion de la mémoire
5. **Ordre d'initialisation**: Respecter l'ordre des dépendances lors de l'initialisation

Cette architecture représente une évolution majeure dans la qualité du code et facilite les futures évolutions du projet.
