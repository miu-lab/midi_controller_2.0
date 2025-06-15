# Plan d'Intégration Complète du Système d'Affichage

## Vue d'ensemble

Ce document détaille le plan complet pour intégrer un système d'affichage moderne et flexible au projet MIDI Controller 2.0, exploitant pleinement les capacités de l'écran ILI9341 (240x320 pixels) et de la bibliothèque graphique TGX.

## État actuel et problèmes identifiés

### Problèmes critiques
1. **Rendu de texte non fonctionnel** : La méthode `drawText()` dessine des rectangles au lieu de caractères
2. **Dimensions inadaptées** : Les vues utilisent 128x64 (OLED) au lieu de 240x320 (ILI9341)
3. **API TGX sous-exploitée** : Les fontes et capacités avancées de TGX ne sont pas utilisées
4. **Architecture UI limitée** : Pas de système de widgets réutilisables

### Capacités non exploitées de TGX
- Fontes vectorielles avec anti-aliasing
- Gestion avancée du texte (ancrage, mesure précise)
- Primitives graphiques optimisées
- Support des transformations et gradients
- Rendu optimisé avec double buffering

## Architecture proposée

### Structure des répertoires

```
src/
├── adapters/
│   └── secondary/
│       └── hardware/
│           └── output/
│               └── display/
│                   ├── Ili9341TgxDisplay.cpp      # Implémentation hardware
│                   └── Ili9341TgxDisplay.hpp
│
├── core/
│   ├── ports/
│   │   └── output/
│   │       └── DisplayPort.hpp                   # Interface abstraite
│   │
│   └── ui/                                       # NOUVEAU: Core UI
│       ├── widgets/                              # Widgets de base
│       │   ├── Widget.hpp                        # Classe de base
│       │   ├── Label.hpp
│       │   ├── Label.cpp
│       │   ├── Button.hpp
│       │   ├── Button.cpp
│       │   ├── ProgressBar.hpp
│       │   ├── ProgressBar.cpp
│       │   ├── ValueDisplay.hpp
│       │   ├── ValueDisplay.cpp
│       │   ├── Icon.hpp
│       │   ├── Icon.cpp
│       │   ├── Knob.hpp
│       │   └── Knob.cpp
│       │
│       ├── containers/                           # Conteneurs et layouts
│       │   ├── Container.hpp
│       │   ├── Panel.hpp
│       │   ├── Panel.cpp
│       │   ├── ScrollableList.hpp
│       │   └── ScrollableList.cpp
│       │
│       ├── layouts/                              # Gestionnaires de layout
│       │   ├── Layout.hpp
│       │   ├── GridLayout.hpp
│       │   ├── GridLayout.cpp
│       │   ├── FlexLayout.hpp
│       │   ├── FlexLayout.cpp
│       │   └── AbsoluteLayout.hpp
│       │
│       ├── theme/                                # Système de thème
│       │   ├── Theme.hpp
│       │   ├── Theme.cpp
│       │   ├── ColorScheme.hpp
│       │   └── Typography.hpp
│       │
│       └── rendering/                            # Moteur de rendu
│           ├── RenderContext.hpp
│           ├── TextRenderer.hpp
│           └── TextRenderer.cpp
│
└── adapters/
    └── primary/
        └── ui/
            ├── views/                            # Vues refactorisées
            │   ├── MenuView.cpp
            │   ├── ParameterFocusView.cpp
            │   └── SplashScreenView.cpp
            │
            └── view_components/                  # NOUVEAU: Composants complexes
                ├── MidiControlKnob.hpp          # Widget composite pour contrôle MIDI
                ├── MidiControlKnob.cpp
                ├── ParameterEditor.hpp           # Éditeur de paramètres
                └── ParameterEditor.cpp
```

### Diagramme d'architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                    View Manager                         │ │
│  │  ┌──────────┐  ┌──────────┐  ┌────────────────────┐  │ │
│  │  │ MenuView │  │ModalView│  │ParameterFocusView │  │ │
│  │  └──────────┘  └──────────┘  └────────────────────┘  │ │
│  └────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│                    UI Framework Layer                        │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                  View Components                        │ │
│  │  ┌───────────────┐  ┌─────────────────┐              │ │
│  │  │ MidiControlKnob│  │ ParameterEditor │              │ │
│  │  └───────────────┘  └─────────────────┘              │ │
│  └────────────────────────────────────────────────────────┘ │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                     Widgets                             │ │
│  │  ┌──────┐ ┌────────┐ ┌────────────┐ ┌──────┐        │ │
│  │  │Label │ │ Button │ │ProgressBar│ │ Knob │        │ │
│  │  └──────┘ └────────┘ └────────────┘ └──────┘        │ │
│  └────────────────────────────────────────────────────────┘ │
│  ┌────────────────────────────────────────────────────────┐ │
│  │              Layout & Container System                  │ │
│  │  ┌──────────┐  ┌────────────┐  ┌──────────────┐      │ │
│  │  │GridLayout│  │ FlexLayout │  │AbsoluteLayout│      │ │
│  │  └──────────┘  └────────────┘  └──────────────┘      │ │
│  └────────────────────────────────────────────────────────┘ │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                   Theme System                          │ │
│  │  ┌────────────┐  ┌────────────┐  ┌─────────────┐     │ │
│  │  │ColorScheme │  │ Typography │  │   Spacing   │     │ │
│  │  └────────────┘  └────────────┘  └─────────────┘     │ │
│  └────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│                   Rendering Layer                            │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                  Text Renderer                          │ │
│  │          (TGX Font Integration & Caching)              │ │
│  └────────────────────────────────────────────────────────┘ │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                   DisplayPort                           │ │
│  │              (Abstract Interface)                       │ │
│  └────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│                   Hardware Layer                             │
│  ┌────────────────────────────────────────────────────────┐ │
│  │               Ili9341TgxDisplay                         │ │
│  │         (TGX Library + ILI9341 Driver)                 │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## Phases de développement détaillées

