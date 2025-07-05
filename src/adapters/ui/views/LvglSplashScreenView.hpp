#pragma once

#include <Arduino.h>
#include <memory>
#include <lvgl.h>

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"

/**
 * @brief Vue splash screen utilisant LVGL directement
 */
class LvglSplashScreenView {
public:
    /**
     * @brief Configuration du splash screen
     */
    struct Config {
        String title;
        String subtitle;
        String version;
        unsigned long duration;
        lv_color_t bg_color;
        lv_color_t text_color;
        lv_color_t progress_color;
        
        Config() : title("MIDI Controller"), subtitle("Welcome"), version("v1.0"), 
                   duration(2000), bg_color(lv_color_hex(0x000000)), 
                   text_color(lv_color_hex(0xFFFFFF)), progress_color(lv_color_hex(0xFF6600)) {}
    };

    /**
     * @brief Constructeur
     * @param bridge Pont LVGL
     * @param config Configuration du splash screen
     */
    explicit LvglSplashScreenView(std::shared_ptr<Ili9341LvglBridge> bridge,
                                 const Config& config = Config());
    
    /**
     * @brief Destructeur
     */
    ~LvglSplashScreenView();
    
    // Interface View
    bool init();
    void update();
    void render();
    bool handleEvent(uint8_t eventType, int32_t data);
    bool isActive() const;
    void setActive(bool active);
    
    /**
     * @brief Vérifie si l'écran de démarrage est terminé
     * @return true si la durée est écoulée
     */
    bool isSplashScreenCompleted() const;

private:
    Config config_;
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    
    // État
    bool initialized_;
    bool active_;
    unsigned long start_time_;
    
    // Objets LVGL
    lv_obj_t* main_screen_;
    lv_obj_t* title_label_;
    lv_obj_t* subtitle_label_;
    lv_obj_t* version_label_;
    lv_obj_t* progress_bar_;
    lv_obj_t* border_rect_;
    
    // Méthodes privées
    void setupMainScreen();
    void setupLabels();
    void setupProgressBar();
    void setupBorder();
    void updateProgressBar();
    void cleanupLvglObjects();
};