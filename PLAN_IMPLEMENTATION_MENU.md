# PLAN D'IMPLÉMENTATION DU SYSTÈME DE MENU MODULAIRE

## Vue d'ensemble
Implémentation d'un système de menu modulaire basé sur l'architecture ParameterView, avec intégration complète des contrôles de navigation définis dans ConfigurationFactory et gestion appropriée des événements.

## Configuration des contrôles de navigation identifiés

### Contrôles de navigation disponibles :
- **Bouton Menu (ID 51)** : `withNavigation("HOME", MappingControlType::BUTTON)`
- **Bouton OK (ID 52)** : `withNavigation("BACK", MappingControlType::BUTTON)`  
- **Encodeur Navigation (ID 79)** : `withNavigation("ITEM_NAVIGATOR", MappingControlType::ENCODER)`
- **Bouton Encodeur Navigation (ID 1079)** : `withNavigation("ITEM_VALIDATE", MappingControlType::BUTTON)` (enfant de 79)

### Comportement souhaité :
- **Bouton Home (51)** : Basculer l'affichage du menu (show/hide)
- **Encodeur Navigation (79)** : Naviguer entre les éléments du menu
- **Bouton Encodeur Navigation (1079)** : Valider la sélection/entrer dans sous-menu
- **Bouton OK (52)** : Annuler/revenir en arrière dans la hiérarchie

## PHASE 1 : Correction du système de navigation existant 🔄

### 1.1 Corriger l'enregistrement des contrôles de navigation ✅
- [x] Modifier `InitializationScript.cpp` pour scanner les définitions de contrôles pour les mappings de navigation
- [x] Enregistrer les IDs de contrôles de navigation (51, 52, 79, 1079) avec NavigationConfigService
- [x] S'assurer que MidiMapper filtre correctement ces contrôles
- [x] Ajouter `getNavigationControlIds()` à MidiMapper pour synchronisation
- [x] Créer `syncNavigationControlsWithConfigService()` dans InitializationScript
- [x] Tests unitaires créés et fonctionnels (test_navigation_controls_sync.cpp)
- [x] Code legacy marqué (LvglMenuView.hpp/.cpp)

### 1.2 Mettre à jour UIControllerEventListener ❌
- [ ] Corriger la gestion des types d'événements pour traiter HighPriorityButtonPress et HighPriorityEncoderChanged
- [ ] Mapper les IDs de contrôles aux actions de navigation appropriées basées sur les définitions de contrôles
- [ ] Supprimer les assumptions hard-codées sur les IDs de boutons

### 1.3 Implémenter le mapping des actions de navigation ❌
- [ ] Créer un système de mapping des chaînes d'action de navigation vers les méthodes UIController
- [ ] Gérer les actions "HOME", "BACK", "ITEM_NAVIGATOR", "ITEM_VALIDATE" correctement
- [ ] S'assurer que la rotation d'encodeur (ITEM_NAVIGATOR) appelle navigateMenu()
- [ ] S'assurer que le bouton d'encodeur (ITEM_VALIDATE) appelle selectMenuItem()

## PHASE 2 : Architecture modulaire du menu ❌

### 2.1 MenuViewController - Orchestrateur principal ❌
```cpp
class MenuViewController : public EventListener {
private:
    std::unique_ptr<MenuConfigurationExtractor> configExtractor_;
    std::unique_ptr<MenuItemMappingManager> mappingManager_;
    std::unique_ptr<MenuSceneManager> sceneManager_;
    std::unique_ptr<MenuEventHandler> eventHandler_;
    
    // État de navigation
    MenuState currentState_;
    std::vector<MenuLevel> menuHierarchy_;
    int currentItemIndex_;
    bool menuVisible_;
};
```