### Phase 1 : Correction du rendu de texte (Priorité CRITIQUE)

#### 1.1 Intégration des fontes TGX

**Fichier**: `src/adapters/secondary/hardware/output/display/Ili9341TgxDisplay.hpp`

```cpp
// Ajouter les includes des fontes
#include <font_tgx_Arial_10.h>
#include <font_tgx_Arial_12.h>
#include <font_tgx_Arial_14.h>
#include <font_tgx_Arial_16.h>
#include <font_tgx_Arial_20.h>
#include <font_tgx_Arial_24.h>
#include <font_tgx_OpenSans_10.h>
#include <font_tgx_OpenSans_12.h>
#include <font_tgx_OpenSans_14.h>
#include <font_tgx_OpenSans_16.h>

class Ili9341TgxDisplay : public DisplayPort {
private:
    // Ajouter un système de gestion des fontes
    struct FontConfig {
        const ILI9341_t3_font_t* tgxFont;
        uint8_t size;
        bool antialiased;
    };
    
    // Fontes par défaut pour différents usages
    FontConfig smallFont_ = {&font_tgx_Arial_10, 10, true};
    FontConfig normalFont_ = {&font_tgx_Arial_14, 14, true};
    FontConfig largeFont_ = {&font_tgx_Arial_20, 20, true};
    FontConfig titleFont_ = {&font_tgx_Arial_24, 24, true};
    
    // Fonte courante
    FontConfig* currentFont_ = &normalFont_;
};
```

#### 1.2 Implémentation correcte de drawText()

**Fichier**: `src/adapters/secondary/hardware/output/display/Ili9341TgxDisplay.cpp`

```cpp
void Ili9341TgxDisplay::drawText(const char* text, int16_t x, int16_t y) {
    if (!text || !currentFont_->tgxFont) return;
    
    // Utiliser l'API TGX native
    tgx::iVec2 position(x, y);
    
    // Dessiner le texte avec la fonte courante
    if (currentFont_->antialiased) {
        // Rendu anti-aliasé pour une meilleure qualité
        tgx_.drawText(text, position, currentFont_->tgxFont, 
                     currentColor_, 1.0f, tgx::ANCHOR_TOPLEFT);
    } else {
        // Rendu standard plus rapide
        tgx_.drawText(text, position, currentFont_->tgxFont, 
                     currentColor_);
    }
}

void Ili9341TgxDisplay::drawCenteredText(const char* text, int16_t y) {
    if (!text || !currentFont_->tgxFont) return;
    
    // Mesurer le texte pour le centrer
    auto bbox = tgx_.measureText(text, currentFont_->tgxFont);
    int16_t textWidth = bbox.lx();
    int16_t x = (width_ - textWidth) / 2;
    
    drawText(text, x, y);
}

void Ili9341TgxDisplay::getTextBounds(const char* text, int16_t x, int16_t y,
                                     int16_t* x1, int16_t* y1, 
                                     int16_t* w, int16_t* h) {
    if (!text || !currentFont_->tgxFont) {
        if (w) *w = 0;
        if (h) *h = 0;
        return;
    }
    
    // Utiliser la mesure précise de TGX
    auto bbox = tgx_.measureText(text, currentFont_->tgxFont);
    
    if (x1) *x1 = x;
    if (y1) *y1 = y;
    if (w) *w = bbox.lx();
    if (h) *h = bbox.ly();
}

// Nouvelle méthode pour changer de fonte
void Ili9341TgxDisplay::setFont(FontSize size) {
    switch (size) {
        case FontSize::SMALL:
            currentFont_ = &smallFont_;
            break;
        case FontSize::NORMAL:
            currentFont_ = &normalFont_;
            break;
        case FontSize::LARGE:
            currentFont_ = &largeFont_;
            break;
        case FontSize::TITLE:
            currentFont_ = &titleFont_;
            break;
    }
}
```

