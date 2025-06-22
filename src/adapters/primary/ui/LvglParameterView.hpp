#pragma once

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include <memory>
#include <lvgl.h>

/**
 * @brief Vue LVGL minimale pour affichage de paramètres
 */
class LvglParameterView {
public:
    /**
     * @brief Constructeur
     * @param bridge Bridge LVGL vers hardware
     */
    explicit LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge);

    /**
     * @brief Destructeur
     */
    ~LvglParameterView();

    // Interface View
    bool init();
    void render();
    void update();
    bool isActive() const;
    void setActive(bool active);

private:
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    
    // Objets LVGL
    lv_obj_t* main_screen_;
    lv_obj_t* label_;
    
    // État
    bool initialized_;
    bool active_;
    
    // Méthodes privées
    void setupMainScreen();
    void cleanupLvglObjects();
};