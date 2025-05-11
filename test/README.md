# Tests pour le Contrôleur MIDI

Ce dossier contient les tests automatisés pour le projet de contrôleur MIDI utilisant Unity comme framework de test.

## Structure des tests

- `/test` - Dossier racine des tests
  - `/mocks` - Implémentations de simulation pour les tests
  - `/integration` - Tests d'intégration entre composants
  - `/unit` - Tests unitaires pour chaque composant

## Exécution des tests

Pour exécuter les tests, utilisez la commande PlatformIO :

```bash
pio test
```

Pour exécuter un environnement de test spécifique :

```bash
pio test -e dev
```

Pour un test spécifique :

```bash
pio test -e dev -f test_mocks
```

## Tests disponibles

### Tests pour le système d'injection de dépendances

- `test_dependency_container_basic` - Test de base du conteneur de dépendances
- `test_dependency_container_interface` - Test d'enregistrement et résolution d'interfaces
- `test_service_locator_adapter_initialize` - Test d'initialisation de l'adaptateur
- `test_service_locator_adapter_container` - Test d'interaction avec le conteneur
- `test_service_locator_adapter_simple_service` - Test de services simples

### Tests des mocks

- `test_mock_initialization` - Test d'initialisation des mocks
- `test_mock_updates` - Test des mises à jour des mocks
- `test_midi_messages` - Test des messages MIDI
- `test_ui_messages` - Test des messages UI

## Mocks disponibles

- `MockInput` - Simulation du système d'entrée (boutons, encodeurs)
- `MockMidi` - Simulation du système MIDI (envoi/réception de messages)
- `MockUI` - Simulation du système d'interface utilisateur (affichage)

## Conventions de test

1. Chaque test doit inclure les fonctions `setUp()` et `tearDown()`
2. Chaque test doit être indépendant et ne pas dépendre d'autres tests
3. Le nom des tests doit suivre le format `test_[composant]_[fonction]`
4. Les assertions Unity doivent être utilisées pour vérifier les résultats

## Remarques importantes

1. **Types incomplets** : Attention aux forward declarations qui peuvent causer des problèmes lors des tests
2. **Dépendances Arduino** : Certaines classes ont des dépendances à Arduino/Teensy qui peuvent être problématiques dans les tests
3. **Classes abstraites** : Les classes abstraites comme `ViewManager` ne peuvent pas être instanciées directement dans les tests

## Bonnes pratiques

1. **Simplifier les tests** : Préférer des tests simples mais robustes plutôt que des tests complexes et fragiles
2. **Tests indépendants** : Chaque test doit fonctionner indépendamment des autres
3. **Commentaires explicites** : Documenter clairement le but de chaque test
4. **Nettoyage** : Nettoyer l'état après chaque test
5. **Tests ciblés** : Tester une seule fonctionnalité à la fois

## Prochains tests à implémenter

- Tests pour ConfigurationSubsystem
- Tests pour InputSubsystem
- Tests pour MidiSubsystem
- Tests pour UISubsystem
- Tests d'intégration entre les sous-systèmes
