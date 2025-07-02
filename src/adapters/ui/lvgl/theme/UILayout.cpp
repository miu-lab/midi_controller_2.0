#include "UILayout.hpp"


//=============================================================================
// Constructeur
//=============================================================================

UILayout::UILayout(const UITheme& theme) : theme_(theme) {
    grid_config_ = createDefaultGrid();
    // DEBUG MSG TO IMPLEMENT
}

//=============================================================================
// Gestion de grille native LVGL
//=============================================================================

void UILayout::setupGrid(lv_obj_t* container, const GridConfig& config) {
    if (!container) return;
    
    // Utiliser les unités fractionnelles LVGL pour distribution automatique
    static int32_t col_dsc[9];  // Max 8 colonnes + terminateur
    static int32_t row_dsc[9];  // Max 8 lignes + terminateur
    
    // LVGL natif : LV_GRID_FR(1) distribue automatiquement l'espace disponible
    for (uint8_t i = 0; i < config.columns && i < 8; i++) {
        col_dsc[i] = LV_GRID_FR(1);  // Distribution égale automatique
    }
    col_dsc[config.columns] = LV_GRID_TEMPLATE_LAST;
    
    // Distribution égale automatique pour les lignes
    for (uint8_t i = 0; i < config.rows && i < 8; i++) {
        row_dsc[i] = LV_GRID_FR(1);  // Distribution égale automatique
    }
    row_dsc[config.rows] = LV_GRID_TEMPLATE_LAST;
    
    // Configurer le container avec le layout grid
    lv_obj_set_layout(container, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_column_dsc_array(container, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(container, row_dsc, 0);
    
    // Configurer les gaps et marges
    lv_obj_set_style_pad_column(container, config.gap_horizontal, 0);
    lv_obj_set_style_pad_row(container, config.gap_vertical, 0);
    lv_obj_set_style_pad_left(container, config.margin_left, 0);
    lv_obj_set_style_pad_right(container, config.margin_right, 0);
    lv_obj_set_style_pad_top(container, config.margin_top, 0);
    lv_obj_set_style_pad_bottom(container, config.margin_bottom, 0);
    
    // Sauvegarder la config pour référence
    grid_config_ = config;
    
    // DEBUG MSG TO IMPLEMENT
}

void UILayout::placeInGrid(lv_obj_t* widget, const GridPosition& position, 
                          lv_grid_align_t align) {
    if (!widget) return;
    
    if (!isValidGridPosition(position)) {
        // DEBUG MSG TO IMPLEMENT
        return;
    }
    
    // Utiliser l'API native LVGL pour placer dans la grille
    lv_obj_set_grid_cell(widget, 
                         align, position.column, position.column_span,
                         align, position.row, position.row_span);
    
    // DEBUG MSG TO IMPLEMENT
}

//=============================================================================
// Positionnement relatif avec API native LVGL
//=============================================================================

void UILayout::placeRelative(lv_obj_t* widget, float x_percent, float y_percent, 
                            float width_percent, float height_percent) {
    if (!widget) return;
    
    // Utiliser l'API native LVGL pour positionnement en pourcentage
    lv_obj_align(widget, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // Convertir en entier pour lv_pct() (attend des entiers 0-100)
    lv_coord_t x_pct = (lv_coord_t)(x_percent * 100);
    lv_coord_t y_pct = (lv_coord_t)(y_percent * 100);
    
    // Position native en pourcentage
    lv_obj_set_x(widget, lv_pct(x_pct));
    lv_obj_set_y(widget, lv_pct(y_pct));
    
    // Taille native en pourcentage si spécifiée
    if (width_percent > 0.0f && height_percent > 0.0f) {
        lv_coord_t w_pct = (lv_coord_t)(width_percent * 100);
        lv_coord_t h_pct = (lv_coord_t)(height_percent * 100);
        lv_obj_set_width(widget, lv_pct(w_pct));
        lv_obj_set_height(widget, lv_pct(h_pct));
    }
    
    // DEBUG MSG TO IMPLEMENT
}

//=============================================================================
// Alignement et centrage
//=============================================================================

void UILayout::centerHorizontally(lv_obj_t* widget, lv_coord_t offset_y) {
    if (!widget) return;
    
    lv_obj_align(widget, LV_ALIGN_TOP_MID, 0, offset_y);
    // DEBUG MSG TO IMPLEMENT
}

void UILayout::centerVertically(lv_obj_t* widget, lv_coord_t offset_x) {
    if (!widget) return;
    
    lv_obj_align(widget, LV_ALIGN_LEFT_MID, offset_x, 0);
    // DEBUG MSG TO IMPLEMENT
}

void UILayout::centerCompletely(lv_obj_t* widget, lv_coord_t offset_x, lv_coord_t offset_y) {
    if (!widget) return;
    
    lv_obj_align(widget, LV_ALIGN_CENTER, offset_x, offset_y);
    // DEBUG MSG TO IMPLEMENT
}

void UILayout::alignToWidget(lv_obj_t* widget, lv_obj_t* reference, lv_align_t align, 
                            lv_coord_t offset_x, lv_coord_t offset_y) {
    if (!widget || !reference) return;
    
    lv_obj_align_to(widget, reference, align, offset_x, offset_y);
    // DEBUG MSG TO IMPLEMENT
}

//=============================================================================
// Distribution automatique avec Flexbox natif LVGL
//=============================================================================

void UILayout::setupFlex(lv_obj_t* container, lv_flex_flow_t flow,
                         lv_flex_align_t main_align,
                         lv_flex_align_t cross_align,
                         lv_flex_align_t track_align) {
    if (!container) return;
    
    // Configurer le container avec layout flex
    lv_obj_set_layout(container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container, flow);
    lv_obj_set_flex_align(container, main_align, cross_align, track_align);
    
    // DEBUG MSG TO IMPLEMENT
}

void UILayout::setFlexGrow(lv_obj_t* widget, uint8_t grow) {
    if (!widget) return;
    
    lv_obj_set_flex_grow(widget, grow);
    
    // DEBUG MSG TO IMPLEMENT
}


//=============================================================================
// Zones prédéfinies
//=============================================================================

UILayout::Rect UILayout::getContentArea() const {
    return {
        .x = theme_.screen.safe_margin,
        .y = theme_.screen.safe_margin,
        .width = theme_.screen.content_width,
        .height = theme_.screen.content_height
    };
}

UILayout::Rect UILayout::getHeaderArea() const {
    return {
        .x = 0,
        .y = 0,
        .width = theme_.screen.width,
        .height = theme_.dimensions.header_height
    };
}

UILayout::Rect UILayout::getFooterArea() const {
    return {
        .x = 0,
        .y = theme_.screen.height - theme_.dimensions.footer_height,
        .width = theme_.screen.width,
        .height = theme_.dimensions.footer_height
    };
}

UILayout::Rect UILayout::getNavigationArea(bool left) const {
    lv_coord_t nav_width = 60; // Largeur navigation fixe
    
    return {
        .x = left ? 0 : theme_.screen.width - nav_width,
        .y = theme_.dimensions.header_height,
        .width = nav_width,
        .height = theme_.screen.height - theme_.dimensions.header_height - theme_.dimensions.footer_height
    };
}

//=============================================================================
// Responsive design
//=============================================================================

bool UILayout::isLandscape() const {
    return theme_.screen.width > theme_.screen.height;
}

lv_coord_t UILayout::scaleToDisplay(lv_coord_t dimension) const {
    // Utiliser l'API native LVGL pour dimensions adaptatives
    // lv_disp_get_dpi() fournirait le scaling natif, mais pour simplifier
    // on utilise les unités relatives qui s'adaptent automatiquement
    return dimension; // Les layouts flex/grid s'adaptent automatiquement
}

//=============================================================================
// Helpers statiques
//=============================================================================

UILayout::GridConfig UILayout::createDefaultGrid() {
    return {
        .columns = 4,
        .rows = 3,
        .gap_horizontal = 8,
        .gap_vertical = 8,
        .margin_left = 16,
        .margin_right = 16,
        .margin_top = 16,
        .margin_bottom = 16
    };
}

UILayout::GridConfig UILayout::createCompactGrid() {
    return {
        .columns = 6,
        .rows = 4,
        .gap_horizontal = 4,
        .gap_vertical = 4,
        .margin_left = 8,
        .margin_right = 8,
        .margin_top = 8,
        .margin_bottom = 8
    };
}


//=============================================================================
// Méthodes utilitaires pour layouts natifs
//=============================================================================

void UILayout::setupHorizontalFlex(lv_obj_t* container, lv_coord_t spacing) {
    if (!container) return;
    
    // Configurer flex horizontal avec centrage
    setupFlex(container, LV_FLEX_FLOW_ROW, 
             LV_FLEX_ALIGN_CENTER,     // Centrer sur axe principal
             LV_FLEX_ALIGN_CENTER,     // Centrer sur axe transversal
             LV_FLEX_ALIGN_START);     // Alignement des pistes
    
    // Configurer espacement via padding
    lv_obj_set_style_pad_column(container, spacing, 0);
    
    // DEBUG MSG TO IMPLEMENT
}

void UILayout::setupVerticalFlex(lv_obj_t* container, lv_coord_t spacing) {
    if (!container) return;
    
    // Configurer flex vertical avec centrage
    setupFlex(container, LV_FLEX_FLOW_COLUMN,
             LV_FLEX_ALIGN_CENTER,     // Centrer sur axe principal
             LV_FLEX_ALIGN_CENTER,     // Centrer sur axe transversal  
             LV_FLEX_ALIGN_START);     // Alignement des pistes
    
    // Configurer espacement via padding
    lv_obj_set_style_pad_row(container, spacing, 0);
    
    // DEBUG MSG TO IMPLEMENT
}

void UILayout::setupParameterGrid(lv_obj_t* container, uint8_t columns, uint8_t rows) {
    if (!container) return;
    
    // Créer configuration optimisée pour paramètres MIDI
    GridConfig config = createDefaultGrid();
    config.columns = columns;
    config.rows = rows;
    config.gap_horizontal = theme_.spacing.widget_margin;
    config.gap_vertical = theme_.spacing.widget_margin;
    config.margin_left = theme_.spacing.container_padding;
    config.margin_right = theme_.spacing.container_padding;
    config.margin_top = theme_.spacing.container_padding;
    config.margin_bottom = theme_.spacing.container_padding;
    
    // Utiliser la grille native LVGL
    setupGrid(container, config);
    
    // DEBUG MSG TO IMPLEMENT
}

//=============================================================================
// Méthodes privées
//=============================================================================

bool UILayout::isValidGridPosition(const GridPosition& position) const {
    return (position.column < grid_config_.columns &&
            position.row < grid_config_.rows &&
            position.column + position.column_span <= grid_config_.columns &&
            position.row + position.row_span <= grid_config_.rows &&
            position.column_span > 0 &&
            position.row_span > 0);
}