#### 1.3 Mise à jour de l'interface DisplayPort

**Fichier**: `src/core/ports/output/DisplayPort.hpp`

```cpp
class DisplayPort {
public:
    // Énumération pour les tailles de fonte standard
    enum class FontSize {
        SMALL,   // 10-12px pour infos secondaires
        NORMAL,  // 14-16px pour texte standard
        LARGE,   // 18-20px pour valeurs importantes
        TITLE    // 24px+ pour titres
    };
    
    // Énumération pour l'alignement du texte
    enum class TextAlign {
        LEFT,
        CENTER,
        RIGHT,
        TOP_LEFT,
        TOP_CENTER,
        TOP_RIGHT,
        MIDDLE_LEFT,
        MIDDLE_CENTER,
        MIDDLE_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_CENTER,
        BOTTOM_RIGHT
    };
    
    // Méthodes de texte améliorées
    virtual void setFont(FontSize size) = 0;
    virtual void drawAlignedText(const char* text, int16_t x, int16_t y, 
                                TextAlign align) = 0;
    
    // Méthodes pour des effets de texte
    virtual void drawTextWithShadow(const char* text, int16_t x, int16_t y,
                                   uint16_t shadowColor) = 0;
    virtual void drawTextWithOutline(const char* text, int16_t x, int16_t y,
                                    uint16_t outlineColor) = 0;
};
```

### Phase 2 : Framework de widgets UI

#### 2.1 Classe de base Widget

**Fichier**: `src/core/ui/widgets/Widget.hpp`

```cpp
#pragma once
#include <cstdint>
#include <memory>
#include "core/ports/output/DisplayPort.hpp"
#include "core/ui/theme/Theme.hpp"

namespace UI {

// Structure pour les dimensions et position
struct Rectangle {
    int16_t x, y, width, height;
    
    bool contains(int16_t px, int16_t py) const {
        return px >= x && px < x + width && 
               py >= y && py < y + height;
    }
    
    bool intersects(const Rectangle& other) const {
        return !(x + width <= other.x || other.x + other.width <= x ||
                y + height <= other.y || other.y + other.height <= y);
    }
};

// Événements d'entrée pour les widgets
struct InputEvent {
    enum Type { TOUCH_DOWN, TOUCH_UP, TOUCH_MOVE, ENCODER_TURN, BUTTON_PRESS };
    Type type;
    int16_t x, y;  // Pour touch
    int16_t delta; // Pour encoder
    uint8_t buttonId; // Pour boutons
};

// Classe de base pour tous les widgets
class Widget {
public:
    Widget() = default;
    virtual ~Widget() = default;
    
    // Cycle de vie
    virtual void init(std::shared_ptr<Theme> theme) { theme_ = theme; }
    virtual void update(uint32_t deltaMs) {}
    
    // Rendu
    virtual void draw(DisplayPort* display) = 0;
    virtual void invalidate() { needsRedraw_ = true; }
    virtual bool needsRedraw() const { return needsRedraw_; }
    
    // Gestion des événements
    virtual bool handleInput(const InputEvent& event) { return false; }
    virtual void onFocusGained() {}
    virtual void onFocusLost() {}
    
    // Propriétés de base
    void setVisible(bool visible) { 
        if (visible_ != visible) {
            visible_ = visible;
            invalidate();
        }
    }
    bool isVisible() const { return visible_; }
    
    void setEnabled(bool enabled) { 
        if (enabled_ != enabled) {
            enabled_ = enabled;
            invalidate();
        }
    }
    bool isEnabled() const { return enabled_; }
    
    // Géométrie
    void setBounds(const Rectangle& bounds) { 
        if (bounds_.x != bounds.x || bounds_.y != bounds.y ||
            bounds_.width != bounds.width || bounds_.height != bounds.height) {
            bounds_ = bounds;
            onBoundsChanged();
            invalidate();
        }
    }
    void setBounds(int16_t x, int16_t y, int16_t w, int16_t h) {
        setBounds({x, y, w, h});
    }
    const Rectangle& getBounds() const { return bounds_; }
    
    // Hiérarchie
    void setParent(Widget* parent) { parent_ = parent; }
    Widget* getParent() const { return parent_; }
    
protected:
    // Appelé quand les dimensions changent
    virtual void onBoundsChanged() {}
    
    // État
    Rectangle bounds_ = {0, 0, 0, 0};
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;
    bool needsRedraw_ = true;
    
    // Références
    Widget* parent_ = nullptr;
    std::shared_ptr<Theme> theme_;
};

} // namespace UI
```

#### 2.2 Widget Label

**Fichier**: `src/core/ui/widgets/Label.hpp`

