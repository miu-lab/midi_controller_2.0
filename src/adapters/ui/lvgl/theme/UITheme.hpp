#pragma once

#include <lvgl.h>
#include <Arduino.h>

/**
 * @brief Système de thème UI centralisé pour le contrôleur MIDI
 * 
 * Cette classe centralise toute la configuration visuelle de l'interface :
 * couleurs, polices, espacements, dimensions, animations, etc.
 * 
 * Usage:
 * - Injection via DependencyContainer
 * - Accessible dans tous les widgets et vues
 * - Configuration via thèmes prédéfinis ou personnalisés
 */
class UITheme {
public:
    //=========================================================================
    // COULEURS
    //=========================================================================
    
    struct Colors {
        // Couleurs primaires
        lv_color_t primary = lv_color_hex(0x00FF80);        ///< Vert néon principal
        lv_color_t primary_variant = lv_color_hex(0x00AA55); ///< Vert plus sombre
        lv_color_t secondary = lv_color_hex(0x0080FF);       ///< Bleu accent
        lv_color_t secondary_variant = lv_color_hex(0x0055AA); ///< Bleu plus sombre
        
        // Couleurs de fond
        lv_color_t background = lv_color_hex(0x000000);      ///< Noir pur
        lv_color_t surface = lv_color_hex(0x1E1E1E);         ///< Gris très foncé
        lv_color_t surface_variant = lv_color_hex(0x2A2A2A); ///< Gris foncé
        
        // Couleurs de texte
        lv_color_t on_background = lv_color_hex(0xFFFFFF);   ///< Blanc sur fond
        lv_color_t on_surface = lv_color_hex(0xFFFFFF);      ///< Blanc sur surface
        lv_color_t on_primary = lv_color_hex(0x000000);      ///< Noir sur primaire
        lv_color_t text_secondary = lv_color_hex(0xBBBBBB);  ///< Gris clair
        lv_color_t text_disabled = lv_color_hex(0x666666);   ///< Gris moyen
        
        // Couleurs d'état
        lv_color_t success = lv_color_hex(0x00FF00);         ///< Vert succès
        lv_color_t warning = lv_color_hex(0xFFAA00);         ///< Orange avertissement
        lv_color_t error = lv_color_hex(0xFF0000);           ///< Rouge erreur
        lv_color_t info = lv_color_hex(0x00AAFF);            ///< Bleu information
        
        // Couleurs MIDI spécifiques
        lv_color_t midi_cc = lv_color_hex(0x00FF80);         ///< Vert pour CC
        lv_color_t midi_note = lv_color_hex(0xFF8000);       ///< Orange pour notes
        lv_color_t midi_channel = lv_color_hex(0x8080FF);    ///< Bleu pour canaux
        lv_color_t midi_inactive = lv_color_hex(0x404040);   ///< Gris pour inactif
        
        // Transparences
        lv_opa_t overlay_light = LV_OPA_20;                  ///< 20% opacité
        lv_opa_t overlay_medium = LV_OPA_50;                 ///< 50% opacité
        lv_opa_t overlay_heavy = LV_OPA_80;                  ///< 80% opacité
    };
    
    //=========================================================================
    // POLICES
    //=========================================================================
    
    struct Fonts {
        const lv_font_t* tiny = &lv_font_montserrat_10;      ///< 10px - texte très petit
        const lv_font_t* small = &lv_font_montserrat_12;     ///< 12px - texte petit
        const lv_font_t* medium = &lv_font_montserrat_14;    ///< 14px - texte normal
        const lv_font_t* large = &lv_font_montserrat_16;     ///< 16px - texte grand
        const lv_font_t* xlarge = &lv_font_montserrat_18;    ///< 18px - titre
        const lv_font_t* xxlarge = &lv_font_montserrat_20;   ///< 20px - gros titre
        
