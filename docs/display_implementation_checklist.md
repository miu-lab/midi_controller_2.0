# Checklist d'Implémentation du Système d'Affichage

## Phase 1 : Correction du Rendu de Texte (PRIORITÉ CRITIQUE)

### 1.1 Analyse et Préparation
- [ ] **Analyser l'implémentation actuelle** de `Ili9341TgxDisplay::drawText()`
  - [ ] Identifier pourquoi le texte n'est pas rendu (rectangles au lieu de lettres)
  - [ ] Vérifier les fontes TGX disponibles dans le projet
  - [ ] Comprendre l'API TGX pour le rendu de texte
  - **Test** : Compiler le projet et identifier les erreurs actuelles

- [ ] **Vérifier les dépendances TGX**
  - [ ] Confirmer que les fontes TGX sont incluses dans platformio.ini
  - [ ] Vérifier la version de la bibliothèque TGX
  - [ ] S'assurer que les headers des fontes sont disponibles
  - **Test** : Compiler avec includes des fontes TGX

### 1.2 Implémentation du Rendu de Texte de Base

- [ ] **Modifier `Ili9341TgxDisplay.hpp`**
  - [ ] Ajouter les includes des fontes TGX (Arial 10, 12, 14, 16, 20, 24)
  - [ ] Ajouter une structure `FontConfig` pour gérer les fontes
  - [ ] Ajouter des variables membres pour les fontes par défaut
  - [ ] Ajouter l'énumération `FontSize` dans DisplayPort.hpp
  - **Test** : Compiler sans erreurs

- [ ] **Implémenter `drawText()` correctement**
  - [ ] Remplacer l'implémentation actuelle par l'API TGX native
  - [ ] Utiliser `tgx_.drawText()` avec une fonte TGX
  - [ ] Gérer les cas où text == nullptr
  - **Test** : Compiler et vérifier qu'il n'y a pas d'erreurs de linkage

- [ ] **Implémenter `getTextBounds()` avec TGX**
  - [ ] Utiliser `tgx_.measureText()` pour obtenir les vraies dimensions
  - [ ] Retourner des valeurs précises au lieu d'estimations
  - **Test** : Compiler et vérifier la cohérence des mesures

- [ ] **Ajouter la méthode `setFont()`**
  - [ ] Permettre de changer entre différentes tailles de fonte
  - [ ] Implémenter un système de cache des fontes
  - **Test** : Compiler et tester le changement de fonte

### 1.3 Test du Rendu de Texte

- [ ] **Créer un test simple**
  - [ ] Modifier une vue existante (SplashScreenView) pour afficher du texte
  - [ ] Utiliser différentes tailles de fonte
  - [ ] Tester l'alignement du texte
  - **Test** : Compiler, uploader et vérifier que le texte s'affiche correctement

- [ ] **Test de l'affichage centré**
  - [ ] Implémenter `drawCenteredText()` avec les nouvelles mesures
  - [ ] Tester sur différentes longueurs de texte
  - **Test** : Vérifier que le texte est bien centré à l'écran

- [ ] **Validation complète Phase 1**
  - [ ] Le texte s'affiche avec de vraies lettres (pas de rectangles)
  - [ ] Les différentes tailles de fonte fonctionnent
  - [ ] L'alignement de texte est correct
  - [ ] Les mesures de texte sont précises
  - [ ] Aucune régression sur les autres fonctionnalités d'affichage
  - **Test** : Tests complets avec toutes les vues existantes

## Phase 2 : Adaptation aux Dimensions ILI9341

### 2.1 Mise à Jour des Constantes
- [ ] **Identifier les dimensions hardcodées**
  - [ ] Rechercher "128" et "64" dans le code des vues
  - [ ] Lister toutes les occurrences à modifier
  - **Test** : Grep pour identifier tous les endroits à modifier

- [ ] **Créer des constantes d'écran**
  - [ ] Définir `SCREEN_WIDTH = 240` et `SCREEN_HEIGHT = 320`
  - [ ] Centraliser dans un fichier de configuration
  - **Test** : Compiler avec les nouvelles constantes

### 2.2 Adaptation des Vues
- [ ] **SplashScreenView**
  - [ ] Adapter les positions des éléments pour 240x320
  - [ ] Redimensionner les éléments graphiques
  - **Test** : Vérifier l'affichage du splash screen

- [ ] **MenuView**
  - [ ] Adapter la taille et position des éléments de menu
  - [ ] Revoir l'espacement vertical
  - **Test** : Navigation dans le menu fonctionnelle

- [ ] **ParameterFocusView**
  - [ ] Adapter le cercle et les arcs pour la nouvelle résolution
  - [ ] Repositionner les labels et valeurs
  - **Test** : Affichage correct des paramètres focalisés

