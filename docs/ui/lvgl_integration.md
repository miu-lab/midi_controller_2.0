# Plan d'Intégration LVGL Pure - Contrôleur MIDI 2.0

## Vue d'ensemble

Ce document détaille le plan de migration vers une architecture LVGL pure, éliminant la couche d'abstraction DisplayPort pour une approche plus maintenable et performante.

## Objectifs Stratégiques

### Performance
- Éliminer le overhead de mapping DisplayPort→LVGL
- Utiliser pleinement les optimisations DMA d'ILI9341_T4
- Rendu différentiel natif LVGL

### Maintenabilité
- Code UI direct et lisible
- Séparation claire hardware/UI
- Widgets réutilisables et modulaires

### Évolutivité
- Ajout de fonctionnalités UI sans breaking changes
- Support natif animations et effets LVGL
- Architecture extensible pour futurs displays

## Architecture Cible

```
┌─────────────────────────────────────────────────┐
│                Application                      │
├─────────────────────────────────────────────────┤
│  UI Layer (LVGL Pure)                          │
│  ┌─────────────────┐ ┌─────────────────────┐    │
│  │ LvglViewManager │ │ Specialized Widgets │    │
│  │                 │ │ - ParameterWidget   │    │
│  │ - State Machine │ │ - MenuWidget        │    │
│  │ - Screen Trans. │ │ - ModalWidget       │    │
│  │ - Event Routing │ │ - MidiKnobWidget    │    │
│  └─────────────────┘ └─────────────────────┘    │
├─────────────────────────────────────────────────┤
│  Hardware Layer                                │
│  ┌─────────────────────────────────────────────┐│
│  │ Ili9341LvglDisplay (Hardware Only)         ││
│  │ - Buffer Management (DMAMEM)               ││
│  │ - SPI/DMA Configuration                    ││
│  │ - LVGL Driver Integration                  ││
│  │ - Performance Monitoring                   ││
│  └─────────────────────────────────────────────┘│
├─────────────────────────────────────────────────┤
│  Hardware Drivers                              │
│  │ ILI9341_T4 | LVGL Core | Teensy 4.1 HAL   │  │
└─────────────────────────────────────────────────┘
```

## Analyse de l'Architecture Actuelle

### Fichiers qui deviendront LEGACY (à supprimer à terme)

**Architecture UI basée sur DisplayPort :**
- `src/adapters/primary/ui/View.hpp` - Interface abstraite obsolète
- `src/adapters/primary/ui/ViewManager.hpp` - Interface gestionnaire traditionnel
- `src/adapters/primary/ui/DefaultViewManager.hpp/.cpp` - Implémentation DisplayPort

**Vues utilisant DisplayPort :**
- `src/adapters/primary/ui/MenuView.hpp/.cpp` - Vue menu primitive
- `src/adapters/primary/ui/ParameterFocusView.hpp/.cpp` - Vue paramètre custom
- `src/adapters/primary/ui/SplashScreenView.hpp/.cpp` - Vue démarrage simple
- `src/adapters/primary/ui/ModalView.hpp/.cpp` - Boîtes de dialogue

**Événements UI legacy :**
- `src/adapters/primary/ui/ViewManagerEventListener.hpp/.cpp` - Système événements ancien
- `src/adapters/primary/ui/DisplayEventListener.hpp` - Écouteur DisplayPort

### Fichiers à MODIFIER (garder mais simplifier)

**Hardware display :**
- `src/adapters/secondary/hardware/output/display/Ili9341LvglDisplay.hpp/.cpp` - Simplifier interface
- `src/core/ports/output/DisplayPort.hpp` - Remplacer par LvglPort

**Core domain :**
- `src/core/controllers/UIController.hpp/.cpp` - Adapter pour widgets LVGL
- `src/app/subsystems/UISubsystem.hpp/.cpp` - Nouveau ViewManager LVGL
- `src/core/domain/interfaces/IUISystem.hpp` - Ajouter méthodes LVGL
- `src/app/InitializationScript.hpp/.cpp` - Adapter DI
- `src/core/tasks/DisplayUpdateTask.hpp` - Adapter pour lv_timer_handler

### Fichiers à CRÉER

