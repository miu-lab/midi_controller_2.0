#include "LvglMenuView.hpp"
#include "ViewManager.hpp"
#include "adapters/secondary/hardware/display/DisplayConfig.hpp"

LvglMenuView::LvglMenuView(std::shared_ptr<Ili9341LvglBridge> bridge)
    : bridge_(bridge), view_manager_(nullptr),
      initialized_(false), active_(false), selected_index_(0),
      main_screen_(nullptr), menu_list_(nullptr) {
}

LvglMenuView::~LvglMenuView() {
    setActive(false);
    cleanupLvglObjects();
}

bool LvglMenuView::init() {
    if (initialized_) {
        return true;
    }
    
    if (!bridge_) {
        return false;
    }
    
    setupMainScreen();
    setupMenuList();
    
    initialized_ = true;
    return true;
}

void LvglMenuView::update() {
    // Menu statique, pas de mise à jour nécessaire
}

void LvglMenuView::render() {
    if (!active_ || !initialized_) {
        return;
    }
    
    if (main_screen_) {
        lv_screen_load(main_screen_);
    }
    
    lv_timer_handler();
    
    if (bridge_) {
        bridge_->refreshDisplay();
    }
}

bool LvglMenuView::handleEvent(uint8_t eventType, int32_t data) {
    return false;
}

bool LvglMenuView::isActive() const {
    return active_;
}

void LvglMenuView::setActive(bool active) {
    active_ = active;
}

void LvglMenuView::selectNext() {
    if (selected_index_ < 2) {  // 3 éléments max
        selected_index_++;
        updateSelection();
    }
}

void LvglMenuView::selectPrevious() {
    if (selected_index_ > 0) {
        selected_index_--;
        updateSelection();
    }
}

void LvglMenuView::setupMainScreen() {
    main_screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(main_screen_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(main_screen_, LV_OPA_COVER, 0);
}

void LvglMenuView::setupMenuList() {
    menu_list_ = lv_list_create(main_screen_);
    lv_obj_set_size(menu_list_, DisplayConfig::SCREEN_WIDTH - 20, DisplayConfig::SCREEN_HEIGHT - 20);
    lv_obj_center(menu_list_);
    
    // Ajouter les éléments du menu
    lv_list_add_text(menu_list_, "MENU");
    lv_list_add_btn(menu_list_, nullptr, "Home");
    lv_list_add_btn(menu_list_, nullptr, "Settings");
    lv_list_add_btn(menu_list_, nullptr, "About");
    
    updateSelection();
}

void LvglMenuView::updateSelection() {
    // Simple mise en évidence - peut être améliorée plus tard
}

void LvglMenuView::cleanupLvglObjects() {
    if (main_screen_) {
        lv_obj_del(main_screen_);
        main_screen_ = nullptr;
        menu_list_ = nullptr;
    }
}