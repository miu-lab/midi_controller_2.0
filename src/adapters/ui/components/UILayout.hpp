#pragma once

#include <lvgl.h>
#include <Arduino.h>
#include "UITheme.hpp"

/**
 * @brief Gestionnaire de layout et positionnement UI
 * 
 * Cette classe fournit des utilitaires pour le positionnement, 
 * l'alignement et la distribution des éléments UI de manière cohérente.
 * 
 * Features:
 * - Système de grille flexible
 * - Positionnement relatif et absolu
 * - Distribution automatique d'éléments
 * - Gestion des marges et espacements
 * - Responsive design pour différentes orientations
 */
class UILayout {
public:
    //=========================================================================
    // STRUCTURES DE CONFIGURATION
    //=========================================================================
    
    /**
     * @brief Configuration d'une grille de layout
     */
    struct GridConfig {
        uint8_t columns = 4;                    ///< Nombre de colonnes
        uint8_t rows = 3;                       ///< Nombre de lignes
        lv_coord_t gap_horizontal = 8;          ///< Espacement horizontal
        lv_coord_t gap_vertical = 8;            ///< Espacement vertical
        lv_coord_t margin_left = 16;            ///< Marge gauche
        lv_coord_t margin_right = 16;           ///< Marge droite
        lv_coord_t margin_top = 16;             ///< Marge haute
        lv_coord_t margin_bottom = 16;          ///< Marge basse
    };
    
    /**
     * @brief Position dans une grille
     */
    struct GridPosition {
        uint8_t column = 0;                     ///< Colonne (0-based)
        uint8_t row = 0;                        ///< Ligne (0-based)
        uint8_t column_span = 1;                ///< Nombre de colonnes occupées
        uint8_t row_span = 1;                   ///< Nombre de lignes occupées
    };
    
    /**
     * @brief Zone rectangulaire calculée
     */
    struct Rect {
        lv_coord_t x = 0;                       ///< Position X
        lv_coord_t y = 0;                       ///< Position Y
        lv_coord_t width = 0;                   ///< Largeur
        lv_coord_t height = 0;                  ///< Hauteur
    };
    
    
    //=========================================================================
    // CONSTRUCTEUR
    //=========================================================================
    
    /**
     * @brief Constructeur
     * @param theme Référence vers le thème UI
     */
    explicit UILayout(const UITheme& theme);
    
    //=========================================================================
    // GESTION DE GRILLE NATIVE LVGL
    //=========================================================================
    
    /**
     * @brief Configure un container avec layout grid natif LVGL
     * @param container Container à configurer
     * @param config Configuration de la grille
     */
    void setupGrid(lv_obj_t* container, const GridConfig& config);
    
    /**
     * @brief Place un widget dans une grille native LVGL
     * @param widget Widget à positionner
     * @param position Position dans la grille
     * @param align Alignement dans la cellule
     */
    void placeInGrid(lv_obj_t* widget, const GridPosition& position, 
                    lv_grid_align_t align = LV_GRID_ALIGN_STRETCH);
    
    /**
     * @brief Obtient la configuration de grille actuelle
     * @return Configuration de grille
     */
    const GridConfig& getGridConfig() const { return grid_config_; }
    
    //=========================================================================
    // POSITIONNEMENT RELATIF
    //=========================================================================
    
    
    /**
     * @brief Positionne un widget de manière relative
     * @param widget Widget à positionner
     * @param x_percent Position X relative (0.0 - 1.0)
     * @param y_percent Position Y relative (0.0 - 1.0)
     * @param width_percent Largeur relative (0.0 - 1.0, 0 = pas de changement)
     * @param height_percent Hauteur relative (0.0 - 1.0, 0 = pas de changement)
     */
    void placeRelative(lv_obj_t* widget, float x_percent, float y_percent, 
                      float width_percent = 0.0f, float height_percent = 0.0f);
    
    //=========================================================================
    // ALIGNEMENT ET CENTRAGE
    //=========================================================================
    
    /**
     * @brief Centre un widget horizontalement
     * @param widget Widget à centrer
     * @param offset_y Décalage vertical optionnel
     */
    void centerHorizontally(lv_obj_t* widget, lv_coord_t offset_y = 0);
    
    /**
     * @brief Centre un widget verticalement
     * @param widget Widget à centrer
     * @param offset_x Décalage horizontal optionnel
     */
    void centerVertically(lv_obj_t* widget, lv_coord_t offset_x = 0);
    
    /**
     * @brief Centre un widget complètement
     * @param widget Widget à centrer
     * @param offset_x Décalage horizontal optionnel
     * @param offset_y Décalage vertical optionnel
     */
    void centerCompletely(lv_obj_t* widget, lv_coord_t offset_x = 0, lv_coord_t offset_y = 0);
    
