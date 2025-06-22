#include "LvglDisplayPortAdapter.hpp"
#include <lvgl.h>
#include <stdarg.h>
#include <stdio.h>

LvglDisplayPortAdapter::LvglDisplayPortAdapter(std::shared_ptr<Ili9341LvglBridge> bridge)
    : bridge_(bridge), currentTextColor_(0xFFFF), currentTextSize_(1), textWrap_(true), cursorX_(0), cursorY_(0), objectCount_(0) {
    // Initialiser les styles une seule fois
    lv_style_init(&textStyle_);
    lv_style_init(&lineStyle_);
    lv_style_init(&rectStyle_);
    
    // Configuration de base des styles
    lv_style_set_text_color(&textStyle_, lv_color_hex(currentTextColor_));
    lv_style_set_line_width(&lineStyle_, 1);
    lv_style_set_line_color(&lineStyle_, lv_color_hex(currentTextColor_));
}

const char* LvglDisplayPortAdapter::getDisplayType() const {
    return "ILI9341_LVGL";
}

void LvglDisplayPortAdapter::getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime, unsigned long& minTime) const {
    // Pour l'instant, retourner des valeurs par défaut
    // TODO: Implémenter les statistiques de performance si nécessaire
    avgTime = 1000;  // 1ms
    maxTime = 2000;  // 2ms
    minTime = 500;   // 0.5ms
}

void LvglDisplayPortAdapter::clear() {
    // Utiliser LVGL pour effacer l'écran et libérer tous les objets
    lv_obj_clean(lv_scr_act());
    objectCount_ = 0;  // Reset du compteur d'objets
}

void LvglDisplayPortAdapter::drawText(int x, int y, const char* text) {
    // Protection contre l'accumulation d'objets
    if (objectCount_ > 50) {  // Limite de sécurité
        Serial.println(F("WARNING: Too many LVGL objects, clearing screen"));
        clear();
    }
    
    // Créer un label LVGL pour afficher le texte
    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, x, y);
    
    // Mettre à jour la couleur du style si nécessaire
    lv_style_set_text_color(&textStyle_, lv_color_hex(currentTextColor_));
    lv_obj_add_style(label, &textStyle_, 0);
    
    objectCount_++;
}

void LvglDisplayPortAdapter::drawLine(int x0, int y0, int x1, int y1) {
    // Créer une ligne LVGL
    lv_obj_t* line = lv_line_create(lv_scr_act());
    
    // Utiliser des points d'instance pour éviter les conflits avec les variables statiques
    lv_point_precise_t line_points[2];
    line_points[0].x = x0;
    line_points[0].y = y0;
    line_points[1].x = x1;
    line_points[1].y = y1;
    
    lv_line_set_points(line, line_points, 2);
    
    // Mettre à jour la couleur du style si nécessaire
    lv_style_set_line_color(&lineStyle_, lv_color_hex(currentTextColor_));
    lv_obj_add_style(line, &lineStyle_, 0);
    
    objectCount_++;
}

void LvglDisplayPortAdapter::drawRect(int x, int y, int width, int height, bool fill) {
    // Créer un rectangle LVGL
    lv_obj_t* rect = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(rect, x, y);
    lv_obj_set_size(rect, width, height);
    
    // Configurer le style selon fill
    if (fill) {
        lv_style_set_bg_color(&rectStyle_, lv_color_hex(currentTextColor_));
        lv_style_set_bg_opa(&rectStyle_, LV_OPA_COVER);
        lv_style_set_border_width(&rectStyle_, 0);
    } else {
        lv_style_set_bg_opa(&rectStyle_, LV_OPA_TRANSP);
        lv_style_set_border_color(&rectStyle_, lv_color_hex(currentTextColor_));
        lv_style_set_border_width(&rectStyle_, 1);
    }
    lv_obj_add_style(rect, &rectStyle_, 0);
    objectCount_++;
}

void LvglDisplayPortAdapter::drawCircle(int x, int y, int radius, bool fill) {
    // Créer un cercle LVGL (utiliser un objet avec border-radius)
    lv_obj_t* circle = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(circle, x - radius, y - radius);
    lv_obj_set_size(circle, radius * 2, radius * 2);
    
    // Réutiliser le style rectangle pour le cercle
    lv_style_set_radius(&rectStyle_, radius);
    
    if (fill) {
        lv_style_set_bg_color(&rectStyle_, lv_color_hex(currentTextColor_));
        lv_style_set_bg_opa(&rectStyle_, LV_OPA_COVER);
        lv_style_set_border_width(&rectStyle_, 0);
    } else {
        lv_style_set_bg_opa(&rectStyle_, LV_OPA_TRANSP);
        lv_style_set_border_color(&rectStyle_, lv_color_hex(currentTextColor_));
        lv_style_set_border_width(&rectStyle_, 1);
    }
    lv_obj_add_style(circle, &rectStyle_, 0);
    objectCount_++;
}

void LvglDisplayPortAdapter::update() {
    // Déléguer au bridge LVGL
    if (bridge_) {
        bridge_->refreshDisplay();
    }
}

void LvglDisplayPortAdapter::setTextSize(uint8_t size) {
    currentTextSize_ = size;
}

void LvglDisplayPortAdapter::setTextColor(uint16_t color) {
    currentTextColor_ = color;
}

void LvglDisplayPortAdapter::setTextWrap(bool wrap) {
    textWrap_ = wrap;
}

void LvglDisplayPortAdapter::setCursor(int16_t x, int16_t y) {
    cursorX_ = x;
    cursorY_ = y;
}

void LvglDisplayPortAdapter::getTextBounds(const char* text, uint16_t* w, uint16_t* h) {
    // Approximation basique - dans un vrai système, on utiliserait les métriques LVGL
    if (text && w && h) {
        *w = strlen(text) * 6 * currentTextSize_;  // Approximation
        *h = 8 * currentTextSize_;  // Hauteur approximative
    }
}

void LvglDisplayPortAdapter::drawCenteredText(int x, int y, const char* text) {
    uint16_t w, h;
    getTextBounds(text, &w, &h);
    drawText(x - w/2, y, text);
}

void LvglDisplayPortAdapter::drawFormattedText(int x, int y, const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    drawText(x, y, buffer);
}