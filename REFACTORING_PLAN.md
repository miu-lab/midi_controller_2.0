# Plan Détaillé de Refactoring Architectural - MIDI Controller 2.0

## Objectif
Transformer l'architecture actuelle en éliminant les violations SOLID et en créant un code minimal, lisible, sans superflu ni legacy.

## Méthodologie de Validation à Chaque Étape

### **Stratégie de Tests**
- [ ] **Compilation Success**: `pio run -e dev` doit réussir
- [ ] **Tests Unitaires**: `pio test -e dev` pour les nouveaux composants
- [ ] **Tests d'Intégration**: Validation fonctionnelle sur hardware
- [ ] **Tests de Régression**: Validation que les fonctionnalités existantes marchent
- [ ] **Point de Rollback**: Git commit + tag à chaque étape validée

### **Environnements de Test**
- **dev**: Debug activé, logs verbeux
- **prod**: Optimisé, validation performance
- **debug**: Verbose maximum pour diagnostics

---

## PHASE 1: Infrastructure de Tests et Préparatifs

### **Étape 1.1: Mise en Place Infrastructure Tests**
- [ ] **1.1.1**: Créer structure test/ avec test_main.cpp minimal
- [ ] **1.1.2**: Ajouter configuration Unity pour Teensy
- [ ] **1.1.3**: Créer MockMidiOut pour isoler tests MIDI
- [ ] **1.1.4**: Créer MockEventBus pour tests événements
- [ ] **1.1.5**: Créer MockConfiguration avec données test

**Validation Étape 1.1**:
- [ ] `pio run -e dev` compile sans erreur
- [ ] `pio test -e dev` exécute les tests
- [ ] Au moins 1 test dummy passe
- [ ] Mocks compilent correctement

**Point de Rollback**: `git tag v1.1-test-infrastructure`

### **Étape 1.2: Tests de Base Système Existant**
- [ ] **1.2.1**: Test ConfigurationSubsystem::init() avec mock container
- [ ] **1.2.2**: Test UISubsystem::init() avec mocks bridge
- [ ] **1.2.3**: Test InputSubsystem::init() avec mock configs
- [ ] **1.2.4**: Test end-to-end minimal (startup → shutdown)

**Validation Étape 1.2**:
- [ ] `pio test -e dev -f "*integration*"` passe
- [ ] Tous les subsystèmes s'initialisent correctement
- [ ] Aucune régression détectée
- [ ] Tests end-to-end passent

**Point de Rollback**: `git tag v1.2-baseline-tests`

---

## PHASE 2: Refactoring Configuration

### **Étape 2.1: Extraction ConfigurationLoader**
- [ ] **2.1.1**: Créer interface ConfigurationLoader avec méthodes extract
- [ ] **2.1.2**: Extraire logique loadUnifiedConfigurations() de ConfigurationSubsystem
- [ ] **2.1.3**: Créer tests unitaires ConfigurationLoader
- [ ] **2.1.4**: Intégrer ConfigurationLoader dans ConfigurationSubsystem existant

**Fichiers créés**:
```
src/core/configuration/
├── ConfigurationLoader.hpp
└── ConfigurationLoader.cpp
```

**Validation Étape 2.1**:
- [ ] `pio test -e dev -f "*configuration_loader*"` passe
- [ ] `pio test -e dev -f "*integration*"` passe (tests de régression)
- [ ] `pio run -e dev` compile
- [ ] ConfigurationLoader tests unitaires passent
- [ ] Tests d'intégration ConfigurationSubsystem passent encore
- [ ] Aucune régression fonctionnelle
- [ ] Code plus lisible (extraction méthode)

**Point de Rollback**: `git tag v2.1-config-loader-extracted`

### **Étape 2.2: Extraction ConfigurationService**
- [ ] **2.2.1**: Créer ConfigurationService avec méthodes business logic
- [ ] **2.2.2**: Extraire getAllControlDefinitions, getControlDefinitionsByType, etc.
- [ ] **2.2.3**: Créer tests unitaires ConfigurationService
- [ ] **2.2.4**: Intégrer service dans ConfigurationSubsystem (délégation)

**Fichiers créés**:
```
src/core/configuration/
├── ConfigurationService.hpp
└── ConfigurationService.cpp
```

