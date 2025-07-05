#pragma once

// Mock pour LVGL (car pas disponible dans l'environnement de test)
extern "C" {
    // Structures LVGL minimales pour les tests
    struct _lv_obj_t {
        int dummy;
    };
    
    // Fonctions LVGL de base - utiliser des noms uniques pour éviter les conflits
    _lv_obj_t* mock_lv_obj_create(_lv_obj_t* parent);
    void mock_lv_obj_delete(_lv_obj_t* obj);
    
    // Fonctions de style et positionnement
    void mock_lv_obj_set_size(_lv_obj_t* obj, int w, int h);
    void mock_lv_obj_set_pos(_lv_obj_t* obj, int x, int y);
    void mock_lv_obj_set_style_bg_color(_lv_obj_t* obj, int color, int state);
    void mock_lv_obj_set_style_bg_opa(_lv_obj_t* obj, int opa, int state);
    void mock_lv_obj_set_style_pad_all(_lv_obj_t* obj, int pad, int state);
    void mock_lv_obj_set_style_border_width(_lv_obj_t* obj, int width, int state);
    void mock_lv_obj_set_style_pad_gap(_lv_obj_t* obj, int gap, int state);
    void mock_lv_obj_set_grid_dsc_array(_lv_obj_t* obj, void* col_dsc, void* row_dsc);
    void mock_lv_obj_set_layout(_lv_obj_t* obj, int layout);
    void mock_lv_obj_set_grid_cell(_lv_obj_t* obj, int align_col, int col, int span_col, int align_row, int row, int span_row);
    void mock_lv_obj_center(_lv_obj_t* obj);
    void mock_lv_obj_move_foreground(_lv_obj_t* obj);
    void mock_lv_obj_invalidate(_lv_obj_t* obj);
    void mock_lv_screen_load(_lv_obj_t* screen);
    
    // Fonctions utilitaires
    int mock_lv_color_hex(int color);
}

// Macros pour remapper les fonctions LVGL vers nos mocks
#define lv_obj_create mock_lv_obj_create
#define lv_obj_delete mock_lv_obj_delete