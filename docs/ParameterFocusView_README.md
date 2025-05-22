# ParameterFocusView - Documentation

## Vue d'ensemble

La `ParameterFocusView` est une nouvelle vue qui affiche un paramètre MIDI unique avec une interface visuelle immersive, inspirée de l'interface "CUTOFF" présentée dans l'image de référence.

## Caractéristiques

### Interface Visuelle
- **CC numéro** affiché en haut à gauche (ex: "CC 127")
- **Canal MIDI** affiché en haut à droite (ex: "CH 16") 
- **Cercle central** avec remplissage proportionnel à la valeur (0-127)
- **Nom du paramètre** centré en bas (ex: "CUTOFF")

### Fonctionnalités
- Mise à jour en temps réel de la valeur via `setValue()`
- Affichage immédiat lors de changements de paramètres MIDI
- Rendu optimisé avec la fonction `drawArc()` existante du SSD1306
- Intégration complète dans le système de vues existant

## Architecture

```
ParameterFocusView
├── Hérite de View (interface commune)
├── Utilise Ssd1306Display::drawArc() pour le cercle
├── Intégrée dans DefaultViewManager
└── Accessible via ViewManager::showParameterFocusView()
```

## Utilisation

### Affichage d'un paramètre

```cpp
// Afficher CC 127, Canal 16, Valeur 64, Nom "CUTOFF"
viewManager.showParameterFocusView(127, 16, 64, "CUTOFF");
```

### Mise à jour de la valeur

```cpp
// Mettre à jour seulement la valeur (pour les changements rapides d'encodeur)
viewManager.updateParameterFocusValue(newValue);
```

### Intégration avec les événements MIDI

```cpp
// Réagir aux changements de CC
void onMidiCCChanged(uint8_t cc, uint8_t channel, uint8_t value) {
    String paramName = mapCCToName(cc);
    viewManager.showParameterFocusView(cc, channel, value, paramName);
}
```

## Configuration

### Layout (positions modifiables dans le header)
```cpp
static const int CC_LABEL_X = 2;        // Position "CC 127"
static const int CC_LABEL_Y = 2;
static const int CH_LABEL_X = 90;       // Position "CH 16"
static const int CH_LABEL_Y = 2;
static const int PARAM_NAME_Y = 55;     // Position nom paramètre

static const int CIRCLE_CENTER_X = 64;  // Centre du cercle
static const int CIRCLE_CENTER_Y = 32;
static const int CIRCLE_RADIUS = 22;    // Rayon du cercle
```

### Mapping des paramètres courants
```cpp
struct ParameterMapping {
    uint8_t cc;
    String name;
} commonParams[] = {
    {1, "MOD WHEEL"},
    {7, "VOLUME"},
    {10, "PAN"},
    {71, "RESONANCE"},
    {74, "FREQUENCY"},
    {127, "CUTOFF"}
};
```

## Implémentation Technique

### Rendu du Cercle Rempli
La méthode `drawFilledCircleArc()` utilise plusieurs passes de `drawArc()` pour créer un effet de remplissage:

```cpp
void drawFilledCircleArc(int centerX, int centerY, int radius, float fillRatio) {
    // Calculer l'angle basé sur le ratio (0.0-1.0 → 0°-360°)
    int startAngle = -90;  // Commencer en haut
    int sweepAngle = (int)(fillRatio * 360.0f);
    int endAngle = startAngle + sweepAngle;
    
    // Plusieurs passes pour effet de remplissage
    for (int r = 2; r < radius - 1; r += 2) {
        drawArc(centerX, centerY, r, startAngle, endAngle, WHITE, 2);
    }
}
```

### Performance
- Rendu optimisé avec marquage "dirty" uniquement si nécessaire
- Limitation de fréquence de mise à jour pour éviter le scintillement
- Réutilisation de la fonction `drawArc()` existante et optimisée

### Intégration dans le ViewManager
- Ajout du type `ViewType::ParameterFocus`
- Méthodes `showParameterFocusView()` et `updateParameterFocusValue()`
- Gestion automatique de l'activation/désactivation des vues

## Cas d'Usage Typiques

### 1. Feedback d'Encodeur
Quand l'utilisateur tourne un encodeur, afficher immédiatement le paramètre correspondant.

### 2. Monitoring MIDI
Afficher visuellement les changements de CC reçus via MIDI IN.

### 3. Mode d'Édition
Interface dédiée pour l'édition fine de paramètres avec feedback visuel clair.

### 4. Mode d'Apprentissage
Aide l'utilisateur à comprendre quels paramètres sont modifiés.

## Extensions Possibles

### Améliorations Visuelles
- Animation de transition lors du changement de valeur
- Indicateur de direction du changement (+ ou -)
- Couleurs différentes selon le type de paramètre

### Fonctionnalités Avancées
- Historique des dernières valeurs
- Indication des limites min/max personnalisées
- Mode édition avec step par step

### Intégration Système
- Timeout automatique pour revenir à la vue précédente
- Sauvegarde automatique des paramètres modifiés
- Intégration avec les profils utilisateur

## Exemples de Paramètres Courants

| CC  | Nom Standard | Description |
|-----|--------------|-------------|
| 1   | MOD WHEEL    | Roue de modulation |
| 7   | VOLUME       | Volume principal |
| 10  | PAN          | Panoramique |
| 11  | EXPRESSION   | Expression |
| 71  | RESONANCE    | Résonance du filtre |
| 72  | RELEASE      | Temps de release |
| 73  | ATTACK       | Temps d'attack |
| 74  | FREQUENCY    | Fréquence de coupure |
| 75  | DECAY        | Temps de decay |
| 91  | REVERB       | Niveau de reverb |
| 93  | CHORUS       | Niveau de chorus |
| 127 | CUTOFF       | Cutoff du filtre |

## Migration depuis SingleParameterView

Si vous utilisez actuellement `SingleParameterView`, la migration est simple:

```cpp
// Ancien code
singleParameterView->setParameter("CUTOFF", "Q.", 64, 16);

// Nouveau code
viewManager.showParameterFocusView(127, 16, 64, "CUTOFF");
```

La `ParameterFocusView` offre une interface plus claire et plus immersive, particulièrement adaptée aux interfaces tactiles et à l'usage en direct.