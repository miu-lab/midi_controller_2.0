# Architecture MIDI Controller

Ce document décrit l'architecture du projet MIDI Controller après refactoring.

## Vue d'ensemble

L'application utilise une architecture modulaire basée sur les principes SOLID, avec une séparation claire des responsabilités et l'injection de dépendances. Cette approche facilite les tests, la maintenabilité et les évolutions futures.

## Composants principaux

### Injection de dépendances

- **DependencyContainer** (`app/di/DependencyContainer.hpp`) : Conteneur central pour l'injection de dépendances, permettant l'enregistrement et la résolution de dépendances typées.

### Interfaces abstraites

Les composants communiquent via des interfaces clairement définies :

- **IConfiguration** (`core/domain/interfaces/IConfiguration.hpp`) : Gestion des configurations.
- **IInputSystem** (`core/domain/interfaces/IInputSystem.hpp`) : Gestion des entrées utilisateur.
- **IMidiSystem** (`core/domain/interfaces/IMidiSystem.hpp`) : Communications MIDI.
- **IUISystem** (`core/domain/interfaces/IUISystem.hpp`) : Interface utilisateur.

### Sous-systèmes

Chaque sous-système implémente une interface et encapsule un ensemble de responsabilités :

- **ConfigurationSubsystem** (`app/subsystems/ConfigurationSubsystem.hpp`) : Gestion centralisée des paramètres.
- **InputSubsystem** (`app/subsystems/InputSubsystem.hpp`) : Lecture et traitement des entrées.
- **MidiSubsystem** (`app/subsystems/MidiSubsystem.hpp`) : Communications MIDI et mappings.
- **UISubsystem** (`app/subsystems/UISubsystem.hpp`) : Affichage et interactions utilisateur.

### Application

- **MidiControllerApp** (`app/MidiControllerApp.hpp`) : Point d'entrée principal qui coordonne les sous-systèmes.

## Gestion des erreurs

L'application utilise le type `Result<T, E>` pour une gestion des erreurs robuste et expressive, sans utiliser d'exceptions (incompatibles avec l'environnement Arduino/Teensy).

## Cycle de vie des composants

1. **Initialisation** : L'application crée et configure les sous-systèmes dans un ordre précis qui respecte leurs dépendances.
2. **Boucle principale** : La méthode `update()` est appelée périodiquement pour mettre à jour l'état de tous les sous-systèmes.
3. **Nettoyage** : Les ressources sont libérées dans l'ordre inverse de leur création.

## Testabilité

L'architecture a été conçue pour faciliter les tests :
- Les interfaces peuvent être mockées pour les tests unitaires
- L'injection de dépendances permet de substituer des implémentations réelles par des mocks
- Les tests d'intégration vérifient l'interaction entre les composants

## Diagramme simplifié

```
                   +-------------------+
                   | MidiControllerApp |
                   +-------------------+
                            |
                            v
                   +-------------------+
                   | DependencyContainer|
                   +-------------------+
                            |
              +-------------+-------------+
              |             |             |
              v             v             v
    +-------------------+  +---+  +---------------+
    |  IConfiguration   |  |...+  |   IUISystem   |
    +-------------------+  +---+  +---------------+
              |                            |
              v                            v
+---------------------------+  +-------------------------+
| ConfigurationSubsystem    |  | UISubsystem             |
+---------------------------+  +-------------------------+
```

## Conseils pour les développeurs

1. Toujours privilégier les interactions via les interfaces plutôt que via des implémentations concrètes.
2. Utiliser le `DependencyContainer` pour la résolution de dépendances plutôt que de créer des instances directement.
3. Vérifier les résultats des opérations avec `Result<T, E>` pour une gestion robuste des erreurs.
4. Ajouter des tests unitaires pour les nouveaux composants ou les modifications significatives.
