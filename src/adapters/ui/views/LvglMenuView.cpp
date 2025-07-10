#include "LvglMenuView.hpp"
#include "ViewManager.hpp"
#include "config/DisplayConfig.hpp"

LvglMenuView::LvglMenuView(std::shared_ptr<Ili9341LvglBridge> bridge)
    : bridge_(bridge), view_manager_(nullptr),
      initialized_(false), active_(false), selected_index_(0),
      main_screen_(nullptr), menu_(nullptr) {
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
    setupNativeMenu();
    
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
    
    // Le refresh LVGL est géré centralement par SystemManager
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
    if (menu_) {
        // Utiliser l'API LVGL pour naviguer vers le bas
        lv_group_focus_next(lv_group_get_default());
        selected_index_++;
    }
}

void LvglMenuView::selectPrevious() {
    if (menu_) {
        // Utiliser l'API LVGL pour naviguer vers le haut
        lv_group_focus_prev(lv_group_get_default());
        if (selected_index_ > 0) {
            selected_index_--;
        }
    }
}

void LvglMenuView::selectEnter() {
    if (menu_) {
        // Utiliser l'API LVGL pour valider la sélection
        lv_obj_t* focused = lv_group_get_focused(lv_group_get_default());
        if (focused) {
            lv_obj_send_event(focused, LV_EVENT_CLICKED, NULL);
        }
    }
}

void LvglMenuView::setupMainScreen() {
    main_screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(main_screen_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(main_screen_, LV_OPA_COVER, 0);
}

void LvglMenuView::setupNativeMenu() {
    // Créer le widget menu natif LVGL
    menu_ = lv_menu_create(main_screen_);
    lv_obj_set_size(menu_, lv_pct(100), lv_pct(100));
    lv_obj_center(menu_);
    
    // Configurer le style pour un meilleur affichage
    lv_obj_set_style_bg_color(menu_, lv_color_hex(0x222222), 0);
    lv_obj_set_style_text_color(menu_, lv_color_hex(0xFFFFFF), 0);
    
    // Créer la page principale du menu
    lv_obj_t* main_page = lv_menu_page_create(menu_, "MIDI Controller");
    
    // Créer les sections du menu
    createMainMenuSection(main_page);
    createSettingsSection(main_page);
    
    // Définir la page principale comme page active
    lv_menu_set_page(menu_, main_page);
    
    // Ajouter le menu au groupe par défaut pour la navigation
    lv_group_t* group = lv_group_get_default();
    if (!group) {
        group = lv_group_create();
        lv_group_set_default(group);
    }
    lv_group_add_obj(group, menu_);
}

void LvglMenuView::createMainMenuSection(lv_obj_t* parent_page) {
    lv_obj_t* section = lv_menu_section_create(parent_page);
    
    // Ajouter des éléments au menu principal
    lv_obj_t* home_item = lv_menu_cont_create(section);
    lv_obj_t* home_label = lv_label_create(home_item);
    lv_label_set_text(home_label, "> Home");
    lv_obj_add_event_cb(home_item, menuItemEventHandler, LV_EVENT_CLICKED, this);
    
    lv_obj_t* params_item = lv_menu_cont_create(section);
    lv_obj_t* params_label = lv_label_create(params_item);
    lv_label_set_text(params_label, "> Parameters");
    lv_obj_add_event_cb(params_item, menuItemEventHandler, LV_EVENT_CLICKED, this);
    
    lv_obj_t* profiles_item = lv_menu_cont_create(section);
    lv_obj_t* profiles_label = lv_label_create(profiles_item);
    lv_label_set_text(profiles_label, "> Profiles");
    lv_obj_add_event_cb(profiles_item, menuItemEventHandler, LV_EVENT_CLICKED, this);
}

void LvglMenuView::createSettingsSection(lv_obj_t* parent_page) {
    lv_obj_t* section = lv_menu_section_create(parent_page);
    
    // Créer une sous-page pour les paramètres
    lv_obj_t* settings_page = lv_menu_page_create(menu_, "Settings");
    
    lv_obj_t* settings_item = lv_menu_cont_create(section);
    lv_obj_t* settings_label = lv_label_create(settings_item);
    lv_label_set_text(settings_label, "> Settings");
    lv_menu_set_load_page_event(menu_, settings_item, settings_page);
    
    // Ajouter des éléments à la page Settings
    addSettingsItems(settings_page);
}

void LvglMenuView::addSettingsItems(lv_obj_t* settings_page) {
    lv_obj_t* section = lv_menu_section_create(settings_page);
    
    // MIDI Channel
    lv_obj_t* midi_item = lv_menu_cont_create(section);
    lv_obj_t* midi_label = lv_label_create(midi_item);
    lv_label_set_text(midi_label, "MIDI Channel: 1");
    
    // Display Brightness
    lv_obj_t* brightness_item = lv_menu_cont_create(section);
    lv_obj_t* brightness_label = lv_label_create(brightness_item);
    lv_label_set_text(brightness_label, "Brightness: 80%");
    
    // About
    lv_obj_t* about_item = lv_menu_cont_create(section);
    lv_obj_t* about_label = lv_label_create(about_item);
    lv_label_set_text(about_label, "> About");
}

void LvglMenuView::updateSelection() {
    // Simple mise en évidence - peut être améliorée plus tard
}

void LvglMenuView::cleanupLvglObjects() {
    if (main_screen_) {
        lv_obj_del(main_screen_);
        main_screen_ = nullptr;
        menu_ = nullptr;
    }
}

void LvglMenuView::menuItemEventHandler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    LvglMenuView* menu_view = (LvglMenuView*)lv_event_get_user_data(e);
    
    if (code == LV_EVENT_CLICKED && menu_view && menu_view->view_manager_) {
        // Déterminer quelle action effectuer selon l'élément cliqué
        lv_obj_t* label = lv_obj_get_child(obj, 0);
        if (label) {
            const char* text = lv_label_get_text(label);
            if (strcmp(text, "> Home") == 0) {
                menu_view->view_manager_->showHome();
            } else if (strcmp(text, "> Parameters") == 0) {
                menu_view->view_manager_->showParameterFocus(0, 1, 0, "Parameter");
            } else if (strcmp(text, "> Profiles") == 0) {
                menu_view->view_manager_->showMenu();
            }
        }
    }
}