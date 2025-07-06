# CLAUDE.md

Ce fichier fournit des conseils à Claude Code (claude.ai/code) lors du travail avec le code de ce dépôt.

## Vue d'ensemble du projet

Il s'agit d'un projet de contrôleur MIDI pour la plateforme Teensy utilisant PlatformIO. Il implémente un contrôleur MIDI complet avec :
- Encodeurs rotatifs, boutons et autres contrôles
- Écran TFT ILI9341 avec bibliothèque graphique LVGL 9.3.0 pour l'interface utilisateur
- Communication MIDI via USB
- Gestion de profils et configurations personnalisables
- Architecture hexagonale avec injection de dépendances moderne

## Architecture

Le projet suit une Architecture Hexagonale (aussi appelée Architecture en Oignon) avec injection de dépendances :

### Couches principales
- **Core** : Contient la logique métier indépendante de la plateforme
  - **Domain/Interfaces** : Interfaces pures (IInputManager, IUIManager, IViewFactory, IDisplayManager, IViewManager)
  - **Use Cases** : Implémentation de la logique métier (ProcessButtons, ProcessEncoders)
  - **Controllers** : Orchestration de haut niveau (InputController, MenuController, UIController)
  - **Commands** : Pattern Command pour les opérations (NavigateMenuCommand, SendMidiCCCommand)

- **App** : Orchestration et services applicatifs
  - **Services** : Services métier (InputManagerService, NavigationConfigService)
  - **Factories** : Création d'objets avec DI (ViewFactory)
  - **Subsystems** : Sous-systèmes modulaires (ConfigurationSubsystem, InputSubsystem, MidiSubsystem, UISubsystem)
  - **DI** : Injection de dépendances via DependencyContainer

- **Adapters** : Implémentations concrètes
  - **UI** : Adapters UI (UISystemAdapter, vues LVGL)
  - **Secondary** : Hardware (DisplayManagerAdapter, gestionnaires input), MIDI, Storage

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
- **Affichage** : TFT ILI9341 avec bibliothèque graphique LVGL 9.3.0

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

## Architecture détaillée

### Structure actuelle des interfaces (Core)
Le core définit les contrats via des interfaces pures :
- `IInputManager` : Gestion centralisée des entrées utilisateur
- `IUIManager` : Gestion de l'interface utilisateur  
- `IViewFactory` : Création de gestionnaires de vues
- `IDisplayManager` : Gestion optimisée de l'affichage
- `IViewManager` : Navigation entre les vues
- `IInputSystem`, `IMidiSystem`, `IUISystem` : Interfaces de sous-systèmes

### Services applicatifs (App)
- **InputManagerService** : Implémente IInputManager, coordonne les gestionnaires hardware
- **NavigationConfigService** : Configuration de la navigation
- **ViewFactory** : Création de ViewManager avec injection de dépendances

### Adapters
- **UISystemAdapter** : Adapter principal UI, remplace l'ancien UISystemCore
- **DisplayManagerAdapter** : Gestion de l'affichage avec timing optimisé
- **Vues LVGL** : LvglMenuView, LvglParameterView, LvglModalView, LvglSplashScreenView

### Injection de dépendances
Le système utilise DependencyContainer pour :
- Enregistrement des services et interfaces
- Résolution automatique des dépendances
- Configuration centralisée dans InitializationScript

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
- `src/main.cpp` : Point d'entrée minimal
- `src/app/SystemManager.cpp` : Gestionnaire système principal
- `src/app/MidiControllerApp.cpp` : Application principale
- `src/app/InitializationScript.hpp` : Configuration de l'injection de dépendances
- `src/app/services/InputManagerService.hpp` : Service centralisé de gestion des entrées
- `src/adapters/ui/UISystemAdapter.hpp` : Adapter principal UI
- `src/core/domain/interfaces/` : Interfaces de l'architecture hexagonale
- `src/config/unified/UnifiedConfiguration.cpp` : Gestion de la configuration
- `platformio.ini` : Configuration de compilation

## État actuel du projet

### Migration Architecture Hexagonale ✅ TERMINÉE
- **Classes migrées** : InputManager → InputManagerService, UISystemCore → UISystemAdapter
- **Interfaces créées** : IInputManager, IUIManager, IViewFactory, IDisplayManager, IViewManager  
- **Dépendances inversées** : Commands et Controllers utilisent maintenant des interfaces
- **Compilation** : ✅ Projet compile sans erreurs

### Technologies actuelles
- **Interface graphique** : LVGL 9.3.0 (migration TGX → LVGL terminée)
- **Conteneurs temps réel** : ETL (Embedded Template Library) 20.39.4
- **Architecture** : Hexagonale avec injection de dépendances
- **Gestion d'erreurs** : Pattern Result<T, E>

### Composants clés post-migration
- `InputManagerService` : Service centralisé remplaçant InputManager
- `UISystemAdapter` : Adapter UI remplaçant UISystemCore  
- `DisplayManagerAdapter` : Gestion optimisée de l'affichage
- `ViewFactory` : Factory moderne avec DI dans app/factories/

## Memories

- **Migration Architecture Hexagonale (2025-01-06)** : Migration complète réalisée avec succès
  - Création de 5 nouvelles interfaces dans core/domain/interfaces/
  - Déplacement de 4 classes vers les bonnes couches architecturales
  - Correction de toutes les violations de dépendances
  - Projet compile et respecte l'architecture hexagonale

- Concernant l'intégration LVGL : ✅ TERMINÉE - Le projet utilise maintenant LVGL 9.3.0 avec succès