```cpp
#pragma once
#include "Widget.hpp"
#include <string>

namespace UI {

class Label : public Widget {
public:
    Label(const char* text = "") : text_(text) {}
    
    // Texte
    void setText(const char* text) { 
        if (text_ != text) {
            text_ = text;
            invalidate();
        }
    }
    const std::string& getText() const { return text_; }
    
    // Style
    void setTextAlign(DisplayPort::TextAlign align) {
        if (textAlign_ != align) {
            textAlign_ = align;
            invalidate();
        }
    }
    
    void setFontSize(DisplayPort::FontSize size) {
        if (fontSize_ != size) {
            fontSize_ = size;
            invalidate();
        }
    }
    
    void setTextColor(uint16_t color) {
        if (textColor_ != color) {
            textColor_ = color;
            invalidate();
        }
    }
    
    // Auto-sizing
    void setAutoSize(bool autoSize) { autoSize_ = autoSize; }
    
    // Rendu
    void draw(DisplayPort* display) override;
    
protected:
    void onBoundsChanged() override;
    
private:
    std::string text_;
    DisplayPort::TextAlign textAlign_ = DisplayPort::TextAlign::LEFT;
    DisplayPort::FontSize fontSize_ = DisplayPort::FontSize::NORMAL;
    uint16_t textColor_ = 0xFFFF; // Blanc par défaut
    bool autoSize_ = false;
    
    // Cache pour optimisation
    int16_t cachedTextWidth_ = 0;
    int16_t cachedTextHeight_ = 0;
};

} // namespace UI
```

**Fichier**: `src/core/ui/widgets/Label.cpp`

```cpp
#include "Label.hpp"

namespace UI {

void Label::draw(DisplayPort* display) {
    if (!isVisible() || text_.empty()) return;
    
    // Appliquer le style
    display->setFont(fontSize_);
    display->setTextColor(textColor_);
    
    // Calculer la position selon l'alignement
    int16_t x = bounds_.x;
    int16_t y = bounds_.y;
    
    // Ajuster selon l'alignement
    switch (textAlign_) {
        case DisplayPort::TextAlign::CENTER:
        case DisplayPort::TextAlign::TOP_CENTER:
        case DisplayPort::TextAlign::BOTTOM_CENTER:
            x = bounds_.x + bounds_.width / 2;
            break;
            
        case DisplayPort::TextAlign::RIGHT:
        case DisplayPort::TextAlign::TOP_RIGHT:
        case DisplayPort::TextAlign::BOTTOM_RIGHT:
            x = bounds_.x + bounds_.width;
            break;
    }
    
    switch (textAlign_) {
        case DisplayPort::TextAlign::MIDDLE_LEFT:
        case DisplayPort::TextAlign::MIDDLE_CENTER:
        case DisplayPort::TextAlign::MIDDLE_RIGHT:
            y = bounds_.y + bounds_.height / 2;
            break;
            
        case DisplayPort::TextAlign::BOTTOM_LEFT:
        case DisplayPort::TextAlign::BOTTOM_CENTER:
        case DisplayPort::TextAlign::BOTTOM_RIGHT:
            y = bounds_.y + bounds_.height;
            break;
    }
    
    // Dessiner le texte
    display->drawAlignedText(text_.c_str(), x, y, textAlign_);
    
    needsRedraw_ = false;
}

void Label::onBoundsChanged() {
    if (autoSize_) {
        // Recalculer les dimensions en fonction du texte
        // TODO: Implémenter l'auto-sizing
    }
}

} // namespace UI
```

#### 2.3 Widget Button

**Fichier**: `src/core/ui/widgets/Button.hpp`

```cpp
#pragma once
#include "Label.hpp"
#include <functional>

namespace UI {

class Button : public Label {
public:
    // États du bouton
    enum class State {
        NORMAL,
        HOVERED,
        PRESSED,
        DISABLED
    };
    
    // Callbacks
    using OnClickCallback = std::function<void()>;
    using OnPressCallback = std::function<void()>;
    using OnReleaseCallback = std::function<void()>;
    
    Button(const char* text = "") : Label(text) {
        setTextAlign(DisplayPort::TextAlign::MIDDLE_CENTER);
    }
    
    // Callbacks
    void setOnClick(OnClickCallback callback) { onClick_ = callback; }
    void setOnPress(OnPressCallback callback) { onPress_ = callback; }
    void setOnRelease(OnReleaseCallback callback) { onRelease_ = callback; }
    
    // Style pour différents états
    void setBackgroundColor(State state, uint16_t color);
    void setBorderColor(State state, uint16_t color);
    void setTextColor(State state, uint16_t color);
    
    // Override
    void draw(DisplayPort* display) override;
    bool handleInput(const InputEvent& event) override;
    void onFocusGained() override { state_ = State::HOVERED; invalidate(); }
    void onFocusLost() override { state_ = State::NORMAL; invalidate(); }
    
private:
    State state_ = State::NORMAL;
    
    // Callbacks
    OnClickCallback onClick_;
    OnPressCallback onPress_;
    OnReleaseCallback onRelease_;
    
    // Styles par état
    struct StateStyle {
        uint16_t backgroundColor = 0x4208; // Gris foncé
        uint16_t borderColor = 0x8410;     // Gris clair
        uint16_t textColor = 0xFFFF;       // Blanc
    };
    
    StateStyle normalStyle_;
    StateStyle hoveredStyle_;
    StateStyle pressedStyle_;
    StateStyle disabledStyle_;
    
    StateStyle& getCurrentStyle();
};

} // namespace UI
```

