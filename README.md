# MIDI Controller

MIDI Controller est un projet de contrôleur MIDI pour la plateforme Teensy, utilisant PlatformIO pour la gestion du projet et la compilation.

## Architecture

Le projet est organisé selon une architecture hexagonale (aussi appelée architecture en oignon) :

- **Core** : Contient la logique métier principale, indépendante de l'implémentation
  - **Domain** : Définitions des entités et interfaces principales
  - **Use Cases** : Implémentation des cas d'utilisation
  - **Ports** : Interfaces pour les adaptateurs

- **Adapters** : Implémentations concrètes des interfaces
  - **Primary** : Adaptateurs d'entrée (UI, contrôles)
  - **Secondary** : Adaptateurs de sortie (MIDI, stockage)

- **App** : Point d'entrée de l'application et orchestration
  - **Subsystems** : Sous-systèmes spécialisés encapsulant des fonctionnalités complètes
  - **DI** : Système d'injection de dépendances

## Conventions de code

Le projet suit les conventions définies dans le fichier [CodingStandard.md](CodingStandard.md). En résumé :

- Membres de classe : suffixe underscore (`_`) 
- Variables locales : camelCase sans suffixe
- Noms de classes : PascalCase
- Interfaces : préfixe "I" + PascalCase
- Méthodes : camelCase
- Documentation Doxygen pour l'API publique

## Gestion des erreurs

À partir de la version 2.0, le projet utilise `Result<T, E>` pour la gestion des erreurs. Voir [GestionErreurs.md](docs/GestionErreurs.md) pour plus d'informations.

## Compilation et déploiement

### Prérequis
- PlatformIO
- Teensy Loader
- Librairies Arduino requises (voir platformio.ini)

### Compilation
```bash
pio run
```

### Upload vers le Teensy
```bash
pio run -t upload
```

## Tests

Le projet comprend une infrastructure de tests unitaires et d'intégration.

```bash
pio test
```

## Documentation

La documentation complète se trouve dans le dossier `docs/`.