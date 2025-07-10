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
        lv_group_t* group = lv_group_get_default();
        if (group) {
            lv_group_focus_next(group);
            selected_index_++;
        }
    }
}

void LvglMenuView::selectPrevious() {
    if (menu_) {
        lv_group_t* group = lv_group_get_default();
        if (group) {
            lv_group_focus_prev(group);
            if (selected_index_ > 0) {
                selected_index_--;
            }
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
    lv_obj_set_style_bg_opa(main_screen_, LV_OPA_100, 0);
}

void LvglMenuView::setupNativeMenu() {
    // Créer le groupe LVGL EN PREMIER
    lv_group_t* group = lv_group_get_default();
    if (!group) {
        group = lv_group_create();
        lv_group_set_default(group);
    }

    // Créer le widget menu natif LVGL - plus simple
    menu_ = lv_menu_create(main_screen_);
    lv_obj_set_size(menu_, lv_pct(80), lv_pct(100));

    // Style minimal pour le menu
    lv_obj_set_style_bg_opa(menu_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_color(menu_, lv_color_hex(0xFFFFFF), 0);

    // Créer la page principale
    lv_obj_t* main_page = lv_menu_page_create(menu_, NULL);

    // Rendre la page transparente
    lv_obj_set_style_bg_opa(main_page, LV_OPA_TRANSP, 0);

    // Menu items simples
    createSimpleMenu(main_page);

    // Activer la page
    lv_menu_set_page(menu_, main_page);
}

void LvglMenuView::createSimpleMenu(lv_obj_t* main_page) {
    // Créer une seule section simple
    lv_obj_t* section = lv_menu_section_create(main_page);

    // Rendre la section transparente
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);

    // Liste des éléments de menu
    const char* menu_items[] = {"Home", "Parameters", "Profiles", "Settings", "About"};

    // Créer chaque élément de menu de façon simplifiée
    for (int i = 0; i < 5; i++) {
        lv_obj_t* item = lv_menu_cont_create(section);
        lv_obj_t* label = lv_label_create(item);
        lv_label_set_text(label, menu_items[i]);

        // FOND TRANSPARENT pour les items
        lv_obj_set_style_bg_opa(item, LV_OPA_TRANSP, 0);

        // Style focus automatique via LVGL - BORDURE INTENSE
        lv_obj_set_style_border_side(item, LV_BORDER_SIDE_LEFT, LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(item, 5, LV_STATE_FOCUSED);  // Plus large
        lv_obj_set_style_border_color(item,
                                      lv_color_hex(0x96FC6A),
                                      LV_STATE_FOCUSED);                    // Vert pur plus intense
        lv_obj_set_style_border_opa(item, LV_OPA_COVER, LV_STATE_FOCUSED);  // Opacité complète

        // Ajouter au groupe pour navigation
        lv_group_t* group = lv_group_get_default();
        if (group) {
            lv_group_add_obj(group, item);
        }

        // Event handler simple
        lv_obj_add_event_cb(item, menuItemEventHandler, LV_EVENT_CLICKED, this);
    }
}

void LvglMenuView::updateSelection() {
    // Navigation gérée automatiquement par LVGL
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
        // Récupérer le texte de l'élément cliqué
        lv_obj_t* label = lv_obj_get_child(obj, 0);
        if (label) {
            const char* text = lv_label_get_text(label);

            // Actions simplifiées
            if (strcmp(text, "Home") == 0) {
                menu_view->view_manager_->showHome();
            } else if (strcmp(text, "Parameters") == 0) {
                menu_view->view_manager_->showParameterFocus(0, 1, 0, "Parameter");
            } else if (strcmp(text, "Settings") == 0) {
                // Rester dans le menu pour l'instant
            }
            // Autres actions peuvent être ajoutées facilement
        }
    }
}