#### 2.4 Widget ProgressBar

**Fichier**: `src/core/ui/widgets/ProgressBar.hpp`

```cpp
#pragma once
#include "Widget.hpp"

namespace UI {

class ProgressBar : public Widget {
public:
    enum class Orientation {
        HORIZONTAL,
        VERTICAL
    };
    
    enum class Style {
        SOLID,      // Barre pleine
        SEGMENTED,  // Segments séparés
        GRADIENT    // Dégradé de couleur
    };
    
    ProgressBar() = default;
    
    // Valeur (0.0 à 1.0)
    void setValue(float value) {
        value = std::max(0.0f, std::min(1.0f, value));
        if (value_ != value) {
            value_ = value;
            invalidate();
        }
    }
    float getValue() const { return value_; }
    
    // Configuration
    void setOrientation(Orientation orientation) {
        if (orientation_ != orientation) {
            orientation_ = orientation;
            invalidate();
        }
    }
    
    void setStyle(Style style) {
        if (style_ != style) {
            style_ = style;
            invalidate();
        }
    }
    
    // Apparence
    void setFillColor(uint16_t color) { fillColor_ = color; invalidate(); }
    void setBackgroundColor(uint16_t color) { backgroundColor_ = color; invalidate(); }
    void setBorderColor(uint16_t color) { borderColor_ = color; invalidate(); }
    void setShowPercentage(bool show) { showPercentage_ = show; invalidate(); }
    
    // Animation
    void setAnimated(bool animated) { animated_ = animated; }
    void update(uint32_t deltaMs) override;
    
    // Rendu
    void draw(DisplayPort* display) override;
    
private:
    float value_ = 0.0f;
    float displayValue_ = 0.0f; // Pour animation
    
    Orientation orientation_ = Orientation::HORIZONTAL;
    Style style_ = Style::SOLID;
    
    uint16_t fillColor_ = 0x07E0;      // Vert
    uint16_t backgroundColor_ = 0x2104; // Gris foncé
    uint16_t borderColor_ = 0x8410;    // Gris clair
    
    bool showPercentage_ = false;
    bool animated_ = true;
    
    static constexpr float ANIMATION_SPEED = 0.005f; // Par ms
};

} // namespace UI
```

### Phase 3 : Système de layout

#### 3.1 Interface Layout de base

**Fichier**: `src/core/ui/layouts/Layout.hpp`

```cpp
#pragma once
#include <vector>
#include <memory>
#include "core/ui/widgets/Widget.hpp"

namespace UI {

class Layout {
public:
    virtual ~Layout() = default;
    
    // Gestion des widgets
    virtual void addWidget(std::shared_ptr<Widget> widget) {
        widgets_.push_back(widget);
        invalidate();
    }
    
    virtual void removeWidget(std::shared_ptr<Widget> widget) {
        widgets_.erase(
            std::remove(widgets_.begin(), widgets_.end(), widget),
            widgets_.end()
        );
        invalidate();
    }
    
    // Mise en page
    virtual void performLayout(const Rectangle& bounds) = 0;
    
    // Configuration
    void setPadding(int16_t padding) { padding_ = padding; invalidate(); }
    void setSpacing(int16_t spacing) { spacing_ = spacing; invalidate(); }
    
protected:
    void invalidate() { needsLayout_ = true; }
    
    std::vector<std::shared_ptr<Widget>> widgets_;
    int16_t padding_ = 0;
    int16_t spacing_ = 0;
    bool needsLayout_ = true;
};

} // namespace UI
```

#### 3.2 GridLayout

**Fichier**: `src/core/ui/layouts/GridLayout.hpp`

```cpp
#pragma once
#include "Layout.hpp"

namespace UI {

class GridLayout : public Layout {
public:
    GridLayout(int16_t columns, int16_t rows) 
        : columns_(columns), rows_(rows) {}
    
    // Configuration de la grille
    void setColumns(int16_t columns) { columns_ = columns; invalidate(); }
    void setRows(int16_t rows) { rows_ = rows; invalidate(); }
    
    // Poids des colonnes/lignes pour sizing flexible
    void setColumnWeight(int16_t column, float weight) {
        if (column < columns_) {
            columnWeights_[column] = weight;
            invalidate();
        }
    }
    
    void setRowWeight(int16_t row, float weight) {
        if (row < rows_) {
            rowWeights_[row] = weight;
            invalidate();
        }
    }
    
    // Placement spécifique
    void addWidget(std::shared_ptr<Widget> widget, 
                  int16_t column, int16_t row,
                  int16_t columnSpan = 1, int16_t rowSpan = 1) {
        widgets_.push_back(widget);
        placements_[widget] = {column, row, columnSpan, rowSpan};
        invalidate();
    }
    
    void performLayout(const Rectangle& bounds) override;
    
private:
    struct Placement {
        int16_t column, row;
        int16_t columnSpan, rowSpan;
    };
    
    int16_t columns_;
    int16_t rows_;
    std::map<std::shared_ptr<Widget>, Placement> placements_;
    std::map<int16_t, float> columnWeights_;
    std::map<int16_t, float> rowWeights_;
};

} // namespace UI
```

