#pragma once

#include <Arduino.h>
#include <memory>
#include <lvgl.h>

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"

/**
 * @brief Vue menu simplifiée utilisant LVGL
 */
class LvglMenuView {
public:
    explicit LvglMenuView(std::shared_ptr<Ili9341LvglBridge> bridge);
    ~LvglMenuView();
    
    // Interface View
    bool init();
    void update();
    void render();
    bool handleEvent(uint8_t eventType, int32_t data);
    bool isActive() const;
    void setActive(bool active);
    
    // Navigation
    void selectNext();
    void selectPrevious();
    int getCurrentIndex() const { return selected_index_; }
    
    // ViewManager reference (pour callbacks)
    void setViewManager(class ViewManager* manager) { view_manager_ = manager; }

private:
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    class ViewManager* view_manager_;
    
    // État
    bool initialized_;
    bool active_;
    int selected_index_;
    
    // Objets LVGL
    lv_obj_t* main_screen_;
    lv_obj_t* menu_list_;
    
    // Méthodes privées
    void setupMainScreen();
    void setupMenuList();
    void updateSelection();
    void cleanupLvglObjects();
};