        // Polices spécialisées
        const lv_font_t* parameter_value = &lv_font_montserrat_16; ///< Valeurs paramètres
        const lv_font_t* parameter_label = &lv_font_montserrat_12; ///< Labels paramètres
        const lv_font_t* menu_item = &lv_font_montserrat_14;      ///< Items de menu
        const lv_font_t* status_bar = &lv_font_montserrat_10;     ///< Barre de status
    };
    
    //=========================================================================
    // ESPACEMENTS ET DIMENSIONS
    //=========================================================================
    
    struct Spacing {
        // Padding
        lv_coord_t none = 0;                                 ///< Aucun espacement
        lv_coord_t tiny = 2;                                 ///< 2px
        lv_coord_t small = 4;                                ///< 4px
        lv_coord_t medium = 8;                               ///< 8px (base)
        lv_coord_t large = 12;                               ///< 12px
        lv_coord_t xlarge = 16;                              ///< 16px
        lv_coord_t xxlarge = 24;                             ///< 24px
        
        // Marges spécifiques
        lv_coord_t widget_margin = 8;                       ///< Marge entre widgets
        lv_coord_t container_padding = 12;                  ///< Padding containers
        lv_coord_t screen_margin = 16;                      ///< Marge écran
    };
    
    struct Dimensions {
        // Éléments de base
        lv_coord_t button_height = 40;                      ///< Hauteur boutons
        lv_coord_t input_height = 36;                       ///< Hauteur inputs
        lv_coord_t header_height = 48;                      ///< Hauteur header
        lv_coord_t footer_height = 32;                      ///< Hauteur footer
        
        // Éléments MIDI
        lv_coord_t parameter_widget_width = 200;            ///< Largeur widget param
        lv_coord_t parameter_widget_height = 160;           ///< Hauteur widget param
        lv_coord_t parameter_arc_size = 100;                ///< Taille arc param
        lv_coord_t menu_item_height = 44;                   ///< Hauteur item menu
        
        // Bordures et épaisseurs
        lv_coord_t border_thin = 1;                         ///< Bordure fine
        lv_coord_t border_medium = 2;                       ///< Bordure normale
        lv_coord_t border_thick = 4;                        ///< Bordure épaisse
        lv_coord_t arc_width = 8;                           ///< Épaisseur arcs
        lv_coord_t separator_width = 1;                     ///< Épaisseur séparateurs
        
        // Rayons de courbure
        lv_coord_t radius_none = 0;                         ///< Pas de rayon
        lv_coord_t radius_small = 4;                        ///< Petit rayon
        lv_coord_t radius_medium = 8;                       ///< Rayon normal
        lv_coord_t radius_large = 12;                       ///< Grand rayon
        lv_coord_t radius_circle = LV_RADIUS_CIRCLE;        ///< Cercle complet
    };
    
    //=========================================================================
    // ANIMATIONS
    //=========================================================================
    
    struct Animations {
        // Durées (en millisecondes)
        uint32_t instant = 0;                               ///< Pas d'animation
        uint32_t fast = 150;                                ///< Animation rapide
        uint32_t normal = 300;                              ///< Animation normale
        uint32_t slow = 500;                                ///< Animation lente
        uint32_t very_slow = 1000;                          ///< Animation très lente
        
        // Durées spécifiques
        uint32_t parameter_change = 200;                    ///< Changement paramètre
        uint32_t menu_transition = 250;                     ///< Transition menu
        uint32_t popup_appear = 300;                        ///< Apparition popup
        uint32_t screen_transition = 400;                   ///< Transition écran
        
        // Courbes d'animation (LVGL anim path)
        lv_anim_path_cb_t ease_in = lv_anim_path_ease_in;    ///< Accélération
        lv_anim_path_cb_t ease_out = lv_anim_path_ease_out;  ///< Décélération
        lv_anim_path_cb_t ease_in_out = lv_anim_path_ease_in_out; ///< Les deux
        lv_anim_path_cb_t linear = lv_anim_path_linear;      ///< Linéaire
        lv_anim_path_cb_t bounce = lv_anim_path_bounce;      ///< Rebond
    };
    
