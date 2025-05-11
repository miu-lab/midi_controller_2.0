# Analyse des dépendances du projet MIDI Controller

## 1. Structure globale

Le projet MIDI Controller suit une architecture hexagonale (ports et adaptateurs) avec les composants suivants :

- **Core** : Contient la logique métier et les cas d'utilisation
- **Adapters** : Contient les adaptateurs primaires (UI) et secondaires (matériel, MIDI, stockage)
- **App** : Coordonne les différentes parties de l'application
- **Config** : Contient la configuration de l'application
- **Tools** : Contient des outils de diagnostic et de débogage

## 2. Points centraux de dépendances

### 2.1. ServiceLocator

Le `ServiceLocator` est actuellement le point central de gestion des dépendances. Il fonctionne comme un registre global où les services sont enregistrés et récupérés. Cela présente plusieurs problèmes :

- **Dépendance globale** : Toutes les classes qui ont besoin d'un service doivent connaître le `ServiceLocator` et dépendre de lui
- **Initialisation implicite** : L'ordre d'initialisation des services n'est pas explicite
- **Couplage fort** : Les composants sont fortement couplés au `ServiceLocator` plutôt qu'à leurs dépendances directes
- **Testabilité réduite** : Le caractère global rend les tests unitaires plus difficiles

### 2.2. MidiControllerApp

La classe `MidiControllerApp` est actuellement responsable de :
- L'initialisation de tous les services
- L'enregistrement des services dans le `ServiceLocator`
- La coordination des updates de tous les systèmes
- La gestion des événements à travers le `EventBus`

Cette concentration de responsabilités viole le principe de responsabilité unique.

### 2.3. EventBus

Le système d'événements (`EventBus`) est utilisé pour découpler certaines parties de l'application, mais il crée également des dépendances implicites difficiles à suivre.

## 3. Redondances identifiées

### 3.1. Systèmes d'entrée

Il existe deux implémentations qui font essentiellement la même chose :
- `InputSystem` (app/services/InputSystem.hpp)
- `EventInputSystem` (app/services/EventInputSystem.hpp)

`EventInputSystem` hérite de `InputSystem` pour des raisons de compatibilité, mais réimplémente toutes ses fonctionnalités. Cette redondance complique la maintenance et crée de la confusion.

### 3.2. Gestion des configurations

Il existe plusieurs services liés à la configuration, avec des responsabilités qui se chevauchent :
- `ConfigurationService`
- `NavigationConfigService`
- `ApplicationConfiguration`

## 4. Cycles de dépendances problématiques

### 4.1. InputSystem <-> UIController

Il existe une dépendance circulaire où :
- `InputSystem` a besoin de `InputController` et de `UIController`
- `UIController` a besoin d'événements générés par `InputSystem`

### 4.2. MidiSystem <-> ProfileManager

- `MidiSystem` dépend de `ProfileManager`
- `ProfileManager` peut affecter le comportement de `MidiSystem`

### 4.3. ServiceLocator et ses services

- Plusieurs services enregistrés dans `ServiceLocator` dépendent d'autres services également enregistrés
- L'ordre d'initialisation et les dépendances entre ces services ne sont pas explicites

## 5. Opportunités d'amélioration

### 5.1. Remplacement du ServiceLocator

Le `ServiceLocator` peut être remplacé par un système d'injection de dépendances plus moderne et explicite comme `DependencyContainer`.

### 5.2. Unification des systèmes d'entrée

Les systèmes `InputSystem` et `EventInputSystem` peuvent être unifiés en une seule implémentation claire avec une interface bien définie.

### 5.3. Organisation en sous-systèmes cohérents

Regrouper les fonctionnalités en sous-systèmes indépendants et cohérents :
- `ConfigurationSubsystem`
- `InputSubsystem`
- `MidiSubsystem`
- `UISubsystem`

### 5.4. Interfaces claires

Définir des interfaces claires pour chaque sous-système afin de réduire le couplage et d'améliorer la testabilité.

### 5.5. Gestion explicite du cycle de vie des objets

Adopter l'utilisation de smart pointers (`std::unique_ptr`, `std::shared_ptr`) pour gérer explicitement le cycle de vie des objets.

## 6. Plan d'action pour le refactoring

1. Créer les interfaces pour chaque sous-système
2. Implémenter un `DependencyContainer` pour gérer les dépendances
3. Créer un adaptateur `ServiceLocatorAdapter` pour assurer la compatibilité avec le code existant
4. Implémenter progressivement chaque sous-système
5. Refactoriser `MidiControllerApp` pour utiliser les sous-systèmes
6. Supprimer les redondances et les classes obsolètes