**Fichiers modifiés**:
- `src/app/subsystems/ConfigurationSubsystem.hpp` - Ajout service member
- `src/app/subsystems/ConfigurationSubsystem.cpp` - Délégation aux services

**Validation Étape 2.2**:
- [ ] `pio test -e dev -f "*configuration_service*"` passe
- [ ] `pio test -e dev -f "*configuration_system*"` passe (intégration)
- [ ] `pio run -e dev && upload` (test hardware si possible)
- [ ] ConfigurationService tests unitaires passent
- [ ] ConfigurationSubsystem garde même interface publique
- [ ] Tests d'intégration inchangés
- [ ] Performance équivalente

**Point de Rollback**: `git tag v2.2-config-service-extracted`

### **Étape 2.3: Extraction ConfigurationRegistry**
- [ ] **2.3.1**: Créer ConfigurationRegistry pour logique DI
- [ ] **2.3.2**: Extraire registerImplementation et custom deleters
- [ ] **2.3.3**: Créer tests unitaires ConfigurationRegistry
- [ ] **2.3.4**: Modifier ConfigurationSubsystem pour utiliser registry

**Fichiers créés**:
```
src/core/configuration/
├── ConfigurationRegistry.hpp
└── ConfigurationRegistry.cpp
```

**Validation Étape 2.3**:
- [ ] `pio test -e dev -f "*configuration*"` passe
- [ ] `pio test -e dev -f "*integration*"` passe
- [ ] `pio run -e dev` compile
- [ ] ConfigurationRegistry tests passent
- [ ] ConfigurationSubsystem considérablement simplifié
- [ ] Même comportement externe
- [ ] DI toujours fonctionnel

**Point de Rollback**: `git tag v2.3-config-registry-extracted`

### **Étape 2.4: Finalisation Refactoring Configuration**
- [ ] **2.4.1**: Suppression code redondant dans ConfigurationSubsystem
- [ ] **2.4.2**: Suppression méthodes inline devenues obsolètes
- [ ] **2.4.3**: Suppression custom deleters complexes (remplacés par registry)
- [ ] **2.4.4**: Tests complets Phase 2

**Validation Complète Phase 2**:
- [ ] `pio test -e dev` (tous les tests)
- [ ] `pio run -e prod` (test optimisé)
- [ ] Test sur hardware: validation configuration loading
- [ ] Réduction 50%+ complexité ConfigurationSubsystem
- [ ] Tests unitaires complets (90%+ couverture)
- [ ] Aucune régression fonctionnelle
- [ ] Performance maintenue ou améliorée
- [ ] Code plus lisible et maintenable

**Point de Rollback**: `git tag v2.4-config-refactored-complete`

---

## PHASE 3: Refactoring UI System

### **Étape 3.1: Extraction DisplayManager**
- [ ] **3.1.1**: Créer DisplayManager avec logique de timing refresh
- [ ] **3.1.2**: Extraire refresh logic de UISubsystem::update()
- [ ] **3.1.3**: Créer tests timing et performance
- [ ] **3.1.4**: Intégrer DisplayManager dans UISubsystem

**Fichiers créés**:
```
src/core/ui/
├── DisplayManager.hpp
└── DisplayManager.cpp
```

**Validation Étape 3.1**:
- [ ] `pio test -e dev -f "*display_manager*"` passe
- [ ] `pio test -e dev -f "*ui_system*"` passe
- [ ] `pio run -e dev` compile
- [ ] Test hardware: vérifier pas de scintillement écran

**Point de Rollback**: `git tag v3.1-display-manager-extracted`

### **Étape 3.2: Création EventManager (Consolidation EventBus + EventBatcher)**
- [ ] **3.2.1**: Créer EventManager avec API EventBus + logique batching
- [ ] **3.2.2**: Implémenter EventRouter pour dispatch typé
- [ ] **3.2.3**: Créer tests EventManager complets
- [ ] **3.2.4**: Adapter UISubsystem pour utiliser EventManager

**Fichiers créés**:
```
src/core/events/
├── EventManager.hpp
├── EventManager.cpp
└── EventRouter.hpp
```

**Validation Étape 3.2**:
- [ ] `pio test -e dev -f "*event_manager*"` passe
- [ ] `pio test -e dev -f "*ui_system*"` passe
- [ ] Test hardware: vérifier events toujours fonctionnels

**Point de Rollback**: `git tag v3.2-event-manager-created`

