# Nouvelle Architecture MIDI Controller

Ce document présente la nouvelle architecture d'injection de dépendances mise en place pour le MIDI Controller.

## Principes fondamentaux

La nouvelle architecture repose sur les principes suivants:

1. **Injection de dépendances directe**: Toutes les dépendances sont injectées explicitement via les constructeurs
2. **Interfaces abstraites**: Utilisation d'interfaces pour découpler les composants
3. **Sous-systèmes modulaires**: Organisation en sous-systèmes cohérents et indépendants
4. **Smart pointers**: Utilisation systématique de `std::shared_ptr` pour la gestion mémoire
5. **Pas de Singletons**: Élimination des singletons et locators globaux

## Structure principale

L'application est organisée en plusieurs composants principaux:

### 1. DependencyContainer

Le `DependencyContainer` est le moteur central de l'injection de dépendances. Il permet:
- D'enregistrer des instances (via `registerDependency`)
- D'enregistrer des implémentations d'interfaces (via `registerImplementation`)
- De récupérer des instances (via `resolve`)
- D'enregistrer des factories pour créer des instances à la demande

### 2. Interfaces des sous-systèmes

Quatre interfaces principales définissent les contrats entre sous-systèmes:
- `IConfiguration`: Gestion des configurations
- `IInputSystem`: Gestion des périphériques d'entrée
- `IMidiSystem`: Communications MIDI
- `IUISystem`: Interface utilisateur

### 3. Implémentations des sous-systèmes

Chaque interface est implémentée par un sous-système concret:
- `ConfigurationSubsystem`: Implémente `IConfiguration`
- `InputSubsystem`: Implémente `IInputSystem`
- `MidiSubsystem`: Implémente `IMidiSystem`
- `UISubsystem`: Implémente `IUISystem`

### 4. MidiControllerApp

`MidiControllerApp` est le point d'entrée principal de l'application. Il:
- Coordonne les différents sous-systèmes
- Gère le cycle de vie de l'application
- Orchestre l'initialisation et les mises à jour

## Initialisation de l'application

L'initialisation se fait en plusieurs étapes:

1. Création du `DependencyContainer`
2. Enregistrement des adaptateurs de base (display, midiOut, etc.)
3. Création et enregistrement des sous-systèmes
4. Création de l'application avec le conteneur
5. Initialisation des sous-systèmes dans l'ordre approprié

```cpp
// Création du conteneur
auto container = std::make_shared<DependencyContainer>();

// Enregistrement des adaptateurs
auto display = std::make_shared<Ssd1306Display>();
container->registerDependency<IDisplay>(display);

auto midiOut = std::make_shared<TeensyUsbMidiOut>();
container->registerDependency<IMidiOut>(midiOut);

// Création des sous-systèmes
auto configSystem = std::make_shared<ConfigurationSubsystem>(container);
container->registerDependency<IConfiguration>(configSystem);

// ...

// Création et initialisation de l'application
auto app = std::make_shared<MidiControllerApp>(container);
app->init();
```

## Flux de dépendances

Le flux de dépendances est unidirectionnel:

1. Les adaptateurs dépendent uniquement des interfaces bas niveau
2. Les sous-systèmes dépendent des adaptateurs et des interfaces
3. L'application dépend des interfaces des sous-systèmes

Cela permet de facilement remplacer n'importe quel composant sans affecter le reste du système.

## Exemple d'utilisation

Voici un exemple d'utilisation de l'injection de dépendances pour une classe:

```cpp
class InputController {
public:
    // Injection explicite via le constructeur
    explicit InputController(std::shared_ptr<NavigationConfigService> navigationConfig);
    
    // Injection supplémentaire via setter
    void setUIController(std::shared_ptr<UIController> uiController);
    
    // ...
private:
    std::shared_ptr<NavigationConfigService> m_navigationConfig;
    std::shared_ptr<UIController> m_uiController;
};
```

## Avantages de la nouvelle architecture

Cette architecture apporte plusieurs avantages:

1. **Testabilité**: Les composants peuvent être testés avec des mocks
2. **Modularité**: Les composants sont découplés et peuvent évoluer indépendamment
3. **Clarté**: Les dépendances sont explicites et visibles dans les signatures
4. **Flexibilité**: Les implémentations peuvent être facilement remplacées
5. **Sécurité mémoire**: Utilisation systématique de smart pointers
6. **Maintenabilité**: Structure cohérente et prévisible