#include "LvglMenuView.hpp"
#include "ViewManager.hpp"
#include "DefaultViewManager.hpp"
#include "config/DisplayConfig.hpp"
#include "config/UIConstants.hpp"

LvglMenuView::LvglMenuView(std::shared_ptr<Ili9341LvglBridge> bridge)
    : bridge_(bridge), view_manager_(nullptr),
      initialized_(false), active_(false), selected_index_(0),
      main_screen_(nullptr), menu_(nullptr),
      root_page_(nullptr), wifi_page_(nullptr), bluetooth_page_(nullptr),
      audio_page_(nullptr), input_page_(nullptr), display_page_(nullptr),
      about_page_(nullptr) {
}

LvglMenuView::~LvglMenuView() {
    setActive(false);
    cleanupLvglObjects();
    
    // Nettoyer les builders ETL optional (automatique)
    page_factory_.reset();
    page_builder_.reset();
}

bool LvglMenuView::init() {
    if (initialized_) {
        return true;
    }
    
    if (!bridge_) {
        return false;
    }
    
    setupMainScreen();
    
    // D'abord créer le menu de base SANS contenu
    setupBasicMenu();
    
    // Initialiser les builders ETL avec allocation statique
    page_builder_.emplace(menu_);
    page_factory_.emplace(*page_builder_);
    
    // MAINTENANT créer le contenu avec les builders initialisés
    createSophisticatedMenu();
    
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
            // INVERSION : selectNext appelle focus_prev pour inverser le sens
            lv_group_focus_prev(group);
            if (selected_index_ > 0) {
                selected_index_--;
            }
        }
    }
}

void LvglMenuView::selectPrevious() {
    if (menu_) {
        lv_group_t* group = lv_group_get_default();
        if (group) {
            // INVERSION : selectPrevious appelle focus_next pour inverser le sens
            lv_group_focus_next(group);
            selected_index_++;
        }
    }
}

void LvglMenuView::selectEnter() {
    if (!menu_) {
        return;
    }
    
    lv_group_t* group = lv_group_get_default();
    if (!group) {
        return;
    }
    
    // Utiliser l'API LVGL pour valider la sélection
    lv_obj_t* focused = lv_group_get_focused(group);
    if (focused) {
        lv_obj_send_event(focused, LV_EVENT_CLICKED, NULL);
    }
}

void LvglMenuView::goBackToRoot() {
    if (!menu_ || !root_page_) {
        return;
    }
    
    // Retourner à la page racine du menu
    lv_menu_set_page(menu_, root_page_);
}

void LvglMenuView::goBackOneLevel() {
    if (!menu_) {
        return;
    }
    
    // Vérifier si on est à la racine
    lv_obj_t* current_page = lv_menu_get_cur_main_page(menu_);
    if (current_page == root_page_) {
        return;
    }
    
    // Utiliser l'API LVGL native pour naviguer en arrière
    // lv_menu_clear_history() ou chercher l'API de back
    // Pour l'instant, on retourne à la page racine comme fallback
    lv_menu_set_page(menu_, root_page_);
}

void LvglMenuView::setupMainScreen() {
    main_screen_ = lv_obj_create(nullptr);

    // FORCER LA TAILLE COMPLETE DE L'ECRAN
    // lv_obj_set_size(main_screen_, lv_pct(100), lv_pct(100));
    // lv_obj_center(main_screen_);
    // lv_obj_set_style_margin_hor(main_screen_, 20, 0);

    lv_obj_set_style_bg_color(main_screen_, lv_color_hex(UIConstants::Colors::BLACK), 0);
    lv_obj_set_style_bg_opa(main_screen_, LV_OPA_100, 0);
}

