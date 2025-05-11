# Architecture en Injection de Dépendances Pure (sans ServiceLocator)

Ce document décrit l'architecture d'injection de dépendances pure mise en place dans le MIDI Controller après la suppression complète de `ServiceLocator` et `ServiceLocatorAdapter`.

## Évolution de l'architecture

L'architecture a évolué en plusieurs phases:

1. **Architecture originale**: Utilisation directe de `ServiceLocator` comme registre global de services.
2. **Phase intermédiaire**: Introduction de `DependencyContainer` et de `ServiceLocatorAdapter` pour assurer la compatibilité.
3. **Architecture actuelle**: Utilisation exclusive de `DependencyContainer` avec injection directe par constructeur.

## Principes de l'architecture actuelle

L'architecture actuelle est basée sur les principes suivants:

1. **Injection par constructeur**: Les dépendances sont injectées via les constructeurs.
2. **Interfaces explicites**: Toutes les interactions entre composants se font via des interfaces.
3. **Conteneur centralisé**: `DependencyContainer` gère la résolution des dépendances.
4. **Initialisation scripté**: `InitializationScript` configure toutes les dépendances.

## Composants clés

### DependencyContainer

Le `DependencyContainer` est le cœur de notre système. Il offre:

- Enregistrement d'instances (`registerDependency<T>`)
- Résolution de dépendances (`resolve<T>`)
- Support pour les factories (`registerFactory<T>`)
- Gestion du cycle de vie avec smart pointers

```cpp
auto container = std::make_shared<DependencyContainer>();
container->registerDependency<IConfiguration>(configSubsystem);
auto config = container->resolve<IConfiguration>();
```

### InitializationScript

`InitializationScript` est une classe statique qui initialise le conteneur de dépendances:

```cpp
// Création et initialisation du conteneur
auto container = std::make_shared<DependencyContainer>();
InitializationScript::initializeContainer(container, appConfig);
```

Cette classe:
- Crée toutes les instances nécessaires
- Configure les dépendances entre instances
- Enregistre toutes les instances dans le conteneur

### MidiControllerApp

`MidiControllerApp` est la classe principale de l'application. Elle:

- Reçoit le conteneur de dépendances via son constructeur
- Résout les sous-systèmes à partir du conteneur
- Coordonne l'initialisation et la mise à jour des sous-systèmes

```cpp
MidiControllerApp::MidiControllerApp(std::shared_ptr<DependencyContainer> container)
    : m_container(container) {
}

Result<bool, std::string> MidiControllerApp::init() {
    // Résoudre les sous-systèmes
    m_configSystem = m_container->resolve<IConfiguration>();
    m_inputSystem = m_container->resolve<IInputSystem>();
    m_midiSystem = m_container->resolve<IMidiSystem>();
    m_uiSystem = m_container->resolve<IUISystem>();
    
    // Initialiser les sous-systèmes...
}
```

## Sous-systèmes

L'application est organisée en sous-systèmes, chacun ayant sa propre interface:

- `IConfiguration`: Gestion des paramètres et configurations
- `IInputSystem`: Gestion des entrées utilisateur
- `IMidiSystem`: Communication MIDI
- `IUISystem`: Interface utilisateur

Chaque sous-système:
- Implémente son interface correspondante
- Reçoit le conteneur de dépendances via son constructeur
- Résout ses propres dépendances à partir du conteneur

## Gestion des dépendances circulaires

Les dépendances circulaires sont gérées de plusieurs façons:

1. **Interfaces**: L'utilisation d'interfaces réduit les dépendances directes.
2. **Injection différée**: Certaines dépendances sont injectées après la construction.
3. **Weak pointers**: `std::weak_ptr` est utilisé pour briser les cycles de référence.

## Avantages de l'architecture pure

1. **Testabilité**: Chaque composant peut être testé isolément.
2. **Modularité**: Les composants sont faiblement couplés.
3. **Clarté**: Les dépendances sont explicites dans les constructeurs.
4. **Extensibilité**: De nouveaux composants peuvent être ajoutés facilement.
5. **Cycle de vie explicite**: Meilleure gestion des ressources.

## Exemple d'utilisation

```cpp
// Dans main.cpp
void setup() {
    // Configuration
    ApplicationConfiguration appConfig;
    
    // Création et initialisation du conteneur
    auto container = std::make_shared<DependencyContainer>();
    InitializationScript::initializeContainer(container, appConfig);
    
    // Création de l'application
    app = std::make_shared<MidiControllerApp>(container);
    
    // Initialisation de l'application
    auto result = app->init();
    if (result.isError()) {
        // Gestion de l'erreur
    }
}

void loop() {
    // Mise à jour de l'application
    app->update();
}
```

## Bonnes pratiques

1. **Injecter les interfaces, pas les implémentations**: Utiliser `IConfiguration` plutôt que `ConfigurationSubsystem`.
2. **Limiter l'accès au conteneur**: Le conteneur ne devrait être utilisé que pour la résolution initiale des dépendances.
3. **Documenter les dépendances**: Les dépendances d'une classe devraient être clairement documentées.
4. **Utiliser des smart pointers**: Préférer `std::shared_ptr` et `std::weak_ptr` aux pointeurs bruts.
5. **Injection par constructeur**: Les dépendances devraient être injectées via le constructeur.