### 2.2 MenuConfigurationExtractor - Extraction de configuration ❌
```cpp
class MenuConfigurationExtractor {
    struct MenuItemInfo {
        uint8_t itemId;
        String label;
        String action;
        uint8_t parentId;
        bool enabled;
        String iconPath;
    };
    
    std::vector<MenuItemInfo> extractMenuItems(const UnifiedConfiguration& config);
    MenuDisplaySettings extractDisplaySettings(const UnifiedConfiguration& config);
};
```

### 2.3 MenuItemMappingManager - Gestion de la hiérarchie ❌
```cpp
class MenuItemMappingManager {
    // Structure hiérarchique du menu
    std::map<uint8_t, std::vector<uint8_t>> menuHierarchy_;
    
    // Gestion de la pile de navigation
    std::stack<MenuLevel> navigationStack_;
    
    int8_t getCurrentItem() const;
    std::vector<uint8_t> getCurrentLevelItems() const;
    bool navigateToItem(uint8_t itemId);
    bool goBack();
    bool goHome();
};
```

### 2.4 MenuSceneManager - Gestion UI LVGL ❌
```cpp
class MenuSceneManager {
    struct MenuSceneConfig {
        uint8_t maxItems;
        uint16_t itemHeight;
        uint16_t scrollAreaHeight;
        uint32_t backgroundColor;
        uint32_t selectionColor;
        uint16_t animationDuration;
        bool enableAnimations;
    };
    
    lv_obj_t* menuScreen_;
    lv_obj_t* menuContainer_;
    lv_obj_t* titleBar_;
    lv_obj_t* itemsList_;
    std::vector<std::unique_ptr<MenuItemWidget>> menuItems_;
    
    bool createMenuScreen();
    bool createMenuContainer();
    bool createTitleBar();
    bool createItemsList();
    void updateMenuSelection(int8_t selectedIndex);
    void animateTransition(MenuTransitionType type);
};
```

## PHASE 3 : Composants UI du menu ❌

### 3.1 MenuItemWidget - Widget d'élément de menu ❌
```cpp
class MenuItemWidget {
    lv_obj_t* container_;
    lv_obj_t* iconContainer_;
    lv_obj_t* icon_;
    lv_obj_t* label_;
    lv_obj_t* arrow_;
    lv_obj_t* selectionIndicator_;
    
    bool isSelected_;
    bool isEnabled_;
    bool hasSubMenu_;
    
    void setSelected(bool selected);
    void setEnabled(bool enabled);
    void setHasSubMenu(bool hasSubMenu);
    void updateLabel(const String& text);
    void updateIcon(const String& iconPath);
    void applyTheme(const UITheme& theme);
};
```

### 3.2 MenuTitleBar - Barre de titre du menu ❌
```cpp
class MenuTitleBar {
    lv_obj_t* container_;
    lv_obj_t* backButton_;
    lv_obj_t* titleLabel_;
    lv_obj_t* breadcrumb_;
    
    void updateTitle(const String& title);
    void updateBreadcrumb(const std::vector<String>& path);
    void showBackButton(bool show);
};
```

## PHASE 4 : Traitement des événements ❌

### 4.1 MenuEventHandler - Gestionnaire d'événements ❌
```cpp
class MenuEventHandler : public EventListener {
    bool handleEncoderEvent(const HighPriorityEncoderChangedEvent& event);
    bool handleButtonEvent(const HighPriorityButtonPressEvent& event);
    bool handleMenuToggle();
    bool handleMenuNavigation(int8_t direction);
    bool handleMenuSelection();
    bool handleMenuBack();
    
    // Logique de navigation
    void navigateUp();
    void navigateDown(); 
    void selectCurrentItem();
    void goBack();
    void toggleMenu();
};
```

### 4.2 Types d'événements de menu ❌
```cpp
// Extensions des types d'événements existants
constexpr EventType MenuToggle = 1006;
constexpr EventType MenuNavigate = 1007;
constexpr EventType MenuSelect = 1008;
constexpr EventType MenuBack = 1009;
constexpr EventType MenuItemAction = 1010;
```

## PHASE 5 : Configuration et structure du menu ❌