### **Étape 3.3: Extraction ViewFactory**
- [ ] **3.3.1**: Créer ViewFactory avec createParameterView, createMenuView, etc.
- [ ] **3.3.2**: Extraire logique création vues de UISubsystem
- [ ] **3.3.3**: Tests factory avec mocks
- [ ] **3.3.4**: Intégrer factory dans UISubsystem

**Fichiers créés**:
```
src/core/ui/factories/
├── ViewFactory.hpp
└── ViewFactory.cpp
```

**Validation Étape 3.3**:
- [ ] `pio test -e dev -f "*view_factory*"` passe
- [ ] `pio test -e dev -f "*ui_system*"` passe
- [ ] `pio run -e dev` compile

**Point de Rollback**: `git tag v3.3-view-factory-extracted`

### **Étape 3.4: Création UISystemCore**
- [ ] **3.4.1**: Créer UISystemCore avec orchestration pure
- [ ] **3.4.2**: Migrer logique coordination de UISubsystem
- [ ] **3.4.3**: Tests orchestration
- [ ] **3.4.4**: Faire UISubsystem wrapper de UISystemCore

**Fichiers créés**:
```
src/core/ui/
├── UISystemCore.hpp
└── UISystemCore.cpp
```

**Validation Complète Phase 3**:
- [ ] `pio test -e dev` (tous tests UI)
- [ ] `pio run -e prod` (performance)
- [ ] Test hardware: UI complètement fonctionnelle
- [ ] UISubsystem réduit à wrapper simple
- [ ] EventManager unifié fonctionnel
- [ ] Toutes fonctionnalités UI préservées
- [ ] Performance égale ou meilleure
- [ ] Tests complets passent

**Point de Rollback**: `git tag v3.4-ui-refactored-complete`

---

## PHASE 4: Refactoring Input System

### **Étape 4.1: Extraction InputConfigExtractor**
- [ ] **4.1.1**: Créer InputConfigExtractor avec extractEncoderConfigs, extractButtonConfigs
- [ ] **4.1.2**: Extraire logique extraction de InputSubsystem
- [ ] **4.1.3**: Tests extraction avec configurations complexes
- [ ] **4.1.4**: Intégrer extractor dans InputSubsystem

**Fichiers créés**:
```
src/core/input/
├── InputConfigExtractor.hpp
└── InputConfigExtractor.cpp
```

**Validation Étape 4.1**:
- [ ] `pio test -e dev -f "*input_config*"` passe
- [ ] `pio test -e dev -f "*input_system*"` passe
- [ ] Test hardware: inputs détectés correctement

**Point de Rollback**: `git tag v4.1-input-config-extracted`

### **Étape 4.2: Extraction InputManagerFactory**
- [ ] **4.2.1**: Créer factory pour EncoderManager, ButtonManager
- [ ] **4.2.2**: Extraire createManagers de InputSubsystem
- [ ] **4.2.3**: Tests factory avec configs variées
- [ ] **4.2.4**: Intégrer factory dans InputSubsystem

**Fichiers créés**:
```
src/core/input/
├── InputManagerFactory.hpp
└── InputManagerFactory.cpp
```

**Validation Étape 4.2**:
- [ ] `pio test -e dev -f "*input_manager*"` passe
- [ ] Test hardware: encodeurs et boutons fonctionnels

**Point de Rollback**: `git tag v4.2-input-factory-extracted`

### **Étape 4.3: Extraction InputProcessorCoordinator**
- [ ] **4.3.1**: Créer coordinator pour ProcessEncoders, ProcessButtons
- [ ] **4.3.2**: Extraire initializeProcessors de InputSubsystem
- [ ] **4.3.3**: Tests coordination
- [ ] **4.3.4**: Intégrer coordinator dans InputSubsystem

**Fichiers créés**:
```
src/core/input/
├── InputProcessorCoordinator.hpp
└── InputProcessorCoordinator.cpp
```

**Validation Étape 4.3**:
- [ ] `pio test -e dev -f "*input_processor*"` passe
- [ ] Test hardware: événements input traités

**Point de Rollback**: `git tag v4.3-input-coordinator-extracted`

### **Étape 4.4: Création InputSystemCore**
- [ ] **4.4.1**: Créer InputSystemCore orchestration pure
- [ ] **4.4.2**: Migrer logique restante InputSubsystem
- [ ] **4.4.3**: Faire InputSubsystem wrapper
- [ ] **4.4.4**: Tests complets Input system

