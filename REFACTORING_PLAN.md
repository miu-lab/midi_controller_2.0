# PLAN DE REFACTORISATION - ARCHITECTURE SOLID PRINCIPLES

## Vue d'ensemble

Ce plan de refactorisation vise à appliquer les principes SOLID et Clean Architecture au contrôleur MIDI, en particulier pour résoudre les violations du Single Responsibility Principle (SRP) identifiées dans l'analyse architecturale.

## Phases de refactorisation

### ✅ PHASE 1: INFRASTRUCTURE DE TESTS ET TESTS DE BASE

#### ✅ Phase 1.1: Infrastructure de tests Unity
- ✅ **1.1.1**: Créer structure test/ avec test_main.cpp minimal
- ✅ **1.1.2**: Ajouter configuration Unity pour Teensy  
- ✅ **1.1.3**: Créer MockMidiOut pour isoler tests MIDI
- ✅ **1.1.4**: Créer MockEventBus pour tests événements
- ✅ **1.1.5**: Créer MockConfiguration avec données test
- ✅ **Validation**: `pio test -e dev` doit compiler (sans device)

#### 🔄 Phase 1.2: Tests de base système existant
- ✅ **1.2.1**: Test ConfigurationSubsystem::init() avec mock container
- 🔄 **1.2.2**: Test InputSubsystem::init() basique
- ⏳ **1.2.3**: Test UISubsystem::init() basique  
- ⏳ **1.2.4**: Test MidiSubsystem::init() basique
- ⏳ **Validation**: Tous les tests de base passent

**Tag de rollback**: `refactor-phase1-complete`

---

### ⏳ PHASE 2: REFACTORING CONFIGURATION (SRP VIOLATION #1)

#### ⏳ Phase 2.1: Extraction ConfigurationLoader
- ⏳ **2.1.1**: Créer `ConfigurationLoader` pour charger les configurations
- ⏳ **2.1.2**: Extraire `loadUnifiedConfigurations()` de ConfigurationSubsystem
- ⏳ **2.1.3**: Injecter ConfigurationLoader dans ConfigurationSubsystem
- ⏳ **Validation**: Tests ConfigurationSubsystem continuent de passer

#### ⏳ Phase 2.2: Extraction ConfigurationService
- ⏳ **2.2.1**: Créer `ConfigurationService` pour les opérations métier
- ⏳ **2.2.2**: Déplacer méthodes query (getAllControls, getByType, etc.)
- ⏳ **2.2.3**: ConfigurationSubsystem devient orchestrateur simple
- ⏳ **Validation**: Interface IConfiguration reste identique

#### ⏳ Phase 2.3: Extraction ConfigurationRegistry
- ⏳ **2.3.1**: Créer `ConfigurationRegistry` pour gestion DI
- ⏳ **2.3.2**: Déplacer logique d'enregistrement des dépendances
- ⏳ **2.3.3**: ConfigurationSubsystem utilise ConfigurationRegistry
- ⏳ **Validation**: Injection de dépendances fonctionne

**Tag de rollback**: `refactor-phase2-complete`

---

### ⏳ PHASE 3: REFACTORING UI SYSTEM (SRP VIOLATION #2)

#### ⏳ Phase 3.1: Extraction DisplayManager
- ⏳ **3.1.1**: Créer `DisplayManager` pour gestion affichage
- ⏳ **3.1.2**: Déplacer création ILI9341Bridge et LVGL init
- ⏳ **3.1.3**: UISubsystem délègue à DisplayManager
- ⏳ **Validation**: Affichage continue de fonctionner

#### ⏳ Phase 3.2: Création EventManager
- ⏳ **3.2.1**: Créer `EventManager` pour EventBatcher et listeners
- ⏳ **3.2.2**: Centraliser création et configuration événements
- ⏳ **3.2.3**: UISubsystem utilise EventManager
- ⏳ **Validation**: Événements UI fonctionnent

#### ⏳ Phase 3.3: Extraction ViewFactory
- ⏳ **3.3.1**: Créer `ViewFactory` pour création des vues
- ⏳ **3.3.2**: Déplacer logique de création ViewManager
- ⏳ **3.3.3**: UISubsystem utilise ViewFactory
- ⏳ **Validation**: Navigation UI fonctionne

**Tag de rollback**: `refactor-phase3-complete`

---

### ⏳ PHASE 4: REFACTORING INPUT SYSTEM (SRP VIOLATION #3)