**Nouveau système LVGL :**
- `src/adapters/ui/lvgl/LvglViewManager.hpp/.cpp` - Gestionnaire screens
- `src/adapters/ui/lvgl/widgets/ParameterWidget.hpp/.cpp` - Widget paramètre
- `src/adapters/ui/lvgl/widgets/MenuWidget.hpp/.cpp` - Widget menu
- `src/adapters/ui/lvgl/widgets/SplashWidget.hpp/.cpp` - Widget démarrage
- `src/adapters/ui/lvgl/screens/MainScreen.hpp/.cpp` - Écran principal
- `src/adapters/ui/lvgl/screens/MenuScreen.hpp/.cpp` - Écran menu

**Framework et styles :**
- `src/core/ui/framework/LvglPort.hpp/.cpp` - Interface abstraction
- `src/core/ui/framework/LvglEventHandler.hpp/.cpp` - Pont événements
- `src/adapters/ui/lvgl/styles/MidiTheme.hpp/.cpp` - Thème interface
- `src/config/ui/LvglConfig.hpp` - Configuration projet

**Tests :**
- `test/ui/test_lvgl_widgets.cpp` - Tests widgets
- `test/ui/test_lvgl_view_manager.cpp` - Tests gestionnaire
- `test/ui/test_parameter_widget.cpp` - Tests widget paramètre

## Phase 1 : Refactoring du Hardware Layer (Semaine 1) - DÉTAILLÉ

### Étape 1.1 : Simplifier Ili9341LvglDisplay (Jour 1-2)

**Objectif** : Transformer en driver hardware pur en supprimant le mapping DisplayPort

#### Actions détaillées :

**1.1.1 - Analyser l'état actuel (30 min)**
```bash
# Valider que LVGL fonctionne déjà
pio run -e MIDI_CONTROLLER_DEVELOPMENT
pio run -t upload
# Test écran : doit afficher quelque chose
```

**1.1.2 - Créer version simplifiée (2h)**
- [ ] **Dupliquer fichiers** pour backup : `Ili9341LvglDisplay_legacy.hpp/.cpp`
- [ ] **Supprimer méthodes DisplayPort** : drawText, drawLine, drawRect, drawCircle, etc.
- [ ] **Garder uniquement interface LVGL** :
  ```cpp
  class Ili9341LvglDisplay {
  public:
      bool init();
      lv_display_t* getLvglDisplay() const;
      void getPerformanceStats(...) const;
      void setRotation(uint8_t rotation);
      void getDimensions(uint16_t& width, uint16_t& height);
      
      // Test helper temporaire
      lv_obj_t* createTestScreen();
      
  private:
      static void flush_cb(...);
      // Garde callbacks et buffers
  };
  ```

**1.1.3 - Test de validation (30 min)**
```bash
pio run -e MIDI_CONTROLLER_DEVELOPMENT
# Doit compiler sans erreur DisplayPort
```

**Critères de succès Étape 1.1** :
- ✅ Compilation réussie sans méthodes DisplayPort
- ✅ `getLvglDisplay()` retourne display valide
- ✅ Aucune régression sur init hardware

---

### Étape 1.2 : Optimiser Buffers et Performance (Jour 2-3)

**Objectif** : Valider et optimiser l'allocation mémoire et transferts DMA

#### Actions détaillées :

**1.2.1 - Diagnostic mémoire actuel (1h)**
- [ ] **Ajouter debug allocation** :
  ```cpp
  void Ili9341LvglDisplay::debugMemory() {
      Serial.print("Framebuffer: 0x"); Serial.println((uint32_t)framebuffer_, HEX);
      Serial.print("LVGL buf1: 0x"); Serial.println((uint32_t)lvgl_buf1_, HEX);
      Serial.print("Free DMAMEM: "); Serial.println(available_dmamem());
  }
  ```
- [ ] **Mesurer utilisation RAM** actuelle

**1.2.2 - Optimiser flush_cb (2h)**
- [ ] **Analyser performance actuelle** :
  ```cpp
  static void flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
      unsigned long start = micros();
      
      // Transfer optimisé
      // ... existing code ...
      
      unsigned long duration = micros() - start;
      if (duration > 1000) { // > 1ms = problème
          Serial.print("SLOW flush: "); Serial.println(duration);
      }
  }
  ```
- [ ] **Optimiser copie pixels** si nécessaire
- [ ] **Valider que DMA est utilisé** (pas de transferts bloquants)

**1.2.3 - Implémenter dirty region tracking (2h)**
- [ ] **Ajouter métriques zones sales** :
  ```cpp
  class FlushProfiler {
      uint32_t total_pixels_updated_;
      uint32_t flush_count_;
  public:
      void recordFlush(const lv_area_t* area);
      float getAveragePixelsPerFlush();
  };
  ```
