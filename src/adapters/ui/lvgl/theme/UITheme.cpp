#include "UITheme.hpp"

//=============================================================================
// Application du thème LVGL
//=============================================================================

void UITheme::applyToLvglDisplay(lv_display_t* display) {
    if (!display) return;
    
    // Créer et appliquer le thème par défaut LVGL avec nos couleurs
    lv_theme_t* theme = lv_theme_default_init(
        display,
        lv_color_hex(0x00FF80),    // Primary (notre vert néon)
        lv_color_hex(0x0080FF),    // Secondary (notre bleu)
        false,                     // Mode sombre
        fonts.medium               // Police normale
    );
    
    lv_display_set_theme(display, theme);
    
    Serial.println(F("UITheme: Thème LVGL appliqué au display"));
}

//=============================================================================
// Création de styles
//=============================================================================

void UITheme::createContainerStyle(lv_style_t* style) {
    if (!style) return;
    
    lv_style_init(style);
    
    // Fond et transparence
    lv_style_set_bg_color(style, colors.surface);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    
    // Bordures
    lv_style_set_border_width(style, dimensions.border_thin);
    lv_style_set_border_color(style, colors.surface_variant);
    lv_style_set_border_opa(style, LV_OPA_50);
    
    // Padding et rayons
    lv_style_set_pad_all(style, spacing.medium);
    lv_style_set_radius(style, dimensions.radius_medium);
    
    Serial.println(F("UITheme: Style container créé"));
}

void UITheme::createButtonStyle(lv_style_t* style) {
    if (!style) return;
    
    lv_style_init(style);
    
    // État normal
    lv_style_set_bg_color(style, colors.primary);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_text_color(style, colors.on_primary);
    lv_style_set_text_font(style, fonts.medium);
    
    // Dimensions
    lv_style_set_height(style, dimensions.button_height);
    lv_style_set_pad_hor(style, spacing.large);
    lv_style_set_pad_ver(style, spacing.small);
    
    // Apparence
    lv_style_set_radius(style, dimensions.radius_medium);
    lv_style_set_border_width(style, 0);
    
    // Ombre
    lv_style_set_shadow_width(style, spacing.tiny);
    lv_style_set_shadow_color(style, colors.background);
    lv_style_set_shadow_opa(style, LV_OPA_30);
    
    Serial.println(F("UITheme: Style bouton créé"));
}

void UITheme::createLabelStyle(lv_style_t* style, const lv_font_t* font) {
    if (!style) return;
    
    lv_style_init(style);
    
    // Police et couleur
    lv_style_set_text_color(style, colors.on_surface);
    lv_style_set_text_font(style, font ? font : fonts.medium);
    
    // Alignement
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    
    // Pas de fond
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_all(style, 0);
    
    Serial.println(F("UITheme: Style label créé"));
}

void UITheme::createArcStyle(lv_style_t* style) {
    if (!style) return;
    
    lv_style_init(style);
    
    // Arc principal (fond)
    lv_style_set_arc_width(style, dimensions.arc_width);
    lv_style_set_arc_color(style, colors.midi_inactive);
    lv_style_set_arc_opa(style, LV_OPA_COVER);
    
    Serial.println(F("UITheme: Style arc créé"));
}