**Fichiers créés**:
```
src/core/input/
├── InputSystemCore.hpp
└── InputSystemCore.cpp
```

**Validation Complète Phase 4**:
- [ ] `pio test -e dev` (tous tests input)
- [ ] Test hardware: tous inputs fonctionnels

**Point de Rollback**: `git tag v4.4-input-refactored-complete`

---

## PHASE 5: Refactoring LvglParameterView

### **Étape 5.1: Extraction ConfigurationMapper**
- [ ] **5.1.1**: Extraire extractMidiControlsFromConfig de LvglParameterView
- [ ] **5.1.2**: Créer mapper avec API claire
- [ ] **5.1.3**: Tests mapping avec configs variées
- [ ] **5.1.4**: Intégrer mapper dans LvglParameterView

**Fichiers créés**:
```
src/adapters/primary/ui/parameter/
├── ConfigurationMapper.hpp
└── ConfigurationMapper.cpp
```

**Validation Étape 5.1**:
- [ ] `pio test -e dev -f "*configuration_mapper*"` passe
- [ ] Test hardware: paramètres mappés correctement

**Point de Rollback**: `git tag v5.1-config-mapper-extracted`

### **Étape 5.2: Extraction WidgetLayoutManager**
- [ ] **5.2.1**: Extraire createParameterWidgets, createGridContainer
- [ ] **5.2.2**: Créer manager layout LVGL
- [ ] **5.2.3**: Tests layout et positionnement
- [ ] **5.2.4**: Intégrer dans LvglParameterView

**Fichiers créés**:
```
src/adapters/primary/ui/parameter/
├── WidgetLayoutManager.hpp
└── WidgetLayoutManager.cpp
```

**Validation Étape 5.2**:
- [ ] `pio test -e dev -f "*widget_layout*"` passe
- [ ] Test hardware: widgets positionnés correctement

**Point de Rollback**: `git tag v5.2-widget-layout-extracted`

### **Étape 5.3: Extraction ParameterEventHandler**
- [ ] **5.3.1**: Extraire handleUIParameterUpdateEvent, handleButtonEvent
- [ ] **5.3.2**: Créer handler événements spécialisé
- [ ] **5.3.3**: Tests gestion événements
- [ ] **5.3.4**: Intégrer handler dans LvglParameterView

**Fichiers créés**:
```
src/adapters/primary/ui/parameter/
├── ParameterEventHandler.hpp
└── ParameterEventHandler.cpp
```

**Validation Étape 5.3**:
- [ ] `pio test -e dev -f "*parameter_event*"` passe
- [ ] Test hardware: événements traités correctement

**Point de Rollback**: `git tag v5.3-parameter-event-extracted`

### **Étape 5.4: Extraction ButtonIndicatorManager**
- [ ] **5.4.1**: Extraire setupButtonIndicators, finalizeButtonIndicatorPositions
- [ ] **5.4.2**: Créer manager indicateurs
- [ ] **5.4.3**: Tests indicateurs boutons
- [ ] **5.4.4**: Intégrer manager dans LvglParameterView

**Fichiers créés**:
```
src/adapters/primary/ui/parameter/
├── ButtonIndicatorManager.hpp
└── ButtonIndicatorManager.cpp
```

**Validation Étape 5.4**:
- [ ] `pio test -e dev -f "*button_indicator*"` passe
- [ ] Test hardware: indicateurs LEDs fonctionnels

**Point de Rollback**: `git tag v5.4-button-indicator-extracted`

### **Étape 5.5: Création ParameterViewController**
- [ ] **5.5.1**: Créer controller orchestration composants
- [ ] **5.5.2**: Migrer logique coordination LvglParameterView
- [ ] **5.5.3**: Tests controller
- [ ] **5.5.4**: Faire LvglParameterView wrapper

**Fichiers créés**:
```
src/adapters/primary/ui/parameter/
├── ParameterViewController.hpp
└── ParameterViewController.cpp
```

**Validation Étape 5.5**:
- [ ] `pio test -e dev -f "*parameter_view*"` passe
- [ ] Test hardware: vue paramètres complète

**Point de Rollback**: `git tag v5.5-parameter-view-refactored`

