# Tests pour le Contrôleur MIDI

Ce dossier contient les tests automatisés pour le projet de contrôleur MIDI utilisant Unity comme framework de test.

## Structure des tests

- `/test` - Dossier racine des tests
  - `/mocks` - Implémentations de simulation pour les tests
  - `/integration` - Tests d'intégration entre composants
  - `/unit` - Tests unitaires pour chaque composant (à implémenter)

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

## Mocks disponibles

- `MockInput` - Simulation du système d'entrée (boutons, encodeurs)
- `MockMidi` - Simulation du système MIDI (envoi/réception de messages)
- `MockUI` - Simulation du système d'interface utilisateur (affichage)

## Conventions de test

1. Chaque test doit inclure les fonctions `setUp()` et `tearDown()`
2. Chaque test doit être indépendant et ne pas dépendre d'autres tests
3. Le nom des tests doit suivre le format `test_[composant]_[fonction]`
4. Les assertions Unity doivent être utilisées pour vérifier les résultats

## Extensions prévues

- Tests de performance pour vérifier le temps d'exécution
- Tests de robustesse pour vérifier la gestion des erreurs
- Tests d'état pour vérifier la gestion correcte des états de l'application