- [ ] **Optimiser pour zones partielles** vs écran complet

**1.2.4 - Test benchmark (1h)**
```bash
# Créer test de stress
lv_obj_t* stress_test = lv_obj_create(lv_screen_active());
// Animer plusieurs éléments simultanément
# Mesurer FPS réels
```

**Critères de succès Étape 1.2** :
- ✅ flush_cb < 500μs pour zone partielle (< 40 lignes)
- ✅ flush_cb < 2ms pour écran complet
- ✅ Allocation DMAMEM confirmée (addresses 0x2020xxxx)
- ✅ Pas de malloc() dans flush_cb

---

### Étape 1.3 : Tests Hardware Complets (Jour 3-4)

**Objectif** : Valider stabilité et performance sous charge

#### Actions détaillées :

**1.3.1 - Test initialisation robuste (1h)**
- [ ] **Test init multiple** :
  ```cpp
  void test_multiple_init() {
      display.init(); // Premier
      delay(100);
      display.init(); // Second - ne doit pas planter
      // Vérifier écran toujours fonctionnel
  }
  ```
- [ ] **Test rotation** :
  ```cpp
  for (int rot = 0; rot < 4; rot++) {
      display.setRotation(rot);
      // Afficher pattern test
      delay(1000);
  }
  ```

**1.3.2 - Benchmark transfert DMA (2h)**
- [ ] **Test patterns variés** :
  ```cpp
  void benchmark_patterns() {
      // Pattern 1: Écran uni
      lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
      measure_fps(5000); // 5 sec
      
      // Pattern 2: Dégradé
      // Pattern 3: Animation
      // Pattern 4: Texte scrolling
  }
  ```
- [ ] **Mesurer FPS réels** avec différents contenus
- [ ] **Test mémoire sous stress** (allocations/libérations répétées)

**1.3.3 - Test stabilité longue durée (2h)**
- [ ] **Test endurance** :
  ```cpp
  void endurance_test() {
      for (int cycle = 0; cycle < 1000; cycle++) {
          // Créer/détruire objets LVGL
          // Changer contenus
          // Mesurer mémoire
          if (cycle % 100 == 0) {
              Serial.print("Cycle "); Serial.println(cycle);
              debugMemory();
          }
      }
  }
  ```
- [ ] **Surveiller fuites mémoire**
- [ ] **Test thermal** (performance à chaud)

**1.3.4 - Validation mémoire finale (1h)**
- [ ] **Vérifier allocations DMAMEM** correctes
- [ ] **Confirmer pas de malloc/free** dans callbacks
- [ ] **Mesurer fragmentation** heap

**Critères de succès Étape 1.3** :
- ✅ 60 FPS stable sur animation continue (10 min)
- ✅ Aucune fuite mémoire détectée (endurance test)
- ✅ Init robuste (10 cycles successifs OK)
- ✅ Toutes rotations fonctionnelles

---

### Étape 1.4 : Intégration Test Hardware Complet (Jour 4-5)

**Objectif** : Créer programme de test complet pour validation finale

#### Actions détaillées :

**1.4.1 - Test suite complète (3h)**
- [ ] **Créer programme test dédié** :
  ```cpp
  // src/test_hardware/main_hardware_test.cpp
  void setup() {
      // Init display
      // Run all tests
      // Report results
  }
  
  void test_suite() {
      test_init_multiple();
      test_performance_benchmark();
      test_memory_stability();
      test_rotation_all();
      // Results sur Serial
  }
  ```

**1.4.2 - Documentation résultats (1h)**
- [ ] **Logger métriques** dans fichier test
- [ ] **Documenter performance** obtenue vs cible
- [ ] **Identifier optimisations** restantes si nécessaire

**1.4.3 - Validation équipe (1h)**
- [ ] **Demo hardware** fonctionnel
- [ ] **Review code** changements Phase 1
- [ ] **Validation critères** succès atteints

**Livrables Phase 1** :
- ✅ Ili9341LvglDisplay simplifié et optimisé
- ✅ Tests hardware automatisés
- ✅ Métriques performance documentées
- ✅ Code ready pour Phase 2 (widgets)

**Critères de succès globaux Phase 1** :
- ✅ 60 FPS stable mesurés
- ✅ flush_cb < 500μs (zones partielles)
- ✅ Allocation DMAMEM validée
- ✅ Zero fuite mémoire
- ✅ Init robuste confirmé
- ✅ Tests automatisés créés