#### ⏳ Phase 4.1: Extraction InputHardwareManager
- ⏳ **4.1.1**: Créer `InputHardwareManager` pour gestion hardware
- ⏳ **4.1.2**: Déplacer EncoderManager et DigitalButtonManager
- ⏳ **4.1.3**: InputSubsystem délègue à InputHardwareManager
- ⏳ **Validation**: Hardware input continue de fonctionner

#### ⏳ Phase 4.2: Extraction InputEventProcessor
- ⏳ **4.2.1**: Créer `InputEventProcessor` pour traitement événements
- ⏳ **4.2.2**: Déplacer logique ProcessButtons et ProcessEncoders
- ⏳ **4.2.3**: InputSubsystem utilise InputEventProcessor
- ⏳ **Validation**: Événements input traités correctement

#### ⏳ Phase 4.3: Simplification InputSubsystem
- ⏳ **4.3.1**: InputSubsystem devient orchestrateur simple
- ⏳ **4.3.2**: Réduire complexité cyclomatique
- ⏳ **4.3.3**: Tests de régression sur input complet
- ⏳ **Validation**: Input système entièrement fonctionnel

**Tag de rollback**: `refactor-phase4-complete`

---

### ⏳ PHASE 5: REFACTORING LVGLPARAMETERVIEW (GOD CLASS)

#### ⏳ Phase 5.1: Analyse et découpage
- ⏳ **5.1.1**: Identifier 6 responsabilités distinctes de LvglParameterView
- ⏳ **5.1.2**: Créer interfaces pour chaque responsabilité
- ⏳ **5.1.3**: Plan de migration progressive

#### ⏳ Phase 5.2: Extraction composants UI
- ⏳ **5.2.1**: Créer `ParameterDisplayRenderer` (rendering)
- ⏳ **5.2.2**: Créer `ParameterInputHandler` (input)
- ⏳ **5.2.3**: Créer `ParameterLayoutManager` (layout)
- ⏳ **Validation**: UI parameters continue de s'afficher

#### ⏳ Phase 5.3: Extraction logique métier
- ⏳ **5.3.1**: Créer `ParameterDataManager` (data)
- ⏳ **5.3.2**: Créer `ParameterEventDispatcher` (events)
- ⏳ **5.3.3**: Créer `ParameterValidator` (validation)
- ⏳ **Validation**: Logique parameters fonctionne

#### ⏳ Phase 5.4: Integration et simplification
- ⏳ **5.4.1**: LvglParameterView devient coordinateur simple
- ⏳ **5.4.2**: Injection des 6 composants via DI
- ⏳ **5.4.3**: Tests complets parameter view
- ⏳ **Validation**: Fonctionnalité complète préservée

**Tag de rollback**: `refactor-phase5-complete`

---

### ⏳ PHASE 6: CONSOLIDATION FINALE ET NETTOYAGE

#### ⏳ Phase 6.1: Nettoyage EventBatcher legacy
- ⏳ **6.1.1**: Analyser utilisation EventBatcher vs EventBus
- ⏳ **6.1.2**: Migrer vers EventBus unifié si possible
- ⏳ **6.1.3**: Supprimer code mort EventBatcher
- ⏳ **Validation**: Système événements cohérent

#### ⏳ Phase 6.2: Optimisation DI Container
- ⏳ **6.2.1**: Audit des dépendances enregistrées
- ⏳ **6.2.2**: Optimiser ordre d'initialisation
- ⏳ **6.2.3**: Documentation architecture finale
- ⏳ **Validation**: Performance maintenue

#### ⏳ Phase 6.3: Tests d'intégration complets
- ⏳ **6.3.1**: Tests end-to-end workflow complet
- ⏳ **6.3.2**: Tests performance comparatifs
- ⏳ **6.3.3**: Documentation refactoring
- ⏳ **Validation**: Système entièrement validé

**Tag de rollback**: `refactor-complete`

---

## Critères de validation

### À chaque phase:
1. ✅ Compilation sans erreur
2. ✅ Tests unitaires passent
3. ✅ Performance maintenue (pas de régression > 5%)
4. ✅ Fonctionnalité préservée (tests manuels si nécessaire)

### Rollback automatique si:
- ❌ Compilation échoue
- ❌ Tests critiques échouent  
- ❌ Performance régresse > 10%
- ❌ Fonctionnalité critique cassée

## Progress Tracking

**Phase actuelle**: 1.2.2 - Test InputSubsystem::init() basique

**Dernière validation réussie**: Phase 1.2.1 (✅ 2024-07-03)

**Prochaine étape**: Créer tests pour InputSubsystem initialization