- [ ] **ModalView**
  - [ ] Adapter la taille des modales
  - [ ] Centrer correctement sur l'écran
  - **Test** : Affichage correct des messages modaux

### 2.3 Validation Phase 2
- [ ] **Test de toutes les vues**
  - [ ] Chaque vue s'affiche correctement en 240x320
  - [ ] Aucun élément ne déborde de l'écran
  - [ ] La navigation entre vues fonctionne
  - **Test** : Test complet de l'interface utilisateur

## Phase 3 : Framework de Widgets de Base

### 3.1 Architecture des Widgets
- [ ] **Créer la structure de répertoires**
  - [ ] Créer `src/core/ui/widgets/`
  - [ ] Créer `src/core/ui/layouts/`
  - [ ] Créer `src/core/ui/theme/`
  - **Test** : Structure de projet cohérente

- [ ] **Implémenter la classe Widget de base**
  - [ ] Créer `Widget.hpp` avec les méthodes virtuelles
  - [ ] Implémenter la gestion des bounds et de la visibilité
  - [ ] Ajouter le système d'invalidation
  - **Test** : Compiler la classe de base

### 3.2 Widget Label
- [ ] **Implémenter Label.hpp/.cpp**
  - [ ] Hériter de Widget
  - [ ] Implémenter setText(), setTextAlign(), setFontSize()
  - [ ] Implémenter draw() avec l'affichage de texte
  - **Test** : Créer et afficher un Label simple

- [ ] **Test du widget Label**
  - [ ] Tester différents alignements (gauche, centre, droite)
  - [ ] Tester différentes tailles de fonte
  - [ ] Tester le changement de couleur de texte
  - **Test** : Label fonctionnel dans toutes ses configurations

### 3.3 Widget Button
- [ ] **Implémenter Button.hpp/.cpp**
  - [ ] Hériter de Label
  - [ ] Ajouter la gestion des états (normal, hovered, pressed)
  - [ ] Implémenter handleInput() pour les clics
  - **Test** : Bouton réactif aux interactions

- [ ] **Test du widget Button**
  - [ ] Tester les changements d'état visuels
  - [ ] Tester les callbacks onClick
  - [ ] Tester l'affichage du texte sur le bouton
  - **Test** : Bouton entièrement fonctionnel

### 3.4 Widget ProgressBar
- [ ] **Implémenter ProgressBar.hpp/.cpp**
  - [ ] Hériter de Widget
  - [ ] Implémenter setValue() et les options d'affichage
  - [ ] Ajouter support horizontal/vertical
  - **Test** : Barre de progression affichant correctement les valeurs

- [ ] **Validation Phase 3**
  - [ ] Tous les widgets de base fonctionnent indépendamment
  - [ ] Les widgets s'affichent correctement
  - [ ] L'interaction utilisateur fonctionne
  - **Test** : Demo avec tous les widgets sur une même vue

## Phase 4 : Système de Layout

### 4.1 Layout de Base
- [ ] **Implémenter Layout.hpp**
  - [ ] Interface abstraite pour tous les layouts
  - [ ] Gestion de la liste des widgets enfants
  - [ ] Méthodes pour padding et spacing
  - **Test** : Compiler l'interface de base

### 4.2 GridLayout
- [ ] **Implémenter GridLayout.hpp/.cpp**
  - [ ] Hériter de Layout
  - [ ] Implémenter performLayout() pour disposition en grille
  - [ ] Ajouter support des spans (rowspan, colspan)
  - **Test** : Widgets disposés correctement en grille

- [ ] **Test du GridLayout**
  - [ ] Tester grille 2x2, 3x3, etc.
  - [ ] Tester avec différents paddings/spacings
  - [ ] Tester le redimensionnement automatique
  - **Test** : Layout adaptatif fonctionnel

### 4.3 Validation Phase 4
- [ ] **Test d'intégration Layout + Widgets**
  - [ ] Widgets automatiquement positionnés par le layout
  - [ ] Redimensionnement automatique lors de changements
  - [ ] Performance acceptable pour le rendu
  - **Test** : Interface complète avec layout automatique

## Phase 5 : Système de Thème

### 5.1 Définition du Thème
- [ ] **Implémenter Theme.hpp/.cpp**
  - [ ] Définir ColorScheme avec toutes les couleurs
  - [ ] Définir Typography avec les tailles de fonte
  - [ ] Définir Spacing avec les constantes d'espacement
  - **Test** : Compiler les définitions de thème