    /**
     * @brief Aligne un widget par rapport à un autre
     * @param widget Widget à aligner
     * @param reference Widget de référence
     * @param align Type d'alignement LVGL
     * @param offset_x Décalage horizontal
     * @param offset_y Décalage vertical
     */
    void alignToWidget(lv_obj_t* widget, lv_obj_t* reference, lv_align_t align, 
                      lv_coord_t offset_x = 0, lv_coord_t offset_y = 0);
    
    //=========================================================================
    // DISTRIBUTION AUTOMATIQUE AVEC FLEXBOX NATIF
    //=========================================================================
    
    /**
     * @brief Configure un container avec layout flex natif LVGL
     * @param container Container à configurer
     * @param flow Direction du flex (ROW, COLUMN, etc.)
     * @param main_align Alignement sur l'axe principal
     * @param cross_align Alignement sur l'axe transversal
     * @param track_align Alignement des pistes (pour wrap)
     */
    void setupFlex(lv_obj_t* container, lv_flex_flow_t flow,
                  lv_flex_align_t main_align = LV_FLEX_ALIGN_START,
                  lv_flex_align_t cross_align = LV_FLEX_ALIGN_START,
                  lv_flex_align_t track_align = LV_FLEX_ALIGN_START);
    
    /**
     * @brief Configure le facteur de croissance d'un widget dans un container flex
     * @param widget Widget à configurer
     * @param grow Facteur de croissance (0 = pas de croissance, 1+ = croissance)
     */
    void setFlexGrow(lv_obj_t* widget, uint8_t grow);
    
    //=========================================================================
    // ZONES PRÉDÉFINIES
    //=========================================================================
    
    /**
     * @brief Obtient la zone de contenu principal (sans marges)
     * @return Rectangle de la zone de contenu
     */
    Rect getContentArea() const;
    
    /**
     * @brief Obtient la zone header
     * @return Rectangle du header
     */
    Rect getHeaderArea() const;
    
    /**
     * @brief Obtient la zone footer
     * @return Rectangle du footer
     */
    Rect getFooterArea() const;
    
    /**
     * @brief Obtient la zone de navigation (côté)
     * @param left true pour côté gauche, false pour droite
     * @return Rectangle de la zone de navigation
     */
    Rect getNavigationArea(bool left = true) const;
    
    //=========================================================================
    // RESPONSIVE DESIGN
    //=========================================================================
    
    /**
     * @brief Détecte l'orientation de l'écran
     * @return true si paysage, false si portrait
     */
    bool isLandscape() const;
    
    
    /**
     * @brief Adapte une dimension selon le facteur d'échelle
     * @param dimension Dimension originale
     * @return Dimension adaptée
     */
    lv_coord_t scaleToDisplay(lv_coord_t dimension) const;
    
    //=========================================================================
    // HELPERS STATIQUES
    //=========================================================================
    
    /**
     * @brief Crée une configuration de grille par défaut
     * @return Configuration de grille
     */
    static GridConfig createDefaultGrid();
    
    /**
     * @brief Crée une configuration de grille compacte
     * @return Configuration de grille compacte
     */
    static GridConfig createCompactGrid();
    
    
    //=========================================================================
    // MÉTHODES UTILITAIRES POUR LAYOUTS NATIFS
    //=========================================================================
    
    /**
     * @brief Configure un container flex horizontal avec centrage
     * @param container Container à configurer
     * @param spacing Espacement entre éléments (via padding)
     */
    void setupHorizontalFlex(lv_obj_t* container, lv_coord_t spacing = 8);
    
    /**
     * @brief Configure un container flex vertical avec centrage
     * @param container Container à configurer
     * @param spacing Espacement entre éléments (via padding)
     */
    void setupVerticalFlex(lv_obj_t* container, lv_coord_t spacing = 8);
    
    /**
     * @brief Configure une grille simple pour paramètres (ex: 4x3 pour ParameterWidget)
     * @param container Container à configurer
     * @param columns Nombre de colonnes
     * @param rows Nombre de lignes
     */
    void setupParameterGrid(lv_obj_t* container, uint8_t columns = 4, uint8_t rows = 3);

private:
    //=========================================================================
    // MEMBRES PRIVÉS
    //=========================================================================
    
    const UITheme& theme_;                      ///< Référence vers le thème
    GridConfig grid_config_;                    ///< Configuration de grille actuelle
    
    //=========================================================================
    // MÉTHODES PRIVÉES
    //=========================================================================
    
    /**
     * @brief Valide une position de grille
     * @param position Position à valider
     * @return true si valide
     */
    bool isValidGridPosition(const GridPosition& position) const;
};