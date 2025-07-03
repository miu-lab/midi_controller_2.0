#include "ButtonIndicator.hpp"
#include "adapters/ui/lvgl/theme/UITheme.hpp"

//=============================================================================
// Constructeur et destructeur
//=============================================================================

ButtonIndicator::ButtonIndicator(lv_obj_t* parent, const UITheme& theme, lv_coord_t size)
    : parent_(parent ? parent : lv_screen_active()),
      theme_(&theme),
      size_(size),
      current_state_(State::OFF),
      led_(nullptr) {
    
    createLvglObjects();
    updateVisualState(false);
}

ButtonIndicator::ButtonIndicator(lv_obj_t* parent, lv_coord_t size)
    : parent_(parent ? parent : lv_screen_active()),
      theme_(nullptr),
      size_(size),
      current_state_(State::OFF),
      led_(nullptr) {
    
    createLvglObjects();
    setupLegacyStyles();
    updateVisualState(false);
}

ButtonIndicator::~ButtonIndicator() {
    // LVGL supprime automatiquement les enfants quand on supprime le parent
    if (led_) {
        lv_obj_delete(led_);
        led_ = nullptr;
    }
}

//=============================================================================
// Création objets LVGL
//=============================================================================

void ButtonIndicator::createLvglObjects() {
    // Créer la LED native LVGL
    led_ = lv_led_create(parent_);
    
    if (!led_) {
        return;
    }
    
    lv_obj_set_size(led_, size_, size_);
    lv_obj_set_user_data(led_, this);
    
    // S'assurer que la LED est visible
    lv_obj_clear_flag(led_, LV_OBJ_FLAG_HIDDEN);
    
    // Style de base - forme ronde
    lv_obj_set_style_radius(led_, LV_RADIUS_CIRCLE, LV_PART_MAIN);
}

void ButtonIndicator::setupLegacyStyles() {
    // Styles hardcodés pour le constructeur legacy
    if (!led_) return;
    
    // Style de base - LED ronde
    lv_obj_set_style_radius(led_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(led_, 1, 0);
    lv_obj_set_style_border_color(led_, lv_color_hex(0x333333), 0);
    
    // Appliquer l'état initial
    updateVisualState(false);
}

//=============================================================================
// Interface publique
//=============================================================================

void ButtonIndicator::setState(State state, bool animate) {
    if (current_state_ != state) {
        current_state_ = state;
        updateVisualState(animate);
    }
}

void ButtonIndicator::setVisible(bool visible) {
    if (led_) {
        if (visible) {
            lv_obj_clear_flag(led_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(led_, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

bool ButtonIndicator::isVisible() const {
    if (led_) {
        return !lv_obj_has_flag(led_, LV_OBJ_FLAG_HIDDEN);
    }
    return false;
}

void ButtonIndicator::setPosition(lv_coord_t x, lv_coord_t y) {
    if (led_) {
        lv_obj_set_pos(led_, x, y);
    }
}

void ButtonIndicator::setSize(lv_coord_t size) {
    if (led_) {
        size_ = size;
        lv_obj_set_size(led_, size, size);
    }
}

void ButtonIndicator::center() {
    if (led_) {
        lv_obj_center(led_);
    }
}

void ButtonIndicator::alignTo(lv_obj_t* align_to, lv_align_t align, lv_coord_t x_offset, lv_coord_t y_offset) {
    if (led_ && align_to) {
        lv_obj_align_to(led_, align_to, align, x_offset, y_offset);
    }
}

//=============================================================================
// Méthodes statiques utilitaires
//=============================================================================

lv_color_t ButtonIndicator::getDefaultColorForState(State state) {
    switch (state) {
        case State::OFF:
            return lv_color_hex(0x606060);  // Gris plus visible
        case State::PRESSED:
            return lv_color_hex(0x00FF00);  // Vert vif
        case State::ACTIVE:
            return lv_color_hex(0x00AA00);  // Vert moyen
        case State::ERROR:
            return lv_color_hex(0xFF0000);  // Rouge
        default:
            return lv_color_hex(0x606060);
    }
}

lv_opa_t ButtonIndicator::getDefaultOpacityForState(State state) {
    switch (state) {
        case State::OFF:
            return LV_OPA_60;   // 60% opacité - plus visible qu'avant
        case State::PRESSED:
            return LV_OPA_COVER; // 100% opacité - très visible
        case State::ACTIVE:
            return LV_OPA_80;   // 80% opacité - bien visible
        case State::ERROR:
            return LV_OPA_COVER; // 100% opacité - erreur très visible
        default:
            return LV_OPA_60;
    }
}

//=============================================================================
// Méthodes privées
//=============================================================================

void ButtonIndicator::updateVisualState(bool animate) {
    if (!led_) return;
    
    lv_color_t color;
    lv_opa_t opacity;
    
    // Déterminer la couleur et l'opacité selon l'état et le thème
    if (theme_) {
        // Utiliser les couleurs du thème UITheme
        switch (current_state_) {
            case State::OFF:
                color = theme_->colors.midi_inactive;
                opacity = LV_OPA_30;
                break;
            case State::PRESSED:
                color = theme_->colors.success;
                opacity = LV_OPA_COVER;
                break;
            case State::ACTIVE:
                color = theme_->colors.primary;
                opacity = LV_OPA_80;
                break;
            case State::ERROR:
                color = theme_->colors.error;
                opacity = LV_OPA_COVER;
                break;
            default:
                color = theme_->colors.midi_inactive;
                opacity = LV_OPA_30;
                break;
        }
    } else {
        // Utiliser les couleurs par défaut hardcodées
        color = getDefaultColorForState(current_state_);
        opacity = getDefaultOpacityForState(current_state_);
    }
    
    // Appliquer la couleur et l'opacité
    applyColorAndOpacity(color, opacity, animate);
}

void ButtonIndicator::applyColorAndOpacity(lv_color_t color, lv_opa_t opacity, bool animate) {
    if (!led_) return;
    
    // Déterminer si la LED doit être allumée ou éteinte
    bool should_be_on = (current_state_ != State::OFF);
    
    if (should_be_on) {
        lv_led_on(led_);
        // Configurer la luminosité selon l'opacité
        uint8_t brightness = (opacity * 255) / LV_OPA_COVER;
        lv_led_set_brightness(led_, brightness);
    } else {
        lv_led_off(led_);
        lv_led_set_brightness(led_, 0);
    }
    
    // Appliquer la couleur à la LED
    lv_obj_set_style_bg_color(led_, color, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(led_, opacity, LV_PART_MAIN);
    
    // Pour une LED, on peut aussi configurer la couleur du "shadow" pour un effet plus réaliste
    if (should_be_on) {
        lv_obj_set_style_shadow_color(led_, color, 0);
        lv_obj_set_style_shadow_width(led_, 3, 0);
        lv_obj_set_style_shadow_opa(led_, LV_OPA_50, 0);
    } else {
        lv_obj_set_style_shadow_width(led_, 0, 0);
        lv_obj_set_style_shadow_opa(led_, LV_OPA_0, 0);
    }
    
    // Animation optionnelle (simple fade pour l'opacité)
    if (animate && should_be_on) {
        // TODO: Ajouter animation LVGL si nécessaire
        // Pour l'instant, changement immédiat
    }
    
    // Invalider l'objet pour forcer le redraw
    lv_obj_invalidate(led_);
}