void LvglMenuView::setupBasicMenu() {
    // Créer le groupe LVGL EN PREMIER
    lv_group_t* group = lv_group_get_default();
    if (!group) {
        group = lv_group_create();
        lv_group_set_default(group);
    }

    // Créer le widget menu natif LVGL - SANS contenu pour l'instant
    menu_ = lv_menu_create(main_screen_);

    // Style minimal pour le menu
    lv_obj_set_style_bg_opa(menu_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_color(menu_, lv_color_hex(UIConstants::Colors::WHITE), 0);
    
    // Ajouter callback pour notifications de changement de page
    lv_obj_add_event_cb(menu_, menuPageChangeCallback, LV_EVENT_VALUE_CHANGED, this);
}

void LvglMenuView::createSophisticatedMenu() {
    // Créer la page racine
    root_page_ = lv_menu_page_create(menu_, NULL);
    lv_obj_set_style_bg_opa(root_page_, LV_OPA_TRANSP, 0);

    // Créer les sous-pages
    createSubPages();

    // Créer le contenu de la page racine
    createRootPageContent();
    
    // Charger la page racine une fois tout créé
    lv_menu_set_page(menu_, root_page_);
}

void LvglMenuView::createSubPages() {
    // Builder Pattern ETL - allocation statique pure
    wifi_page_ = page_factory_->createWiFiPage(root_page_);
    bluetooth_page_ = page_factory_->createBluetoothPage(root_page_);
    audio_page_ = page_factory_->createAudioPage(root_page_);
    input_page_ = page_factory_->createInputPage(root_page_);
    display_page_ = page_factory_->createDisplayPage(root_page_);
    about_page_ = page_factory_->createAboutPage(root_page_);
}

void LvglMenuView::createRootPageContent() {
    // Builder Pattern ETL pur - allocation statique garantie
    lv_obj_t* section = page_builder_->createSection(root_page_);

    // Créer tous les éléments de navigation avec UIConstants
    page_builder_->createNavigationItem(section, UIConstants::Labels::WIFI_SETTINGS, wifi_page_);
    page_builder_->createNavigationItem(section, UIConstants::Labels::BLUETOOTH_SETTINGS, bluetooth_page_);
    page_builder_->createNavigationItem(section, UIConstants::Labels::AUDIO_SETTINGS, audio_page_);
    page_builder_->createNavigationItem(section, UIConstants::Labels::INPUT_SETTINGS, input_page_);
    page_builder_->createNavigationItem(section, UIConstants::Labels::DISPLAY_SETTINGS, display_page_);
    page_builder_->createNavigationItem(section, UIConstants::Labels::ABOUT, about_page_);
}

// ===================================================================
// MÉTHODES SUPPRIMÉES - REMPLACÉES PAR MenuPageBuilder et MenuPageFactory
// ===================================================================
// 
// Les méthodes suivantes ont été supprimées pour éliminer la duplication de code :
// - applyItemStyle() -> remplacée par MenuPageBuilder::applyStandardItemStyle()
// - createWiFiPage() -> remplacée par MenuPageFactory::createWiFiPage()
// - createBluetoothPage() -> remplacée par MenuPageFactory::createBluetoothPage()
// - createAudioPage() -> remplacée par MenuPageFactory::createAudioPage()
// - createInputPage() -> remplacée par MenuPageFactory::createInputPage()
// - createDisplayPage() -> remplacée par MenuPageFactory::createDisplayPage()
// - createAboutPage() -> remplacée par MenuPageFactory::createAboutPage()
//
// Ces méthodes contenaient plus de 300 lignes de code dupliqué qui sont maintenant
// gérées de manière centralisée et réutilisable.

void LvglMenuView::updateSelection() {
    // Navigation gérée automatiquement par LVGL
}

void LvglMenuView::cleanupLvglObjects() {
    if (main_screen_) {
        lv_obj_del(main_screen_);
        main_screen_ = nullptr;
        menu_ = nullptr;
        
        // Les pages sont détruites automatiquement avec le menu
        root_page_ = nullptr;
        wifi_page_ = nullptr;
        bluetooth_page_ = nullptr;
        audio_page_ = nullptr;
        input_page_ = nullptr;
        display_page_ = nullptr;
        about_page_ = nullptr;
    }
}

void LvglMenuView::menuItemEventHandler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    LvglMenuView* menu_view = (LvglMenuView*)lv_event_get_user_data(e);
    
    if (code == LV_EVENT_CLICKED) {
        if (menu_view && menu_view->view_manager_) {
            // La navigation entre pages est maintenant gérée automatiquement par LVGL
            // via lv_menu_set_load_page_event() dans createRootPageContent()
            
            // Actions spécialisées peuvent être ajoutées ici si nécessaire
            // pour certains éléments spécifiques
        }
    }
}

void LvglMenuView::menuPageChangeCallback(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* menu = (lv_obj_t*)lv_event_get_target(e);
    LvglMenuView* menu_view = (LvglMenuView*)lv_event_get_user_data(e);
    
    if (code == LV_EVENT_VALUE_CHANGED && menu_view) {
        lv_obj_t* current_page = lv_menu_get_cur_main_page(menu);
        
        // Déterminer si on est sur la page racine ou une sous-page
        bool is_root_page = (current_page == menu_view->root_page_);
        
        if (!is_root_page) {
            // Notifier le ViewManager qu'on est entré dans une sous-page
            if (menu_view->view_manager_) {
                // Cast vers DefaultViewManager pour accéder à onSubPageEntered()
                auto* default_vm = static_cast<class DefaultViewManager*>(menu_view->view_manager_);
                if (default_vm) {
                    default_vm->onSubPageEntered();
                }
            }
        }
    }
}