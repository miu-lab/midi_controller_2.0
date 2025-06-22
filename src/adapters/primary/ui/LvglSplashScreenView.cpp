#include "LvglSplashScreenView.hpp"
#include "config/DisplayConfig.hpp"
#include <Arduino.h>

LvglSplashScreenView::LvglSplashScreenView(std::shared_ptr<Ili9341LvglBridge> bridge,
                                         const Config& config)
    : config_(config), bridge_(bridge),
      initialized_(false), active_(false), start_time_(0),
      main_screen_(nullptr), title_label_(nullptr), subtitle_label_(nullptr),
      version_label_(nullptr), progress_bar_(nullptr), border_rect_(nullptr) {
}

LvglSplashScreenView::~LvglSplashScreenView() {
    setActive(false);
    cleanupLvglObjects();
}

bool LvglSplashScreenView::init() {
    if (initialized_) {
        return true;
    }
    
    if (!bridge_) {
        Serial.println(F("LvglSplashScreenView: Bridge LVGL non disponible"));
        return false;
    }
    
    Serial.println(F("LvglSplashScreenView: Initialisation..."));
    
    // Créer l'écran principal
    setupMainScreen();
    
    // Créer la bordure
    setupBorder();
    
    // Créer les labels
    setupLabels();
    
    // Créer la barre de progression
    setupProgressBar();
    
    initialized_ = true;
    Serial.println(F("LvglSplashScreenView: Initialisé avec succès"));
    return true;
}

void LvglSplashScreenView::update() {
    if (!active_ || !initialized_) {
        return;
    }
    
    // Si la vue vient d'être activée, initialiser le temps de départ
    if (start_time_ == 0) {
        start_time_ = millis();
    }
    
    // Mettre à jour la barre de progression
    updateProgressBar();
}

void LvglSplashScreenView::render() {
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

bool LvglSplashScreenView::handleEvent(uint8_t eventType, int32_t data) {
    // Le splash screen ignore tous les événements
    return false;
}

bool LvglSplashScreenView::isActive() const {
    return active_;
}

void LvglSplashScreenView::setActive(bool active) {
    if (active && !active_) {
        // Activation
        active_ = true;
        start_time_ = millis();
        Serial.println(F("LvglSplashScreenView: Activé"));
    } else if (!active && active_) {
        // Désactivation
        active_ = false;
        start_time_ = 0;
        Serial.println(F("LvglSplashScreenView: Désactivé"));
    }
}

bool LvglSplashScreenView::isSplashScreenCompleted() const {
    if (!active_) return true;
    
    unsigned long current_time = millis();
    return (current_time - start_time_ >= config_.duration);
}

void LvglSplashScreenView::setupMainScreen() {
    // Créer l'écran principal
    main_screen_ = lv_obj_create(nullptr);
    
    // Style de l'écran
    lv_obj_set_style_bg_color(main_screen_, config_.bg_color, 0);
    lv_obj_set_style_bg_opa(main_screen_, LV_OPA_COVER, 0);
    
    Serial.println(F("LvglSplashScreenView: Écran principal créé"));
}

void LvglSplashScreenView::setupBorder() {
    // Créer la bordure (rectangle non rempli)
    border_rect_ = lv_obj_create(main_screen_);
    lv_obj_set_size(border_rect_, DisplayConfig::SCREEN_WIDTH - 4, DisplayConfig::SCREEN_HEIGHT - 4);
    lv_obj_set_pos(border_rect_, 2, 2);
    
    // Style de la bordure
    lv_obj_set_style_bg_opa(border_rect_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(border_rect_, config_.text_color, 0);
    lv_obj_set_style_border_width(border_rect_, 2, 0);
    lv_obj_set_style_border_opa(border_rect_, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(border_rect_, 0, 0);
}

void LvglSplashScreenView::setupLabels() {
    // Titre centré en haut
    title_label_ = lv_label_create(main_screen_);
    lv_label_set_text(title_label_, config_.title.c_str());
    lv_obj_set_style_text_color(title_label_, config_.text_color, 0);
    lv_obj_set_style_text_font(title_label_, &lv_font_montserrat_16, 0);
    lv_obj_align(title_label_, LV_ALIGN_TOP_MID, 0, 50);
    
    // Sous-titre centré au milieu
    subtitle_label_ = lv_label_create(main_screen_);
    lv_label_set_text(subtitle_label_, config_.subtitle.c_str());
    lv_obj_set_style_text_color(subtitle_label_, config_.text_color, 0);
    lv_obj_set_style_text_font(subtitle_label_, &lv_font_montserrat_14, 0);
    lv_obj_align(subtitle_label_, LV_ALIGN_CENTER, 0, 0);
    
    // Version en bas à droite
    version_label_ = lv_label_create(main_screen_);
    lv_label_set_text(version_label_, config_.version.c_str());
    lv_obj_set_style_text_color(version_label_, config_.text_color, 0);
    lv_obj_set_style_text_font(version_label_, &lv_font_montserrat_12, 0);
    lv_obj_align(version_label_, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
}

void LvglSplashScreenView::setupProgressBar() {
    // Créer la barre de progression
    progress_bar_ = lv_bar_create(main_screen_);
    lv_obj_set_size(progress_bar_, DisplayConfig::SCREEN_WIDTH - 40, 10);
    lv_obj_align(progress_bar_, LV_ALIGN_BOTTOM_MID, 0, -80);
    
    // Style de la barre
    lv_obj_set_style_bg_color(progress_bar_, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_color(progress_bar_, config_.progress_color, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(progress_bar_, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(progress_bar_, LV_OPA_COVER, LV_PART_INDICATOR);
    
    // Initialiser à 0%
    lv_bar_set_range(progress_bar_, 0, 100);
    lv_bar_set_value(progress_bar_, 0, LV_ANIM_OFF);
}

void LvglSplashScreenView::updateProgressBar() {
    if (!progress_bar_ || start_time_ == 0) {
        return;
    }
    
    unsigned long current_time = millis();
    unsigned long elapsed = current_time - start_time_;
    
    if (elapsed <= config_.duration) {
        // Calculer le pourcentage
        int progress = (elapsed * 100) / config_.duration;
        lv_bar_set_value(progress_bar_, progress, LV_ANIM_OFF);
    } else {
        // Terminé
        lv_bar_set_value(progress_bar_, 100, LV_ANIM_OFF);
    }
}

void LvglSplashScreenView::cleanupLvglObjects() {
    // Nettoyer les objets LVGL
    if (main_screen_) {
        lv_obj_del(main_screen_);
        main_screen_ = nullptr;
        title_label_ = nullptr;
        subtitle_label_ = nullptr;
        version_label_ = nullptr;
        progress_bar_ = nullptr;
        border_rect_ = nullptr;
    }
}