### 5.1 MenuConfiguration - Configuration du menu ❌
```cpp
struct MenuConfiguration {
    std::vector<MenuItem> items;
    MenuDisplaySettings display;
    MenuNavigationSettings navigation;
    MenuThemeSettings theme;
};

struct MenuItem {
    uint8_t id;
    String label;
    String action;
    String icon;
    uint8_t parentId;
    bool enabled;
    std::vector<uint8_t> children;
};
```

### 5.2 Structure hiérarchique du menu ❌
```
Menu Principal
├── Paramètres
│   ├── Affichage
│   ├── MIDI
│   └── Navigation
├── Profils
│   ├── Charger Profil
│   ├── Sauvegarder Profil
│   └── Profil par défaut
├── Calibration
│   ├── Encodeurs
│   └── Boutons
└── À propos
    ├── Version
    ├── Mémoire
    └── Informations système
```

## PHASE 6 : Gestion d'état et navigation ❌

### 6.1 Machine d'état du menu ❌
```cpp
enum class MenuState {
    HIDDEN,
    SHOWING,
    VISIBLE,
    NAVIGATING,
    SELECTING,
    TRANSITIONING,
    HIDING
};
```

### 6.2 Pile de navigation ❌
```cpp
struct MenuLevel {
    uint8_t parentId;
    std::vector<uint8_t> items;
    int8_t selectedIndex;
    String title;
    String breadcrumb;
};
```

## PHASE 7 : Intégration avec le système existant ❌

### 7.1 Intégration ViewManager ❌
Étendre l'interface `ViewManager` :
```cpp
virtual void showMenu() = 0;
virtual void hideMenu() = 0;
virtual void toggleMenu() = 0;
virtual void navigateMenu(int8_t direction) = 0;
virtual void selectMenuItem() = 0;
virtual void goBackInMenu() = 0;
virtual bool isMenuVisible() const = 0;
```

### 7.2 Intégration EventBus ❌
- [ ] Abonnement haute priorité pour réponse en temps réel
- [ ] Filtrage d'événements pour éviter les conflits avec parameter view
- [ ] Routage d'événements basé sur l'état

### 7.3 Intégration de configuration ❌
Ajouter la configuration de menu à `UnifiedConfiguration` :
```cpp
struct MenuConfiguration {
    std::vector<MenuItem> items;
    MenuDisplaySettings display;
    MenuNavigationSettings navigation;
};
```

## PHASE 8 : Fonctionnalités avancées ❌

### 8.1 Actions de menu ❌
- [ ] Ajustements de paramètres
- [ ] Changement de profils
- [ ] Paramètres système
- [ ] Modes de calibration
- [ ] Informations système

### 8.2 Améliorations visuelles ❌
- [ ] Animations fluides
- [ ] Icônes et indicateurs
- [ ] Navigation en fil d'Ariane
- [ ] Aide contextuelle
- [ ] Thème cohérent avec UITheme

## PHASE 9 : Tests et optimisation ❌

### 9.1 Tests de navigation ❌
- [ ] Test du bouton home (basculement du menu)
- [ ] Test de l'encodeur de navigation (navigation dans les éléments)
- [ ] Test du bouton d'encodeur (sélection/entrée dans sous-menu)
- [ ] Test du bouton OK (annulation/retour)

### 9.2 Tests de performance ❌
- [ ] Temps de réponse des événements
- [ ] Utilisation mémoire
- [ ] Fluidité des animations
- [ ] Stabilité sur matériel

## PHASE 10 : Documentation et finalisation ❌

### 10.1 Documentation technique ❌
- [ ] Architecture des composants
- [ ] Diagrammes de flux d'événements
- [ ] Guide d'extension du menu
- [ ] API de configuration

### 10.2 Validation finale ❌
- [ ] Tests d'intégration complets
- [ ] Validation sur hardware Teensy 4.1
- [ ] Optimisation mémoire finale
- [ ] Validation des performances