void UITheme::applyParameterWidgetStyle(lv_obj_t* container, lv_obj_t* arc,
                                       lv_obj_t* value_label, lv_obj_t* cc_label,
                                       lv_obj_t* channel_label, lv_obj_t* name_label) const {
    if (!container || !arc) return;
    
    // Style du container
    lv_obj_set_style_bg_color(container, colors.surface, 0);
    lv_obj_set_style_bg_opa(container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_radius(container, dimensions.radius_small, 0);
    lv_obj_set_style_pad_all(container, spacing.tiny, 0);
    
    // Style de l'arc
    lv_obj_set_style_arc_width(arc, dimensions.arc_width, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, colors.midi_inactive, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, dimensions.arc_width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, colors.midi_cc, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    
    // Style des labels
    if (value_label) {
        lv_obj_set_style_text_color(value_label, colors.on_surface, 0);
        lv_obj_set_style_text_font(value_label, fonts.parameter_value, 0);
        lv_obj_set_style_text_align(value_label, LV_TEXT_ALIGN_CENTER, 0);
    }
    
    if (cc_label) {
        lv_obj_set_style_text_color(cc_label, colors.text_secondary, 0);
        lv_obj_set_style_text_font(cc_label, fonts.parameter_label, 0);
    }
    
    if (channel_label) {
        lv_obj_set_style_text_color(channel_label, colors.midi_channel, 0);
        lv_obj_set_style_text_font(channel_label, fonts.parameter_label, 0);
    }
    
    if (name_label) {
        lv_obj_set_style_text_color(name_label, colors.on_surface, 0);
        lv_obj_set_style_text_font(name_label, fonts.parameter_label, 0);
        lv_obj_set_style_text_align(name_label, LV_TEXT_ALIGN_CENTER, 0);
    }
    
    Serial.println(F("UITheme: Styles ParameterWidget appliqués"));
}

//=============================================================================
// Utilitaires couleur et position
//=============================================================================

lv_color_t UITheme::getColorWithOpacity(lv_color_t color, lv_opa_t opacity) {
    // LVGL gère l'opacité séparément, on retourne la couleur telle quelle
    // L'opacité sera appliquée via lv_style_set_*_opa()
    return color;
}

lv_coord_t UITheme::getRelativePosition(float percent, bool is_width) {
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;
    
    if (is_width) {
        return (lv_coord_t)(screen.width * percent);
    } else {
        return (lv_coord_t)(screen.height * percent);
    }
}

lv_coord_t UITheme::getRelativeSize(float percent, bool is_width) {
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;
    
    if (is_width) {
        return (lv_coord_t)(screen.content_width * percent);
    } else {
        return (lv_coord_t)(screen.content_height * percent);
    }
}

//=============================================================================
// Factory Methods - Thèmes prédéfinis
//=============================================================================

UITheme UITheme::createDefaultTheme() {
    UITheme theme;
    
    // Configuration par défaut déjà dans les structures
    Serial.println(F("UITheme: Thème par défaut créé"));
    return theme;
}

UITheme UITheme::createDarkTheme() {
    UITheme theme;
    
    // Couleurs encore plus sombres
    theme.colors.background = lv_color_hex(0x000000);
    theme.colors.surface = lv_color_hex(0x121212);
    theme.colors.surface_variant = lv_color_hex(0x1E1E1E);
    
    // Texte légèrement moins contrasté
    theme.colors.on_background = lv_color_hex(0xE0E0E0);
    theme.colors.on_surface = lv_color_hex(0xE0E0E0);
    theme.colors.text_secondary = lv_color_hex(0xA0A0A0);
    
    Serial.println(F("UITheme: Thème sombre créé"));
    return theme;
}

UITheme UITheme::createLightTheme() {
    UITheme theme;
    
    // Couleurs claires (pour test)
    theme.colors.background = lv_color_hex(0xFFFFFF);
    theme.colors.surface = lv_color_hex(0xF5F5F5);
    theme.colors.surface_variant = lv_color_hex(0xE0E0E0);
    
    // Texte sombre sur fond clair
    theme.colors.on_background = lv_color_hex(0x000000);
    theme.colors.on_surface = lv_color_hex(0x000000);
    theme.colors.text_secondary = lv_color_hex(0x666666);
    
    // Ajuster les couleurs primaires pour contraste
    theme.colors.primary = lv_color_hex(0x00AA55);
    theme.colors.secondary = lv_color_hex(0x0066CC);
    
    Serial.println(F("UITheme: Thème clair créé"));
    return theme;
}