### Phase 4 : Système de thème

#### 4.1 Définition du thème

**Fichier**: `src/core/ui/theme/Theme.hpp`

```cpp
#pragma once
#include <cstdint>
#include "core/ports/output/DisplayPort.hpp"

namespace UI {

// Schéma de couleurs
struct ColorScheme {
    // Couleurs principales
    uint16_t primary = 0x001F;        // Bleu
    uint16_t primaryLight = 0x319F;   // Bleu clair
    uint16_t primaryDark = 0x000F;    // Bleu foncé
    
    // Couleurs secondaires
    uint16_t secondary = 0x07E0;      // Vert
    uint16_t secondaryLight = 0x9FE0; // Vert clair
    uint16_t secondaryDark = 0x03E0;  // Vert foncé
    
    // Couleurs de base
    uint16_t background = 0x0000;     // Noir
    uint16_t surface = 0x2104;        // Gris très foncé
    uint16_t error = 0xF800;          // Rouge
    uint16_t warning = 0xFE00;        // Jaune
    uint16_t success = 0x07E0;        // Vert
    
    // Texte
    uint16_t onPrimary = 0xFFFF;      // Blanc
    uint16_t onSecondary = 0x0000;    // Noir
    uint16_t onBackground = 0xFFFF;   // Blanc
    uint16_t onSurface = 0xFFFF;      // Blanc
    uint16_t onError = 0xFFFF;        // Blanc
    
    // Utilité
    uint16_t divider = 0x4208;        // Gris
    uint16_t overlay = 0x8410;        // Gris moyen
    uint16_t disabled = 0x6318;       // Gris clair
};

// Configuration typographique
struct Typography {
    DisplayPort::FontSize h1 = DisplayPort::FontSize::TITLE;
    DisplayPort::FontSize h2 = DisplayPort::FontSize::LARGE;
    DisplayPort::FontSize h3 = DisplayPort::FontSize::NORMAL;
    DisplayPort::FontSize body1 = DisplayPort::FontSize::NORMAL;
    DisplayPort::FontSize body2 = DisplayPort::FontSize::SMALL;
    DisplayPort::FontSize caption = DisplayPort::FontSize::SMALL;
    DisplayPort::FontSize button = DisplayPort::FontSize::NORMAL;
};

// Espacement et dimensions
struct Spacing {
    int16_t tiny = 2;
    int16_t small = 4;
    int16_t medium = 8;
    int16_t large = 16;
    int16_t extraLarge = 24;
    
    int16_t buttonHeight = 40;
    int16_t inputHeight = 36;
    int16_t iconSize = 24;
    int16_t borderRadius = 4;
    int16_t borderWidth = 1;
};

// Thème complet
class Theme {
public:
    Theme() = default;
    
    // Composants du thème
    ColorScheme colors;
    Typography typography;
    Spacing spacing;
    
    // Préréglages
    static Theme darkTheme();
    static Theme lightTheme();
    static Theme highContrastTheme();
    
    // Utilitaires
    uint16_t getStateColor(bool enabled, bool pressed = false) const {
        if (!enabled) return colors.disabled;
        if (pressed) return colors.primaryDark;
        return colors.primary;
    }
};

} // namespace UI
```

**Fichier**: `src/core/ui/theme/Theme.cpp`

```cpp
#include "Theme.hpp"

namespace UI {

Theme Theme::darkTheme() {
    Theme theme;
    // Configuration par défaut (déjà sombre)
    return theme;
}

Theme Theme::lightTheme() {
    Theme theme;
    theme.colors.background = 0xFFFF;     // Blanc
    theme.colors.surface = 0xF7BE;        // Gris très clair
    theme.colors.onBackground = 0x0000;   // Noir
    theme.colors.onSurface = 0x0000;      // Noir
    theme.colors.divider = 0xC618;        // Gris clair
    return theme;
}

Theme Theme::highContrastTheme() {
    Theme theme;
    theme.colors.primary = 0xFFE0;        // Jaune
    theme.colors.secondary = 0x07FF;      // Cyan
    theme.colors.background = 0x0000;     // Noir pur
    theme.colors.surface = 0x0000;        // Noir pur
    theme.colors.onBackground = 0xFFFF;   // Blanc pur
    theme.colors.divider = 0xFFFF;        // Blanc
    theme.spacing.borderWidth = 2;        // Bordures plus épaisses
    return theme;
}

} // namespace UI
```

