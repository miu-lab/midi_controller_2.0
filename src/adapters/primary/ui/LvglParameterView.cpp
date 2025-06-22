#include "LvglParameterView.hpp"
#include "config/DisplayConfig.hpp"
#include <Arduino.h>

LvglParameterView::LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge)
    : bridge_(bridge), main_screen_(nullptr), label_(nullptr),
      initialized_(false), active_(false) {
}

LvglParameterView::~LvglParameterView() {
    setActive(false);
    cleanupLvglObjects();
}

bool LvglParameterView::init() {
    if (initialized_) {
        return true;
    }
    
    if (!bridge_) {
        Serial.println(F("LvglParameterView: Bridge LVGL non disponible"));
        return false;
    }
    
    Serial.println(F("LvglParameterView: Initialisation..."));
    
    // Créer l'écran principal
    setupMainScreen();
    
    initialized_ = true;
    Serial.println(F("LvglParameterView: Initialisé avec succès"));
    return true;
}

void LvglParameterView::render() {
    if (!active_ || !initialized_) {
        return;
    }
    
    // Charger l'écran LVGL
    if (main_screen_) {
        lv_screen_load(main_screen_);
    }
    
    // Forcer le rendu LVGL
    lv_timer_handler();
    
    // Forcer le refresh du bridge
    if (bridge_) {
        bridge_->refreshDisplay();
    }
}

void LvglParameterView::update() {
    if (!active_ || !initialized_) {
        return;
    }
    
    // Rien à mettre à jour pour l'instant
}

bool LvglParameterView::isActive() const {
    return active_;
}

void LvglParameterView::setActive(bool active) {
    if (active && !active_) {
        // Activation
        active_ = true;
        Serial.println(F("LvglParameterView: Activé"));
    } else if (!active && active_) {
        // Désactivation
        active_ = false;
        Serial.println(F("LvglParameterView: Désactivé"));
    }
}

void LvglParameterView::setupMainScreen() {
    // Créer l'écran principal
    main_screen_ = lv_obj_create(nullptr);
    
    // Style de l'écran
    lv_obj_set_style_bg_color(main_screen_, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_bg_opa(main_screen_, LV_OPA_COVER, 0);
    
    // Créer le label centré
    label_ = lv_label_create(main_screen_);
    lv_label_set_text(label_, "Parameter View");
    lv_obj_set_style_text_color(label_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_, &lv_font_montserrat_16, 0);
    lv_obj_align(label_, LV_ALIGN_CENTER, 0, 0);
    
    Serial.println(F("LvglParameterView: Écran principal créé"));
}

void LvglParameterView::cleanupLvglObjects() {
    // Nettoyer les objets LVGL
    if (main_screen_) {
        lv_obj_del(main_screen_);
        main_screen_ = nullptr;
        label_ = nullptr;
    }
}