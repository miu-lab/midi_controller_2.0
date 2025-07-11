#pragma once

#include <Arduino.h>
#include <memory>
#include <lvgl.h>
#include <etl/optional.h>

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/ui/components/MenuPageBuilder.hpp"

/**
 * @brief Vue menu utilisant le widget lv_menu natif de LVGL 9.3.0
 * 
 * Cette implémentation utilise le widget menu natif de LVGL pour une navigation
 * hiérarchique moderne avec pages et sections organisées.
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
    void selectEnter(); // Valider la sélection courante
    void goBackToRoot(); // Retourner à la page racine
    void goBackOneLevel(); // Remonter d'un niveau dans la hiérarchie
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
    lv_obj_t* menu_;
    
    // Pages du menu sophistiqué
    lv_obj_t* root_page_;
    lv_obj_t* wifi_page_;
    lv_obj_t* bluetooth_page_;
    lv_obj_t* audio_page_;
    lv_obj_t* input_page_;
    lv_obj_t* display_page_;
    lv_obj_t* about_page_;
    
    // Builders avec allocation statique ETL pour microcontrôleur
    etl::optional<MenuPageBuilder> page_builder_;
    etl::optional<MenuPageFactory> page_factory_;
    
    // Méthodes privées
    void setupMainScreen();
    void setupBasicMenu();
    void createSophisticatedMenu();
    void createSubPages();
    void createRootPageContent();
    void updateSelection();
    void cleanupLvglObjects();
    
    // Event handlers statiques pour LVGL
    static void menuItemEventHandler(lv_event_t* e);
    static void menuPageChangeCallback(lv_event_t* e);
};