## Ordre d'implémentation recommandé

1. **Correction du système de navigation** (Phase 1)
2. **MenuViewController et types d'événements** (Phase 2.1, 4.2)
3. **MenuConfigurationExtractor** (Phase 2.2)
4. **MenuItemMappingManager** (Phase 2.3)
5. **MenuSceneManager** (Phase 2.4)
6. **MenuEventHandler** (Phase 4.1)
7. **MenuItemWidget** (Phase 3.1)
8. **Intégration ViewManager** (Phase 7.1)
9. **Tests et optimisation** (Phase 9)
10. **Documentation** (Phase 10)

## Fichiers à créer

### Core (Domain)
- `src/core/domain/menu/MenuTypes.hpp`
- `src/core/domain/menu/MenuState.hpp`
- `src/core/domain/menu/MenuEvents.hpp`

### Use Cases
- `src/core/use_cases/menu/MenuViewController.hpp/.cpp`
- `src/core/use_cases/menu/MenuConfigurationExtractor.hpp/.cpp`

### Adapters
- `src/adapters/primary/ui/menu/MenuSceneManager.hpp/.cpp`
- `src/adapters/primary/ui/menu/MenuEventHandler.hpp/.cpp`
- `src/adapters/primary/ui/menu/MenuItemMappingManager.hpp/.cpp`

### Widgets
- `src/adapters/ui/lvgl/widgets/MenuItemWidget.hpp/.cpp`
- `src/adapters/ui/lvgl/widgets/MenuTitleBar.hpp/.cpp`

### Configuration
- `src/config/menu/MenuConfiguration.hpp/.cpp`

## Principes directeurs

- **Suivre exactement le pattern parameter view**
- **Réutiliser l'architecture EventBus**
- **Maintenir la séparation des responsabilités**
- **Conception axée sur la performance**
- **Implémentation efficace en mémoire**
- **Gestion d'erreurs cohérente**
- **Tests continus**
- **Documentation exhaustive**

## Protocole de développement et validation

### 🔧 Compilation et tests obligatoires à chaque étape

**RÈGLE FONDAMENTALE :** Aucune étape ne peut être considérée comme terminée tant que :
1. ✅ **Le code compile sans erreur ni warning**
2. ✅ **Les tests unitaires existants continuent de passer**
3. ✅ **Les nouveaux tests unitaires passent**
4. ✅ **Le code legacy est clairement marqué pour suppression ultérieure**

### 📋 Protocole de validation par étape

#### Pour chaque sous-étape :
1. **Implémentation** du code selon les spécifications
2. **Compilation** : `pio run -e MIDI_CONTROLLER_DEVELOPMENT`
3. **Tests unitaires** : `pio test -e MIDI_CONTROLLER_DEVELOPMENT`
4. **Marquage du code legacy** avec les commentaires appropriés
5. **Documentation** des changements dans le plan
6. **Commit** sur la branche main avec message descriptif

#### Exemple de workflow :
```bash
# 1. Implémenter le code
# 2. Compiler
pio run -e MIDI_CONTROLLER_DEVELOPMENT

# 3. Exécuter les tests
pio test -e MIDI_CONTROLLER_DEVELOPMENT

# 4. Si tout passe, commit
git add .
git commit -m "feat: Phase 1.1 - Correction enregistrement contrôles navigation

- Ajout du scan des contrôles de navigation dans InitializationScript
- Enregistrement des IDs 51, 52, 79, 1079 avec NavigationConfigService
- Filtrage approprié dans MidiMapper
- Tests unitaires pour NavigationConfigService ajoutés
- Marquage du code legacy dans LvglMenuView

🧪 Tests: ✅ Compilation OK, ✅ Tests unitaires OK"
```

### 🧪 Stratégie de tests unitaires

