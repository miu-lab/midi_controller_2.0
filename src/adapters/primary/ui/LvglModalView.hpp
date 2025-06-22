#pragma once

#include <Arduino.h>
#include <memory>
#include <lvgl.h>

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"

/**
 * @brief Vue modale simplifiée utilisant LVGL
 */
class LvglModalView{
public:
    explicit LvglModalView(std::shared_ptr<Ili9341LvglBridge> bridge);
    ~LvglModalView();
    
    // Interface View
    bool init();
    void update();
    void render();
    bool handleEvent(uint8_t eventType, int32_t data);
    bool isActive() const;
    void setActive(bool active);
    
    // Configuration du modal
    void setMessage(const char* message);

private:
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    
    // État
    bool initialized_;
    bool active_;
    String message_;
    
    // Objets LVGL
    lv_obj_t* modal_screen_;
    lv_obj_t* bg_overlay_;
    lv_obj_t* message_box_;
    lv_obj_t* message_label_;
    
    // Méthodes privées
    void setupModalScreen();
    void updateMessage();
    void cleanupLvglObjects();
};