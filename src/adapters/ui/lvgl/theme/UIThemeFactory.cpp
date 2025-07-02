#include "UIThemeFactory.hpp"


//=============================================================================
// Interface publique
//=============================================================================

UIThemeFactory::ThemeConfig UIThemeFactory::createTheme(ThemeType type) {
    switch (type) {
        case ThemeType::DEFAULT:
            return createDefaultTheme();
        case ThemeType::DARK:
            return createDarkTheme();
        case ThemeType::LIGHT:
            return createLightTheme();
        case ThemeType::MIDI_CONTROLLER:
            return createMidiControllerTheme();
        case ThemeType::PERFORMANCE:
            return createPerformanceTheme();
        case ThemeType::STUDIO:
            return createStudioTheme();
        default:
            // DEBUG MSG TO IMPLEMENT
            return createDefaultTheme();
    }
}

UIThemeFactory::ThemeConfig UIThemeFactory::createMidiControllerTheme() {
    auto theme = std::make_shared<UITheme>(UITheme::createDefaultTheme());
    
    // Optimisations spécifiques contrôleur MIDI
    theme->colors.primary = lv_color_hex(0x00FF80);        // Vert néon signature
    theme->colors.secondary = lv_color_hex(0xFF8000);      // Orange pour contraste
    theme->colors.midi_cc = lv_color_hex(0x00FF80);        // Vert pour CC
    theme->colors.midi_note = lv_color_hex(0xFF8000);      // Orange pour notes
    theme->colors.midi_channel = lv_color_hex(0x8080FF);   // Bleu pour canaux
    
    // Dimensions optimisées pour contrôleur MIDI
    theme->dimensions.parameter_widget_width = 200;
    theme->dimensions.parameter_widget_height = 160;
    theme->dimensions.parameter_arc_size = 120;
    theme->dimensions.arc_width = 10;
    
    // Espacements pour interface dense
    theme->spacing.widget_margin = 6;
    theme->spacing.container_padding = 8;
    
    auto layout = createLayoutForTheme(theme);
    applyThemeOptimizations(*theme, ThemeType::MIDI_CONTROLLER);
    
    // DEBUG MSG TO IMPLEMENT
    
    return {
        .theme = theme,
        .layout = layout,
        .name = "MIDI Controller",
        .description = "Thème optimisé pour contrôleur MIDI avec vert néon signature"
    };
}

UIThemeFactory::ThemeConfig UIThemeFactory::createPerformanceTheme() {
    auto theme = std::make_shared<UITheme>(UITheme::createDarkTheme());
    
    // Couleurs haute visibilité pour scène
    theme->colors.primary = lv_color_hex(0x00FFFF);        // Cyan brillant
    theme->colors.secondary = lv_color_hex(0xFF00FF);      // Magenta
    theme->colors.background = lv_color_hex(0x000000);     // Noir pur
    theme->colors.surface = lv_color_hex(0x0A0A0A);        // Très sombre
    
    // Éléments plus gros pour performance live
    theme->dimensions.parameter_widget_width = 240;
    theme->dimensions.parameter_widget_height = 180;
    theme->dimensions.parameter_arc_size = 140;
    theme->dimensions.arc_width = 12;
    theme->dimensions.button_height = 50;
    
    // Texte plus grand
    theme->fonts.parameter_value = &lv_font_montserrat_18;
    theme->fonts.parameter_label = &lv_font_montserrat_14;
    
    // Animations plus rapides
    theme->animations.parameter_change = 100;
    theme->animations.menu_transition = 150;
    
    auto layout = createLayoutForTheme(theme);
    applyThemeOptimizations(*theme, ThemeType::PERFORMANCE);
    
    // DEBUG MSG TO IMPLEMENT
    
    return {
        .theme = theme,
        .layout = layout,
        .name = "Performance",
        .description = "Thème haute visibilité pour performances live"
    };
}

UIThemeFactory::ThemeConfig UIThemeFactory::createStudioTheme() {
    auto theme = std::make_shared<UITheme>(UITheme::createLightTheme());
    
    // Couleurs douces pour studio
    theme->colors.primary = lv_color_hex(0x2E7D32);        // Vert professionnel
    theme->colors.secondary = lv_color_hex(0x1976D2);      // Bleu professionnel
    theme->colors.background = lv_color_hex(0xF5F5F5);     // Gris très clair
    theme->colors.surface = lv_color_hex(0xFFFFFF);        // Blanc pur
    
    // Dimensions plus compactes pour studio
    theme->dimensions.parameter_widget_width = 180;
    theme->dimensions.parameter_widget_height = 140;
    theme->dimensions.parameter_arc_size = 100;
    theme->dimensions.arc_width = 6;
    
    // Espacements réduits pour plus d'éléments
    theme->spacing.widget_margin = 4;
    theme->spacing.container_padding = 6;
    theme->spacing.screen_margin = 12;
    
    // Animations subtiles
    theme->animations.parameter_change = 300;
    theme->animations.menu_transition = 400;
    
    auto layout = createLayoutForTheme(theme);
    applyThemeOptimizations(*theme, ThemeType::STUDIO);
    
    // DEBUG MSG TO IMPLEMENT
    
    return {
        .theme = theme,
        .layout = layout,
        .name = "Studio",
        .description = "Thème professionnel pour usage studio"
    };
}

