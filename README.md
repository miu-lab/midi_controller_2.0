# MIDI Controller

MIDI Controller est un projet de contrôleur MIDI pour la plateforme Teensy, utilisant PlatformIO pour la gestion du projet et la compilation. Il offre une architecture modulaire et extensible pour le développement d'applications de contrôle MIDI.

## Vue d'ensemble

Ce projet implémente un contrôleur MIDI complet avec:
- Support pour encodeurs rotatifs, boutons et autres contrôles
- Affichage OLED pour l'interface utilisateur
- Communication MIDI via USB
- Gestion de profils et configurations personnalisables

## Architecture

Le projet est organisé selon une architecture hexagonale (aussi appelée architecture en oignon) avec injection de dépendances:

![Architecture du projet](docs/assets/architecture_diagram.png)

### Couches Principales

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

### Sous-systèmes

L'application est divisée en sous-systèmes indépendants, chacun implémentant une interface abstraite:

- **ConfigurationSubsystem** (`IConfiguration`): Gestion des paramètres et préférences
- **InputSubsystem** (`IInputSystem`): Traitement des entrées utilisateur
- **MidiSubsystem** (`IMidiSystem`): Communication MIDI et mapping
- **UISubsystem** (`IUISystem`): Interface utilisateur et affichage

## Fonctionnalités Principales

- **Mapping MIDI flexible**: Assignez facilement des contrôles à différents messages MIDI
- **Interface utilisateur intuitive**: Navigation par menus sur écran OLED
- **Profils multiples**: Sauvegardez et chargez différentes configurations
- **Mode d'apprentissage**: Apprenez facilement de nouveaux mappings MIDI
- **Modularité**: Ajoutez de nouvelles fonctionnalités sans modifier le code existant

## Compilation et Déploiement

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

### Moniteur Série
```bash
pio device monitor
```

## Tests

Le projet comprend une infrastructure de tests unitaires et d'intégration basée sur Unity:

```bash
pio test
```

Les tests couvrent les composants individuels ainsi que l'intégration des sous-systèmes.

## Contribution

Les contributions sont les bienvenues! Veuillez consulter [CONTRIBUTING.md](CONTRIBUTING.md) pour les lignes directrices sur la façon de contribuer au projet.

## Licence

Ce projet est sous licence MIT. Voir le fichier LICENSE pour plus de détails.

## Contact

Pour toute question ou suggestion, veuillez ouvrir une issue sur le dépôt GitHub du projet.