### **Étape 5.6: Finalisation ParameterView**
- [ ] **5.6.1**: Suppression code redondant LvglParameterView (623 → ~100 lignes)
- [ ] **5.6.2**: Suppression méthodes privées devenues obsolètes
- [ ] **5.6.3**: Suppression logique dupliquée
- [ ] **5.6.4**: Tests complets vue paramètres

**Validation Complète Phase 5**:
- [ ] `pio test -e dev` (tous tests)
- [ ] `pio run -e prod` (performance)
- [ ] Test hardware: UI paramètres complète et fluide

**Point de Rollback**: `git tag v5.6-parameter-view-complete`

---

## PHASE 6: Consolidation Finale et Nettoyage

### **Étape 6.1: Suppression EventBatcher Legacy**
- [ ] **6.1.1**: Remplacer toutes références EventBatcher par EventManager
- [ ] **6.1.2**: Supprimer includes EventBatcher
- [ ] **6.1.3**: Tests vérification aucune référence
- [ ] **6.1.4**: Suppression fichiers

**Fichiers supprimés**:
- `src/core/domain/events/EventBatcher.hpp`
- `src/core/domain/events/EventBatcher.cpp`

**Validation Étape 6.1**:
- [ ] `pio run -e dev` compile sans EventBatcher
- [ ] `grep -r "EventBatcher" src/` doit être vide

**Point de Rollback**: `git tag v6.1-eventbatcher-removed`

### **Étape 6.2: Nettoyage Code Legacy**
- [ ] **6.2.1**: Audit includes redondants
- [ ] **6.2.2**: Suppression méthodes deprecated
- [ ] **6.2.3**: Nettoyage custom deleters
- [ ] **6.2.4**: Suppression code DEBUG mort

**Validation Étape 6.2**:
- [ ] `pio run -e prod` compilation optimisée
- [ ] Vérification réduction taille binaire

**Point de Rollback**: `git tag v6.2-legacy-cleaned`

### **Étape 6.3: Tests d'Intégration Complets**
- [ ] **6.3.1**: Tests finaux tous environnements
- [ ] **6.3.2**: Tests hardware complets
- [ ] **6.3.3**: Benchmarks performance
- [ ] **6.3.4**: Validation métriques

**Tests Finaux**:
- [ ] `pio test -e dev` (tous tests unitaires)
- [ ] `pio test -e prod` (tests optimisés)
- [ ] `pio run -e debug` (test verbose)

**Tests Hardware Complets**:
- [ ] Startup → Shutdown cycle
- [ ] All inputs (encoders, buttons)
- [ ] MIDI output functionality
- [ ] UI responsiveness
- [ ] Performance benchmarks

**Validation Finale**:
- [ ] 0 tests échoués
- [ ] Performance ≥ baseline
- [ ] Memory usage ≤ baseline
- [ ] All hardware functions work
- [ ] Code coverage ≥ 90%

**Point de Rollback**: `git tag v6.3-refactoring-complete`

---

## Métriques de Validation Continue

### **Métriques Cibles Finales**
- [ ] **Réduction lignes code**: -40% (complexité réduite)
- [ ] **Couverture tests**: 90%+ (vs 0% initial)
- [ ] **Violations SOLID**: 0 (vs 5+ actuelles)
- [ ] **Compilation warnings**: 0
- [ ] **Memory usage**: -15% (optimisations)
- [ ] **Performance**: Maintenue ou améliorée

### **Commandes Validation à Chaque Étape**
```bash
# Compilation
pio run -e dev -v 2>&1 | grep -E "(error|warning)"

# Tests
pio test -e dev --verbose

# Métriques code
cloc src/ --exclude-dir=test
```

### **Procédure Rollback**
```bash
# En cas de problème à une étape
git reset --hard [TAG_PRECEDENT]
git clean -fd
pio run -e dev  # Validation état précédent
```

---

## Status Global

**Phase Actuelle**: PHASE 1 - Infrastructure de Tests
**Progression**: 0/6 phases complétées
**Dernière Validation**: En attente
**Prochaine Étape**: 1.1.1 - Créer structure test/

---

## Notes et Observations

### **Changements par rapport au plan initial**
- (Ajouter ici les adaptations nécessaires pendant l'implémentation)

### **Problèmes rencontrés**
- (Documenter les obstacles et leurs solutions)

### **Optimisations découvertes**
- (Noter les améliorations trouvées en cours de route)