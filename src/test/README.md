# Architecture du système de test

Cette architecture modulaire sépare les responsabilités pour un code plus propre et maintenable.

## Structure des fichiers

```
src/test/
├── commands/
│   ├── TestCommand.hpp              # Interface de base pour les commandes
│   ├── TestCommandRegistry.hpp      # Registry pour enregistrer les commandes
│   └── DisplayTestCommands.hpp      # Commandes spécifiques aux tests display
├── HardwareTestMode.hpp            # Mode de test hardware complet
├── TestCommandHandler.hpp           # Handler principal pour gérer les commandes
└── README.md                        # Ce fichier
```

## Composants principaux

### 1. TestCommand
Interface de base pour toutes les commandes de test. Chaque commande a :
- Une touche (char) pour l'activer
- Une description
- Une fonction execute()

### 2. TestCommandRegistry
Registry qui gère l'enregistrement et l'exécution des commandes :
- `registerCommand()` : Enregistre une nouvelle commande
- `executeCommand()` : Exécute la commande correspondant à une touche
- `printHelp()` : Affiche toutes les commandes disponibles

### 3. TestCommandHandler
Handler principal qui :
- Gère l'input Serial
- Délègue l'exécution au registry
- Affiche les prompts et messages

### 4. DisplayTestCommands
Classe statique qui enregistre toutes les commandes de test du display :
- Tests hardware (rotations, framebuffer, endurance)
- Tests LVGL (clear screen, demo, widgets)
- Tests performance
- Diagnostics mémoire

### 5. HardwareTestMode
Mode de test complet qui orchestre tout :
- Initialise le driver hardware
- Initialise le bridge LVGL
- Crée la suite de tests
- Configure les commandes
- Gère la loop principale

## Utilisation dans main.cpp

```cpp
#include "test/HardwareTestMode.hpp"

std::unique_ptr<HardwareTestMode> testMode;

void setup() {
    testMode = std::make_unique<HardwareTestMode>();
    if (!testMode->init()) {
        // Erreur fatale
    }
}

void loop() {
    testMode->update();
}
```

## Ajout de nouvelles commandes

Pour ajouter une nouvelle commande :

1. Dans DisplayTestCommands.hpp ou créer une nouvelle classe :
```cpp
registry.registerCommand('X', "Description de la commande", [&]() {
    // Code de la commande
});
```

2. Les commandes peuvent capturer les références nécessaires (driver, bridge, testSuite)

## Avantages de cette architecture

1. **Séparation des responsabilités** : Chaque composant a un rôle clair
2. **Extensibilité** : Facile d'ajouter de nouvelles commandes
3. **Maintenabilité** : Le code main.cpp reste propre et simple
4. **Réutilisabilité** : Le système de commandes peut être réutilisé pour d'autres tests
5. **Testabilité** : Chaque composant peut être testé indépendamment

## Commandes disponibles

- **T** : Run full test suite
- **R** : Test all rotations
- **E** : Run endurance test
- **F** : Direct framebuffer test
- **P** : Performance benchmark
- **M** : Memory diagnostics
- **C** : Clear screen
- **L** : Simple LVGL test
- **D** : Demo screen
- **W** : ParameterWidget demo
- **H/?** : Show help