void UIThemeFactory::applyThemeToDisplay(const ThemeConfig& config, lv_display_t* display) {
    if (config.theme && display) {
        config.theme->applyToLvglDisplay(display);
        // DEBUG MSG TO IMPLEMENT
    }
}

std::vector<UIThemeFactory::ThemeType> UIThemeFactory::getAvailableThemes() {
    return {
        ThemeType::DEFAULT,
        ThemeType::DARK,
        ThemeType::LIGHT,
        ThemeType::MIDI_CONTROLLER,
        ThemeType::PERFORMANCE,
        ThemeType::STUDIO
    };
}

String UIThemeFactory::getThemeName(ThemeType type) {
    switch (type) {
        case ThemeType::DEFAULT: return "Default";
        case ThemeType::DARK: return "Dark";
        case ThemeType::LIGHT: return "Light";
        case ThemeType::MIDI_CONTROLLER: return "MIDI Controller";
        case ThemeType::PERFORMANCE: return "Performance";
        case ThemeType::STUDIO: return "Studio";
        default: return "Unknown";
    }
}

String UIThemeFactory::getThemeDescription(ThemeType type) {
    switch (type) {
        case ThemeType::DEFAULT: 
            return "Thème par défaut avec vert néon";
        case ThemeType::DARK: 
            return "Thème sombre pour utilisation nocturne";
        case ThemeType::LIGHT: 
            return "Thème clair pour environnement lumineux";
        case ThemeType::MIDI_CONTROLLER: 
            return "Optimisé pour contrôleur MIDI";
        case ThemeType::PERFORMANCE: 
            return "Haute visibilité pour performances live";
        case ThemeType::STUDIO: 
            return "Professionnel pour usage studio";
        default: 
            return "Thème inconnu";
    }
}

//=============================================================================
// Méthodes privées de création
//=============================================================================

UIThemeFactory::ThemeConfig UIThemeFactory::createDefaultTheme() {
    auto theme = std::make_shared<UITheme>(UITheme::createDefaultTheme());
    auto layout = createLayoutForTheme(theme);
    
    // DEBUG MSG TO IMPLEMENT
    
    return {
        .theme = theme,
        .layout = layout,
        .name = "Default",
        .description = "Thème par défaut avec vert néon"
    };
}

UIThemeFactory::ThemeConfig UIThemeFactory::createDarkTheme() {
    auto theme = std::make_shared<UITheme>(UITheme::createDarkTheme());
    auto layout = createLayoutForTheme(theme);
    
    // DEBUG MSG TO IMPLEMENT
    
    return {
        .theme = theme,
        .layout = layout,
        .name = "Dark",
        .description = "Thème sombre pour utilisation nocturne"
    };
}

UIThemeFactory::ThemeConfig UIThemeFactory::createLightTheme() {
    auto theme = std::make_shared<UITheme>(UITheme::createLightTheme());
    auto layout = createLayoutForTheme(theme);
    
    // DEBUG MSG TO IMPLEMENT
    
    return {
        .theme = theme,
        .layout = layout,
        .name = "Light",
        .description = "Thème clair pour environnement lumineux"
    };
}

std::shared_ptr<UILayout> UIThemeFactory::createLayoutForTheme(std::shared_ptr<UITheme> theme) {
    if (!theme) {
        // DEBUG MSG TO IMPLEMENT
        return nullptr;
    }
    
    auto layout = std::make_shared<UILayout>(*theme);
    
    // Le layout sera configuré par les vues selon leurs besoins spécifiques
    // avec setupGrid(), setupFlex(), etc.
    
    // DEBUG MSG TO IMPLEMENT
    return layout;
}

void UIThemeFactory::applyThemeOptimizations(UITheme& theme, ThemeType type) {
    switch (type) {
        case ThemeType::MIDI_CONTROLLER:
            // Optimisations pour contrôleur MIDI
            theme.animations.parameter_change = 200;  // Réponse rapide
            theme.dimensions.arc_width = 10;          // Arc bien visible
            break;
            
        case ThemeType::PERFORMANCE:
            // Optimisations pour performance live
            theme.animations.parameter_change = 100;  // Très rapide
            theme.dimensions.arc_width = 12;          // Maximum de visibilité
            theme.colors.overlay_heavy = LV_OPA_90;   // Contrastes élevés
            break;
            
        case ThemeType::STUDIO:
            // Optimisations pour studio
            theme.animations.parameter_change = 300;  // Plus doux
            theme.dimensions.arc_width = 6;           // Plus subtil
            theme.colors.overlay_light = LV_OPA_10;   // Contrastes réduits
            break;
            
        default:
            // Pas d'optimisations spéciales
            break;
    }
    
    // DEBUG MSG TO IMPLEMENT
}