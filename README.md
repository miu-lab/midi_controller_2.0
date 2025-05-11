# MIDI Controller

MIDI Controller est un projet de contrôleur MIDI pour la plateforme Teensy, utilisant PlatformIO pour la gestion du projet et la compilation.

## Architecture

Le projet est organisé selon une architecture hexagonale (aussi appelée architecture en oignon) avec injection de dépendances:

- **Core** : Contient la logique métier principale, indépendante de l'implémentation
  - **Domain** : Définitions des entités et interfaces principales
  - **Use Cases** : Implémentation des cas d'utilisation
  - **Ports** : Interfaces pour les adaptateurs
  - **Interfaces** : Contrats abstraits pour les sous-systèmes

- **Adapters** : Implémentations concrètes des interfaces
  - **Primary** : Adaptateurs d'entrée (UI, contrôles)
  - **Secondary** : Adaptateurs de sortie (MIDI, stockage)

- **App** : Point d'entrée de l'application et orchestration
  - **Subsystems** : Sous-systèmes modulaires encapsulant des fonctionnalités complètes
  - **DI** : Système d'injection de dépendances moderne

### Injection de dépendances

Le projet utilise un système moderne d'injection de dépendances basé sur:

- `DependencyContainer`: Conteneur générique pour enregistrer et résoudre les dépendances
- `ServiceLocatorAdapter`: Adaptateur pour assurer la compatibilité avec le code existant

Pour plus d'informations, voir [DependencyInjection.md](docs/DependencyInjection.md).

### Sous-systèmes

L'application est divisée en sous-systèmes indépendants, chacun implémentant une interface abstraite:

- **ConfigurationSubsystem** (`IConfiguration`): Gestion des paramètres et préférences
- **InputSubsystem** (`IInputSystem`): Traitement des entrées utilisateur
- **MidiSubsystem** (`IMidiSystem`): Communication MIDI et mapping
- **UISubsystem** (`IUISystem`): Interface utilisateur et affichage

## Conventions de code

Le projet suit les conventions définies dans le fichier [CodingStandard.md](CodingStandard.md). En résumé :

- Membres de classe : suffixe underscore (`_`) 
- Variables locales : camelCase sans suffixe
- Noms de classes : PascalCase
- Interfaces : préfixe "I" + PascalCase
- Méthodes : camelCase
- Documentation Doxygen pour l'API publique

## Gestion des erreurs

Le projet utilise `Result<T, E>` pour la gestion des erreurs, permettant un traitement explicite sans exceptions. Voir [GestionErreurs.md](docs/GestionErreurs.md) pour plus d'informations.

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

Le projet comprend une infrastructure de tests unitaires et d'intégration basée sur Unity:

```bash
pio test
```

Les tests couvrent les composants individuels ainsi que l'intégration des sous-systèmes.

## Documentation

La documentation complète se trouve dans le dossier `docs/`:

- [MigrationGuide.md](docs/MigrationGuide.md): Guide de migration vers la nouvelle architecture
- [DependencyInjection.md](docs/DependencyInjection.md): Utilisation du système d'injection de dépendances
- [GestionErreurs.md](docs/GestionErreurs.md): Guide d'utilisation de Result<T, E>