### 5.2 Application du Thème
- [ ] **Intégrer le thème dans les widgets**
  - [ ] Modifier Widget.hpp pour accepter un thème
  - [ ] Appliquer les couleurs du thème dans Label, Button, ProgressBar
  - **Test** : Widgets utilisant les couleurs du thème

- [ ] **Créer des thèmes prédéfinis**
  - [ ] Implémenter darkTheme(), lightTheme(), highContrastTheme()
  - [ ] Permettre le changement de thème à l'exécution
  - **Test** : Basculer entre différents thèmes

### 5.3 Validation Phase 5
- [ ] **Test des thèmes**
  - [ ] Interface cohérente avec le thème sombre
  - [ ] Changement de thème sans redémarrage
  - [ ] Lisibilité du texte dans tous les thèmes
  - **Test** : Interface professionnelle avec thème appliqué

## Phase 6 : Widgets Spécialisés MIDI

### 6.1 MidiControlKnob
- [ ] **Implémenter MidiControlKnob.hpp/.cpp**
  - [ ] Widget composé avec indicateur rotatif
  - [ ] Gestion des valeurs MIDI (0-127)
  - [ ] Interaction par glissement vertical
  - **Test** : Knob réactif affichant les valeurs MIDI

### 6.2 ParameterEditor
- [ ] **Implémenter ParameterEditor.hpp/.cpp**
  - [ ] Combinaison Label + ProgressBar + ValueDisplay
  - [ ] Mode édition avec surbrillance
  - [ ] Support des paramètres bipolaires
  - **Test** : Éditeur de paramètre complet et fonctionnel

### 6.3 Validation Phase 6
- [ ] **Test des widgets MIDI**
  - [ ] Widgets spécialisés fonctionnels
  - [ ] Intégration avec le système MIDI existant
  - [ ] Interface utilisateur intuitive
  - **Test** : Contrôles MIDI entièrement opérationnels

## Phase 7 : Intégration et Refactoring des Vues

### 7.1 Refactoring MenuView
- [ ] **Convertir MenuView au nouveau système**
  - [ ] Utiliser GridLayout pour l'organisation
  - [ ] Remplacer le code de rendu par des widgets
  - [ ] Conserver la logique de navigation
  - **Test** : Menu fonctionnel avec nouveau système UI

### 7.2 Refactoring des Autres Vues
- [ ] **Convertir ParameterFocusView**
  - [ ] Utiliser MidiControlKnob et ParameterEditor
  - [ ] Améliorer l'affichage des arcs de cercle
  - **Test** : Vista de paramètres améliorée

- [ ] **Convertir ModalView et SplashScreenView**
  - [ ] Utiliser les nouveaux widgets pour l'affichage
  - [ ] Maintenir les fonctionnalités existantes
  - **Test** : Toutes les vues converties et fonctionnelles

### 7.3 Validation Finale
- [ ] **Test complet du système**
  - [ ] Toutes les vues utilisent le nouveau système UI
  - [ ] Performance acceptable (pas de ralentissement notable)
  - [ ] Aucune régression fonctionnelle
  - [ ] Interface moderne et cohérente
  - **Test** : Système complet prêt pour production

## Critères de Validation Globaux

### Tests de Régression
- [ ] **Fonctionnalités MIDI**
  - [ ] Les contrôleurs MIDI fonctionnent toujours
  - [ ] La navigation par encodeurs fonctionne
  - [ ] L'envoi de messages MIDI n'est pas affecté

- [ ] **Performance**
  - [ ] Temps de réponse de l'affichage ≤ 50ms
  - [ ] Fluidité de l'animation acceptable
  - [ ] Consommation mémoire dans les limites

- [ ] **Stabilité**
  - [ ] Aucun crash pendant 30 minutes d'utilisation
  - [ ] Changement de vues stable
  - [ ] Gestion correcte des cas d'erreur

### Documentation
- [ ] **Mise à jour de la documentation**
  - [ ] CLAUDE.md mis à jour avec nouvelles capacités
  - [ ] Exemples d'utilisation des widgets
  - [ ] Guide de création de nouveaux widgets

---

## Instructions d'Usage de cette Checklist

1. **Progression séquentielle** : Compléter chaque phase avant de passer à la suivante
2. **Tests obligatoires** : Chaque élément marqué "Test :" doit être validé avant de continuer
3. **Validation des phases** : Une phase n'est complète que si tous ses tests passent
4. **Rollback si nécessaire** : En cas d'échec, revenir à l'état stable précédent
5. **Compilation continue** : Le code doit compiler après chaque étape majeure

**Format de validation :**
- [ ] Non commencé
- [🔄] En cours
- [✅] Terminé et testé
- [❌] Échec - nécessite correction