### Phase 5 : Widgets composites spécifiques MIDI

#### 5.1 MidiControlKnob

**Fichier**: `src/adapters/primary/ui/view_components/MidiControlKnob.hpp`

```cpp
#pragma once
#include "core/ui/widgets/Widget.hpp"
#include <string>

namespace UI {

class MidiControlKnob : public Widget {
public:
    MidiControlKnob();
    
    // Configuration MIDI
    void setMidiCC(uint8_t cc) { midiCC_ = cc; }
    void setMidiChannel(uint8_t channel) { midiChannel_ = channel; }
    
    // Valeur (0-127)
    void setValue(uint8_t value);
    uint8_t getValue() const { return value_; }
    
    // Labels
    void setName(const char* name) { name_ = name; invalidate(); }
    void setShowValue(bool show) { showValue_ = show; invalidate(); }
    void setShowCC(bool show) { showCC_ = show; invalidate(); }
    
    // Mode d'apprentissage
    void setLearningMode(bool learning) { 
        learningMode_ = learning; 
        invalidate(); 
    }
    
    // Callbacks
    using ValueChangeCallback = std::function<void(uint8_t)>;
    void setOnValueChange(ValueChangeCallback callback) { 
        onValueChange_ = callback; 
    }
    
    // Override
    void draw(DisplayPort* display) override;
    bool handleInput(const InputEvent& event) override;
    void update(uint32_t deltaMs) override;
    
private:
    // État MIDI
    uint8_t value_ = 0;
    uint8_t midiCC_ = 0;
    uint8_t midiChannel_ = 0;
    
    // Affichage
    std::string name_;
    bool showValue_ = true;
    bool showCC_ = false;
    bool learningMode_ = false;
    
    // Animation
    float displayAngle_ = 0.0f;
    float targetAngle_ = 0.0f;
    
    // Interaction
    bool isDragging_ = false;
    int16_t dragStartY_ = 0;
    uint8_t dragStartValue_ = 0;
    
    // Callback
    ValueChangeCallback onValueChange_;
    
    // Constantes de rendu
    static constexpr float MIN_ANGLE = -135.0f; // degrés
    static constexpr float MAX_ANGLE = 135.0f;
    static constexpr float ANGLE_RANGE = MAX_ANGLE - MIN_ANGLE;
    static constexpr int16_t INDICATOR_LENGTH = 20;
    static constexpr int16_t TICK_LENGTH = 5;
};

} // namespace UI
```

#### 5.2 ParameterEditor

**Fichier**: `src/adapters/primary/ui/view_components/ParameterEditor.hpp`

```cpp
#pragma once
#include "core/ui/widgets/Widget.hpp"
#include "core/ui/widgets/Label.hpp"
#include "core/ui/widgets/ProgressBar.hpp"
#include <memory>

namespace UI {

// Éditeur de paramètre avec affichage de la valeur et barre de progression
class ParameterEditor : public Widget {
public:
    struct Parameter {
        std::string name;
        int32_t minValue;
        int32_t maxValue;
        int32_t defaultValue;
        int32_t step;
        std::string unit;
        bool isBipolar; // -64 à +63 par exemple
    };
    
    ParameterEditor();
    
    // Configuration
    void setParameter(const Parameter& param);
    void setValue(int32_t value);
    int32_t getValue() const { return currentValue_; }
    
    // Mode d'édition
    void setEditMode(bool editing) { 
        editMode_ = editing; 
        invalidate(); 
    }
    bool isInEditMode() const { return editMode_; }
    
    // Affichage
    void setCompactMode(bool compact) { 
        compactMode_ = compact; 
        onBoundsChanged(); 
    }
    
    // Callbacks
    using ValueChangeCallback = std::function<void(int32_t)>;
    void setOnValueChange(ValueChangeCallback callback) { 
        onValueChange_ = callback; 
    }
    
    // Override
    void init(std::shared_ptr<Theme> theme) override;
    void draw(DisplayPort* display) override;
    bool handleInput(const InputEvent& event) override;
    void update(uint32_t deltaMs) override;
    
protected:
    void onBoundsChanged() override;
    
private:
    void updateLayout();
    std::string formatValue() const;
    
    // Paramètre
    Parameter parameter_;
    int32_t currentValue_ = 0;
    
    // État
    bool editMode_ = false;
    bool compactMode_ = false;
    
    // Widgets enfants
    std::shared_ptr<Label> nameLabel_;
    std::shared_ptr<Label> valueLabel_;
    std::shared_ptr<ProgressBar> progressBar_;
    
    // Animation
    int32_t displayValue_ = 0;
    
    // Callback
    ValueChangeCallback onValueChange_;
};

} // namespace UI
```

