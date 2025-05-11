# Utilisation des Mocks dans le MIDI Controller

Ce document décrit l'approche des mocks utilisée pour faciliter les tests et l'injection de dépendances dans le projet MIDI Controller.

## Qu'est-ce qu'un Mock?

Un mock est une implémentation simplifiée d'une interface ou d'une classe abstraite utilisée pour les tests ou lorsqu'une implémentation réelle n'est pas disponible ou n'est pas souhaitable. Les mocks permettent:

1. **Tests unitaires isolés**: Tester un composant sans dépendre des implémentations réelles de ses dépendances
2. **Développement parallèle**: Développer des composants qui dépendent d'autres composants non encore implémentés
3. **Implémentation minimale**: Fournir uniquement les fonctionnalités nécessaires pour un test ou un scénario spécifique

## Types de Mocks dans le Projet

### 1. DisplayMock / IDisplayMock

Ces mocks implémentent les interfaces d'affichage et permettent de développer et tester les composants d'UI sans dépendre d'un écran physique:

```cpp
class IDisplayMock : public IDisplay {
public:
    void clear() override {}
    void display() override {}
    // ... autres méthodes d'affichage ...
};
```

### 2. ViewManagerMock

Ce mock implémente la classe abstraite ViewManager et permet de tester les composants qui dépendent d'un gestionnaire de vues:

```cpp
class ViewManagerMock : public ViewManager {
public:
    void enterMenu() override {}
    void exitMenu() override {}
    // ... autres méthodes de gestion de vues ...
};
```

## Avantages de l'Approche

1. **Compilation sans matériel**: Le code peut être compilé et testé sans nécessiter le matériel physique
2. **Tests plus robustes**: Les tests unitaires peuvent s'exécuter de manière déterministe sans dépendre d'entrées/sorties réelles
3. **Isolation des problèmes**: Les problèmes peuvent être isolés plus facilement en utilisant des mocks pour les composants non affectés

## Utilisation dans les Tests

Pour utiliser ces mocks dans les tests:

```cpp
// Créer des instances de mock
auto displayMock = std::make_shared<IDisplayMock>();
auto viewManagerMock = std::make_shared<ViewManagerMock>(*displayMock);

// Injecter dans le composant à tester
auto uiController = std::make_shared<UIController>(*viewManagerMock, *menuController);

// Tester le composant
uiController->navigateToMainScreen();
// Vérifier que les méthodes appropriées ont été appelées sur viewManagerMock
```

## Extension des Mocks

Pour étendre les capacités des mocks existants ou créer de nouveaux mocks:

1. Identifier l'interface ou la classe abstraite à mocker
2. Créer une classe qui implémente toutes les méthodes virtuelles pures
3. Fournir des implémentations simples ou enregistrer les appels pour les tests
4. Ajouter le mock à la bibliothèque de mocks du projet

Cette approche permet d'augmenter progressivement la couverture de tests et de faciliter le développement de nouvelles fonctionnalités.
