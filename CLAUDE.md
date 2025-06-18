# CLAUDE.md

Ce fichier fournit des conseils à Claude Code (claude.ai/code) lors du travail avec le code de ce dépôt.

## Vue d'ensemble du projet

Il s'agit d'un projet de contrôleur MIDI pour la plateforme Teensy utilisant PlatformIO. Il implémente un contrôleur MIDI complet avec :
- Encodeurs rotatifs, boutons et autres contrôles
- Écran TFT ILI9341 avec bibliothèque graphique TGX pour l'interface utilisateur
- Communication MIDI via USB
- Gestion de profils et configurations personnalisables

## Architecture

Le projet suit une Architecture Hexagonale (aussi appelée Architecture en Oignon) avec injection de dépendances :

### Couches principales
- **Core** : Contient la logique métier indépendante de la plateforme
  - **Domain** : Entités principales, interfaces et événements
  - **Use Cases** : Implémentation de la logique métier (ProcessButtons, ProcessEncoders, ProcessMidiIn, UpdateDisplay, LearnMidiMapping)
  - **Ports** : Interfaces abstraites pour les adaptateurs
  - **Controllers** : Orchestration de haut niveau (InputController, MenuController, ProfileController, UIController)

- **Adapters** : Implémentations concrètes
  - **Primary** : Adaptateurs d'entrée (vues UI et gestion de l'affichage)
  - **Secondary** : Adaptateurs de sortie (interfaces matérielles, sortie MIDI, stockage)

- **App** : Point d'entrée et orchestration de l'application
  - **Subsystems** : Sous-systèmes modulaires implémentant des interfaces abstraites (IConfiguration, IInputSystem, IMidiSystem, IUISystem)
  - **DI** : Système moderne d'injection de dépendances via DependencyContainer

## Commandes de développement courantes

### Compiler le projet
```bash
pio run
```

### Compiler pour un environnement spécifique
```bash
pio run -e MIDI_CONTROLLER_DEVELOPMENT  # Build développement avec debug
pio run -e MIDI_CONTROLLER_PRODUCTION   # Build production
pio run -e MIDI_CONTROLLER_DEBUG       # Build debug verbeux
```

### Téléverser vers le Teensy
```bash
pio run -t upload
```

### Monitorer la sortie série
```bash
pio device monitor
```

### Exécuter les tests
```bash
pio test
```

### Formater le code (Windows PowerShell)
```powershell
.\format-all.ps1
```

## Détails techniques clés

### Configuration de la plateforme
- **Carte** : Teensy 4.1
- **Framework** : Arduino
- **Standard C++** : C++20
- **Type USB** : USB_MIDI_SERIAL
- **Affichage** : TFT ILI9341 avec bibliothèque graphique TGX

### Gestion de la mémoire
- Préférer l'allocation statique à l'allocation dynamique
- Utiliser des structures de données à taille fixe (tableaux statiques au lieu de std::vector)
- Passer les gros objets par référence constante
- Utiliser des buffers circulaires pour les opérations de streaming (messages MIDI)

### Standards de code
- Variables membres : suffixe underscore (ex : `config_`, `display_`)
- Classes : PascalCase
- Interfaces : préfixe "I" + PascalCase (ex : `IInputSystem`)
- Méthodes : camelCase
- Utiliser `Result<T, E>` pour les opérations qui peuvent échouer
- Éviter les exceptions (coûteuses dans les systèmes embarqués)
- Utiliser les commentaires Doxygen pour l'API publique

### Contraintes temps réel
- Éviter les délais bloquants (`delay()`)
- Utiliser des machines à états ou des timers non-bloquants
- Assurer la précision du timing MIDI
- Mesurer et optimiser les performances des fonctions critiques

## Ajouter de nouvelles fonctionnalités

Pour ajouter un nouveau sous-système :

1. Définir l'interface dans `core/domain/interfaces/IFeatureName.hpp`
2. Créer le sous-système dans `app/subsystems/FeatureNameSubsystem.hpp`
3. Enregistrer dans `InitializationScript.hpp` :
   ```cpp
   auto feature = std::make_shared<FeatureNameSubsystem>(container);
   container->registerDependency<FeatureNameSubsystem>(feature);
   container->registerDependency<IFeatureName>(feature);
   ```
4. Initialiser dans `MidiControllerApp::initializeSubsystems()`

## Tests
- Framework : Unity
- Répertoire de tests : `test/` (configuré dans platformio.ini mais peut nécessiter création)
- Les tests doivent couvrir les scénarios unitaires et d'intégration
- Utiliser le mocking pour la simulation du matériel

## Fichiers importants
- `src/main.cpp` : Point d'entrée
- `src/app/MidiControllerApp.cpp` : Classe principale de l'application
- `src/app/InitializationScript.hpp` : Configuration de l'injection de dépendances
- `src/config/unified/UnifiedConfiguration.cpp` : Gestion de la configuration
- `platformio.ini` : Configuration de compilation

## Memories

- Concernant l'intégration LVGL (2024-01-01) : Plan d'action détaillé à élaborer dans `docs/ui/lvgl_integration.md` avec une approche structurée pour l'intégration de la bibliothèque graphique LVGL dans le contrôleur MIDI
- Toutes les futures commandes pio doivent pointer vers l'exécutable situé dans %USERPROFILE%\.platformio\penv\Scripts. Assurez-vous que c'est bien le cas, même pour les commandes exécutées depuis WSL