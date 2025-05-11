# Stratégie de Test pour le projet MIDI Controller

## Structure des tests

Pour le projet MIDI Controller, nous avons adopté une structure de tests compatible avec PlatformIO et le framework Unity, optimisée pour l'environnement Teensy. Cette structure permet de tester efficacement le code tout en tenant compte des contraintes spécifiques aux systèmes embarqués.

### Organisation des fichiers

```
test/
|-- test_main.cpp           # Point d'entrée principal pour tous les tests
|-- di_tests.h              # Tests pour le système d'injection de dépendances
|-- mock_tests.h            # Tests pour les mocks d'interfaces
|-- mocks/                  # Implémentations simulées pour les tests
|   |-- MockInput.h         # Simulation du système d'entrée
|   |-- MockMidi.h          # Simulation du système MIDI
|   |-- MockUI.h            # Simulation du système d'interface utilisateur
|-- integration/            # Tests d'intégration entre composants
```

### Approche de test

Nous utilisons une approche de test à plusieurs niveaux :

1. **Tests unitaires** : Tests isolés pour chaque composant
2. **Tests d'intégration** : Tests des interactions entre composants
3. **Tests de validation** : Tests du système dans son ensemble

### Convention de tests

Pour éviter les conflits de définitions multiples avec Unity et PlatformIO, nous suivons ces conventions :

1. **Un seul fichier principal** : Seul `test_main.cpp` contient les fonctions `setUp()`, `tearDown()` et `main()`
2. **Tests dans des fichiers d'en-tête** : Les fonctions de test sont définies dans des fichiers .h pour éviter les définitions multiples
3. **Organisation par domaine** : Les tests sont regroupés par domaine fonctionnel pour une meilleure organisation

## Exécution des tests

Les tests peuvent être exécutés avec PlatformIO en utilisant les commandes suivantes :

```bash
# Exécuter tous les tests
pio test -e dev

# Voir la sortie détaillée
pio test -e dev -v

# Exécuter des tests spécifiques
pio test -e dev -f "*di*"
```

## Mocks de test

Pour faciliter les tests, nous avons créé des implémentations simulées (mocks) des interfaces principales :

### MockInput (IInputSystem)
- Simule le comportement du système d'entrée
- Enregistre les appels aux méthodes pour validation
- Permet de tester le traitement des entrées sans matériel réel

### MockMidi (IMidiSystem)
- Simule le comportement du système MIDI
- Enregistre les messages MIDI envoyés pour validation
- Permet de tester la logique MIDI sans périphériques réels

### MockUI (IUISystem)
- Simule le comportement du système d'interface utilisateur
- Enregistre les messages affichés pour validation
- Permet de tester l'interaction UI sans écran physique

## Tests du système d'injection de dépendances

Le système d'injection de dépendances (`DependencyContainer`) est testé pour garantir :

1. L'enregistrement et la résolution correcte des dépendances
2. La gestion des implémentations d'interfaces
3. La création paresseuse via des factories
4. La vérification d'existence et la suppression de dépendances

Ces tests valident que notre nouveau système d'injection de dépendances fonctionne correctement et pourra remplacer le `ServiceLocator` existant.

## Extensions futures

À mesure que le projet évolue, nous prévoyons d'étendre notre stratégie de test avec :

1. **Tests spécifiques pour chaque sous-système** (Configuration, Input, MIDI, UI)
2. **Tests de performance** pour garantir que le code refactorisé répond aux exigences de performance
3. **Tests de robustesse** pour valider la gestion des erreurs et des cas limites