    //=========================================================================
    // CONFIGURATION ÉCRAN
    //=========================================================================
    
    struct Screen {
        // Dimensions ILI9341
        lv_coord_t width = 320;                             ///< Largeur écran
        lv_coord_t height = 240;                            ///< Hauteur écran
        
        // Zones utiles
        lv_coord_t content_width = 280;                     ///< Largeur contenu
        lv_coord_t content_height = 200;                    ///< Hauteur contenu
        lv_coord_t safe_margin = 20;                        ///< Marge sécurité
        
        // Grille de layout
        lv_coord_t grid_cols = 4;                           ///< Colonnes grille
        lv_coord_t grid_rows = 3;                           ///< Lignes grille
        lv_coord_t grid_gap = 8;                            ///< Espacement grille
    };
    
    //=========================================================================
    // MEMBRES PUBLICS
    //=========================================================================
    
    Colors colors;
    Fonts fonts;
    Spacing spacing;
    Dimensions dimensions;
    Animations animations;
    Screen screen;
    
    //=========================================================================
    // INTERFACE PUBLIQUE
    //=========================================================================
    
    /**
     * @brief Constructeur avec thème par défaut
     */
    UITheme() = default;
    
    /**
     * @brief Destructeur
     */
    ~UITheme() = default;
    
    /**
     * @brief Applique le thème LVGL global
     * @param display Display LVGL cible
     */
    void applyToLvglDisplay(lv_display_t* display);
    
    /**
     * @brief Crée un style de base pour les containers
     * @param style Style à configurer
     */
    void createContainerStyle(lv_style_t* style);
    
    /**
     * @brief Crée un style pour les boutons
     * @param style Style à configurer
     */
    void createButtonStyle(lv_style_t* style);
    
    /**
     * @brief Crée un style pour les labels
     * @param style Style à configurer
     * @param font_size Taille de police (tiny, small, medium, etc.)
     */
    void createLabelStyle(lv_style_t* style, const lv_font_t* font = nullptr);
    
    /**
     * @brief Crée un style pour les arcs (paramètres)
     * @param style Style à configurer
     */
    void createArcStyle(lv_style_t* style);
    
    /**
     * @brief Applique le style complet à un ParameterWidget
     * @param container Container du widget
     * @param arc Arc du widget
     * @param cc_label Label CC
     * @param channel_label Label canal
     * @param name_label Label nom
     */
    void applyParameterWidgetStyle(lv_obj_t* container, lv_obj_t* arc,
                                   lv_obj_t* cc_label,
                                   lv_obj_t* channel_label, lv_obj_t* name_label) const;
    
    /**
     * @brief Obtient une couleur avec opacité modifiée
     * @param color Couleur de base
     * @param opacity Opacité (0-255)
     * @return Couleur avec opacité
     */
    lv_color_t getColorWithOpacity(lv_color_t color, lv_opa_t opacity);
    
    /**
     * @brief Calcule une position relative à l'écran
     * @param percent Pourcentage (0.0 - 1.0)
     * @param is_width true pour largeur, false pour hauteur
     * @return Position en pixels
     */
    lv_coord_t getRelativePosition(float percent, bool is_width = true);
    
    /**
     * @brief Calcule une taille relative à l'écran
     * @param percent Pourcentage (0.0 - 1.0)
     * @param is_width true pour largeur, false pour hauteur
     * @return Taille en pixels
     */
    lv_coord_t getRelativeSize(float percent, bool is_width = true);
    
    //=========================================================================
    // HELPERS STATIQUES
    //=========================================================================
    
    /**
     * @brief Crée un thème par défaut optimisé pour contrôleur MIDI
     * @return Instance UITheme configurée
     */
    static UITheme createDefaultTheme();
    
    /**
     * @brief Crée un thème sombre
     * @return Instance UITheme configurée
     */
    static UITheme createDarkTheme();
    
    /**
     * @brief Crée un thème clair
     * @return Instance UITheme configurée
     */
    static UITheme createLightTheme();
};