## Phase 2 : Widgets LVGL Spécialisés (Semaine 2)

### 2.1 Widget Core : ParameterWidget

**Objectif** : Widget principal pour affichage paramètres MIDI

**Design** :
```cpp
class ParameterWidget {
private:
    lv_obj_t* container_;
    lv_obj_t* arc_;           // Encodeur visuel (0-127)
    lv_obj_t* value_label_;   // Valeur numérique
    lv_obj_t* name_label_;    // Nom paramètre
    lv_obj_t* cc_label_;      // Numéro CC
    
    uint8_t current_value_;
    uint8_t cc_number_;
    String parameter_name_;
    
public:
    ParameterWidget(lv_obj_t* parent);
    
    void updateValue(uint8_t value, bool animate = true);
    void setParameter(uint8_t cc, const String& name);
    void setVisible(bool visible);
    void setTheme(bool dark_mode);
};
```

**Fonctionnalités** :
- [ ] **Arc circulaire** pour valeur 0-127
- [ ] **Animation fluide** lors changement valeur
- [ ] **Labels informatifs** (CC#, nom, valeur)
- [ ] **Thèmes sombre/clair**
- [ ] **Responsive layout** selon rotation écran

### 2.2 Widget Menu : MenuWidget

**Design** :
```cpp
class MenuWidget {
private:
    lv_obj_t* list_;
    std::vector<lv_obj_t*> menu_items_;
    uint8_t selected_index_;
    uint8_t visible_items_;
    
public:
    struct MenuItem {
        String text;
        std::function<void()> callback;
        bool enabled = true;
    };
    
    MenuWidget(lv_obj_t* parent);
    
    void addItem(const MenuItem& item);
    void navigate(int8_t direction);
    void selectItem();
    void updateItem(uint8_t index, const String& text);
    void setEnabled(uint8_t index, bool enabled);
};
```

**Fonctionnalités** :
- [ ] **Navigation encodeur** fluide
- [ ] **Scrolling automatique** si plus d'items que d'espace
- [ ] **Feedback visuel** pour sélection
- [ ] **Items dynamiques** (enable/disable)

### 2.3 Widget Modal : ModalWidget

**Design** :
```cpp
class ModalWidget {
private:
    lv_obj_t* overlay_;
    lv_obj_t* container_;
    lv_obj_t* title_label_;
    lv_obj_t* message_label_;
    lv_obj_t* button_matrix_;
    
public:
    enum class ButtonType { OK, CANCEL, YES_NO, CUSTOM };
    
    ModalWidget(lv_obj_t* parent);
    
    void show(const String& title, const String& message, 
              ButtonType type = ButtonType::OK);
    void hide();
    void setCallback(std::function<void(int button_id)> cb);
};
```

### 2.4 Tests Widgets

**Actions** :
- [ ] **Test unitaire** chaque widget isolé
- [ ] **Test intégration** widgets ensemble
- [ ] **Test performance** avec animations
- [ ] **Test mémoire** allocation/libération

## Phase 3 : LvglViewManager (Semaine 3)

### 3.1 Gestionnaire d'État UI

**Design** :
```cpp
enum class UIState {
    Splash,
    ParameterFocus,
    Menu,
    Modal
};

class UIStateMachine {
private:
    UIState current_state_;
    UIState previous_state_;
    std::map<UIState, std::set<UIState>> valid_transitions_;
    
public:
    bool canTransition(UIState from, UIState to);
    void transition(UIState new_state);
    UIState getCurrentState() const;
    void handleBackButton();
};
```

### 3.2 LvglViewManager Principal

**Design** :
```cpp
class LvglViewManager : public ViewManager {
private:
    std::shared_ptr<Ili9341LvglDisplay> hardware_;
    UIStateMachine state_machine_;
    
    // Écrans LVGL
    lv_obj_t* splash_screen_;
    lv_obj_t* parameter_screen_;
    lv_obj_t* menu_screen_;
    
    // Widgets spécialisés
    std::unique_ptr<ParameterWidget> parameter_widget_;
    std::unique_ptr<MenuWidget> menu_widget_;
    std::unique_ptr<ModalWidget> modal_widget_;
    
    // État application
    bool needs_update_;
    uint32_t last_activity_;
    
public:
    LvglViewManager(std::shared_ptr<Ili9341LvglDisplay> display);
    
    // Interface ViewManager
    bool init() override;
    void update() override;
    void render() override;
    
    // Spécialisations MIDI
    void showParameterFocus(uint8_t ccNumber, uint8_t channel, 
                           uint8_t value, const String& parameterName) override;
    void updateParameterValue(uint8_t value) override;
    void showMenu() override;
    void showHome() override;
    
private:
    void setupScreens();
    void setupStyles();
    void handleStateTransition(UIState new_state);
};
```

### 3.3 Gestion des Événements

**Actions** :
- [ ] **Input handling** centralisé (boutons, encodeurs)
- [ ] **Event routing** vers widgets appropriés
- [ ] **Timeout handling** (retour auto à ParameterFocus)
- [ ] **Gesture support** (préparation future fonctionnalités tactiles)

## Phase 4 : Intégration et Migration (Semaine 4)

### 4.1 Migration Progressive

**Stratégie** :
1. **Garder l'ancien système** en parallèle
2. **Feature flag** pour basculer entre ancien/nouveau
3. **Tests A/B** pour validation
4. **Migration graduelle** des fonctionnalités

**Implementation** :
```cpp
// Configuration dans GlobalSettings.hpp
#define USE_LVGL_PURE_UI 1

// Dans DependencyContainer
#if USE_LVGL_PURE_UI
    auto ui_manager = std::make_shared<LvglViewManager>(display);
#else
    auto ui_manager = std::make_shared<DefaultViewManager>(display);
#endif
```

### 4.2 Update de l'Injection de Dépendances

**Actions** :
- [ ] **Modifier DependencyContainer** pour nouveaux composants
- [ ] **Update InitializationScript** avec nouvelle architecture
- [ ] **Adapter les Controllers** existants

### 4.3 Tests d'Intégration Complets

**Actions** :
- [ ] **Test scénarios utilisateur** complets
- [ ] **Test performance** sous charge
- [ ] **Test stabilité** longue durée
- [ ] **Test edge cases** (rotation, mémoire faible, etc.)

### 4.4 Nettoyage et Documentation

**Actions** :
- [ ] **Supprimer ancien code** DisplayPort si validation OK
- [ ] **Update documentation** technique
- [ ] **Création guides** développeur pour widgets LVGL
- [ ] **Performance benchmarks** documentés

## Métriques de Réussite

### Performance
- **FPS Display** : ≥ 60 FPS stable
- **Latence Input** : < 16ms input→affichage
- **Mémoire RAM** : < 80% utilisation peak
- **CPU Load** : < 30% average sur UI tasks

### Qualité Code
- **Coverage Tests** : > 90% sur nouveaux composants
- **Code Reviews** : 100% des changements
- **Documentation** : APIs publiques documentées
- **Pas de Warnings** : Compilation clean

### User Experience
- **Fluidité animations** : Pas de saccades visibles
- **Réactivité** : Response immédiate aux interactions
- **Cohérence visuelle** : Design uniforme
- **Robustesse** : Pas de crashes UI

## Risques et Mitigation

### Risque : Performance dégradée
**Mitigation** : Benchmarks continus, profiling LVGL, optimisation buffers

### Risque : Régression fonctionnelle
**Mitigation** : Tests automated extensifs, validation manuelle systématique

### Risque : Complexité accrue
**Mitigation** : Documentation détaillée, design patterns clairs, code reviews

### Risque : Timeline dépassé
**Mitigation** : Approche incrémentale, feature flags, rollback possible

## Timeline Détaillé

```
Semaine 1: Hardware Layer
├── J1-2: Refactor Ili9341LvglDisplay  
├── J3-4: Optimisation buffers/DMA
└── J5: Tests hardware + benchmark

Semaine 2: Widgets LVGL
├── J1-2: ParameterWidget + tests
├── J3: MenuWidget + tests  
├── J4: ModalWidget + tests
└── J5: Tests intégration widgets

Semaine 3: ViewManager
├── J1-2: UIStateMachine + LvglViewManager base
├── J3-4: Event handling + state transitions
└── J5: Tests ViewManager complet

Semaine 4: Intégration
├── J1-2: Migration DI + feature flags
├── J3-4: Tests intégration + stabilité
└── J5: Nettoyage + documentation finale
```

## Prochaines Étapes

1. **Validation du plan** avec l'équipe
2. **Setup environnement** de développement LVGL
3. **Début Phase 1** : Refactoring hardware layer
4. **Reviews hebdomadaires** progression vs plan

---

*Document vivant - mis à jour au fur et à mesure de l'avancement du projet*