### Phase 6 : Adaptation des vues existantes

#### 6.1 Refactoring de MenuView

**Fichier**: `src/adapters/primary/ui/MenuView.cpp` (extrait)

```cpp
#include "MenuView.hpp"
#include "core/ui/widgets/Label.hpp"
#include "core/ui/widgets/Button.hpp"
#include "core/ui/layouts/GridLayout.hpp"

void MenuView::init() {
    // Créer le layout principal
    layout_ = std::make_unique<UI::GridLayout>(1, 5); // 1 colonne, 5 lignes
    layout_->setPadding(theme_->spacing.medium);
    layout_->setSpacing(theme_->spacing.small);
    
    // Titre
    titleLabel_ = std::make_shared<UI::Label>("MIDI Controller");
    titleLabel_->setFontSize(DisplayPort::FontSize::TITLE);
    titleLabel_->setTextAlign(DisplayPort::TextAlign::CENTER);
    titleLabel_->setTextColor(theme_->colors.primary);
    layout_->addWidget(titleLabel_, 0, 0);
    
    // Boutons de menu
    const char* menuItems[] = {
        "Controllers",
        "MIDI Settings",
        "System Config",
        "About"
    };
    
    for (int i = 0; i < 4; i++) {
        auto button = std::make_shared<UI::Button>(menuItems[i]);
        button->setBackgroundColor(UI::Button::State::NORMAL, theme_->colors.surface);
        button->setBackgroundColor(UI::Button::State::HOVERED, theme_->colors.primary);
        button->setOnClick([this, i]() { 
            handleMenuSelection(i); 
        });
        menuButtons_.push_back(button);
        layout_->addWidget(button, 0, i + 1);
    }
    
    // Effectuer le layout initial
    layout_->performLayout({0, 0, 240, 320});
}

void MenuView::draw(DisplayPort* display) {
    // Effacer l'écran
    display->clear(theme_->colors.background);
    
    // Dessiner tous les widgets
    for (auto& widget : getAllWidgets()) {
        if (widget->isVisible()) {
            widget->draw(display);
        }
    }
    
    // Dessiner le focus si nécessaire
    if (focusedIndex_ >= 0 && focusedIndex_ < menuButtons_.size()) {
        auto& button = menuButtons_[focusedIndex_];
        auto bounds = button->getBounds();
        display->drawRect(bounds.x - 2, bounds.y - 2, 
                         bounds.width + 4, bounds.height + 4,
                         theme_->colors.primary);
    }
}
```

### Phase 7 : Intégration et optimisation

#### 7.1 Mise à jour de DependencyContainer

**Fichier**: `src/app/di/DependencyContainer.hpp` (ajouts)

```cpp
// Ajouter les nouvelles dépendances UI
class TextRenderer;
class UIFramework;

template<>
class DependencyContainer {
public:
    // ... code existant ...
    
    // Enregistrer les composants UI
    void registerUIFramework() {
        auto textRenderer = std::make_shared<TextRenderer>();
        registerDependency<TextRenderer>(textRenderer);
        
        auto uiFramework = std::make_shared<UIFramework>(textRenderer);
        registerDependency<UIFramework>(uiFramework);
    }
};
```

#### 7.2 Configuration dans InitializationScript

**Fichier**: `src/app/InitializationScript.hpp` (modifications)

```cpp
void initializeContainer(std::shared_ptr<DependencyContainer> container) {
    // ... initialisation existante ...
    
    // Initialiser le framework UI
    container->registerUIFramework();
    
    // Créer le thème par défaut
    auto theme = std::make_shared<UI::Theme>(UI::Theme::darkTheme());
    container->registerDependency<UI::Theme>(theme);
    
    // ... reste de l'initialisation ...
}
```

## Résumé des améliorations

### Fonctionnalités ajoutées
1. **Rendu de texte fonctionnel** avec support complet des fontes TGX
2. **Framework de widgets** modulaire et extensible
3. **Système de layout** flexible (Grid, Flex, Absolute)
4. **Thèmes personnalisables** avec support dark/light/high-contrast
5. **Widgets spécialisés MIDI** (knobs, éditeurs de paramètres)
6. **Optimisations de performance** (caching, dirty rectangles)

### Bénéfices
- **Maintenabilité** : Code modulaire et réutilisable
- **Extensibilité** : Facile d'ajouter de nouveaux widgets
- **Performance** : Rendu optimisé avec invalidation sélective
- **Accessibilité** : Support de thèmes à haut contraste
- **Professionnalisme** : Interface moderne et cohérente

### Prochaines étapes possibles
1. Ajouter des animations fluides (transitions, ease-in/out)
2. Implémenter un système de gestion des ressources (images, icônes)
3. Créer un éditeur visuel de layouts
4. Ajouter le support du multi-touch
5. Implémenter un système de skins/thèmes personnalisés par l'utilisateur