#### Tests requis pour chaque composant :
1. **Tests de construction/destruction** (pas de fuites mémoire)
2. **Tests de configuration** (paramètres valides/invalides)
3. **Tests de logique métier** (navigation, sélection, etc.)
4. **Tests d'intégration** (avec EventBus, ViewManager, etc.)
5. **Tests de performance** (temps de réponse, mémoire)

#### Structure des tests :
```cpp
// Exemple pour MenuViewController
class MenuViewControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Configuration du test
    }
    
    void TearDown() override {
        // Nettoyage après test
    }
    
    std::unique_ptr<MenuViewController> controller_;
    std::shared_ptr<MockEventBus> mockEventBus_;
    std::shared_ptr<MockViewManager> mockViewManager_;
};

TEST_F(MenuViewControllerTest, InitializationSuccess) {
    // Test d'initialisation réussie
}

TEST_F(MenuViewControllerTest, HandleNavigationEvents) {
    // Test de gestion des événements de navigation
}
```

### 🏷️ Marquage du code legacy

#### Conventions de marquage :
```cpp
// === CODE LEGACY - À SUPPRIMER APRÈS MIGRATION MENU ===
// TODO: Supprimer après implémentation complète du nouveau système de menu
// Date: [Date actuelle]
// Remplacé par: [Nouveau composant]
class LegacyMenuView {
    // Code legacy...
};
// === FIN CODE LEGACY ===
```

#### Fichiers à marquer comme legacy :
1. **`LvglMenuView.hpp/.cpp`** - Remplacé par le nouveau système modulaire
2. **Parties de `UIController`** - Navigation hardcodée
3. **Parties de `ViewManagerEventListener`** - Gestion des événements de navigation
4. **Méthodes de navigation hardcodées** dans divers composants

### 📊 Suivi de progression par étape

#### Validation d'étape complète :
- [ ] **Code implémenté** selon les spécifications
- [ ] **Compilation réussie** (0 erreurs, 0 warnings)
- [ ] **Tests unitaires** nouveaux créés et passent
- [ ] **Tests de régression** (anciens tests) passent
- [ ] **Code legacy marqué** avec commentaires appropriés
- [ ] **Documentation** mise à jour dans le plan
- [ ] **Commit effectué** avec message descriptif
- [ ] **Validation manuelle** sur hardware (si applicable)

#### Messages de commit standardisés :
```
feat: [Phase X.Y] - [Description courte]

- [Détail 1]
- [Détail 2]
- [Détail 3]

🧪 Tests: ✅ Compilation OK, ✅ Tests unitaires OK
📋 Code legacy: [Fichiers marqués]
🏗️ Architecture: [Composants créés/modifiés]
```

### ⚠️ Critères d'arrêt et escalade

#### Arrêt immédiat si :
1. **Compilation échoue** après 2 tentatives de correction
2. **Tests unitaires échouent** de manière inexpliquée
3. **Régression détectée** sur fonctionnalités existantes
4. **Problème de mémoire** identifié
5. **Conflit architectural** majeur découvert

#### Procédure d'escalade :
1. **Documenter le problème** dans le plan
2. **Marquer l'étape** comme ⚠️ (Problème identifié)
3. **Créer une issue** avec tous les détails
4. **Revenir à la dernière version stable** si nécessaire
5. **Revoir l'approche** avant de continuer

### 🎯 Objectifs de qualité

#### Métriques de qualité à maintenir :
- **Couverture de tests** : > 80% pour le nouveau code
- **Temps de compilation** : < 2 minutes
- **Temps d'exécution tests** : < 30 secondes
- **Utilisation mémoire** : Pas d'augmentation > 10%
- **Warnings de compilation** : 0

#### Validation continue :
- **Tests automatisés** à chaque commit
- **Revue de code** systématique
- **Documentation** maintenue à jour
- **Performance** surveillée en continu

## Statut d'avancement

### Légende
- ✅ Terminé
- 🔄 En cours
- ❌ À faire
- ⚠️ Problème identifié
- 🔍 À analyser

*Dernière mise à jour : [Date de création]*