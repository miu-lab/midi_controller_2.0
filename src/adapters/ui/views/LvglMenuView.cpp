#include "LvglMenuView.hpp"
#include "ViewManager.hpp"
#include "DefaultViewManager.hpp"
#include "config/DisplayConfig.hpp"

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

    // Créer le widget menu natif LVGL - CONFIGURATION SOPHISTIQUEE
    menu_ = lv_menu_create(main_screen_);

    // Style minimal pour le menu
    lv_obj_set_style_bg_opa(menu_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_color(menu_, lv_color_hex(0xFFFFFF), 0);
    
    // Ajouter callback pour notifications de changement de page
    lv_obj_add_event_cb(menu_, menuPageChangeCallback, LV_EVENT_VALUE_CHANGED, this);

    // Créer les pages du menu sophistiqué
    createSophisticatedMenu();

    // Charger la page racine
    lv_menu_set_page(menu_, root_page_);
}

void LvglMenuView::createSophisticatedMenu() {
    // Créer la page racine
    root_page_ = lv_menu_page_create(menu_, NULL);
    lv_obj_set_style_bg_opa(root_page_, LV_OPA_TRANSP, 0);

    // Créer les sous-pages
    createSubPages();

    // Créer le contenu de la page racine
    createRootPageContent();
}

void LvglMenuView::createSubPages() {
    // Page WiFi
    wifi_page_ = lv_menu_page_create(menu_, "WiFi Settings");
    lv_obj_set_style_bg_opa(wifi_page_, LV_OPA_TRANSP, 0);
    createWiFiPage();

    // Page Bluetooth  
    bluetooth_page_ = lv_menu_page_create(menu_, "Bluetooth Settings");
    lv_obj_set_style_bg_opa(bluetooth_page_, LV_OPA_TRANSP, 0);
    createBluetoothPage();

    // Page Audio
    audio_page_ = lv_menu_page_create(menu_, "Audio Settings");
    lv_obj_set_style_bg_opa(audio_page_, LV_OPA_TRANSP, 0);
    createAudioPage();

    // Page Input
    input_page_ = lv_menu_page_create(menu_, "Input Settings");
    lv_obj_set_style_bg_opa(input_page_, LV_OPA_TRANSP, 0);
    createInputPage();

    // Page Display
    display_page_ = lv_menu_page_create(menu_, "Display Settings");
    lv_obj_set_style_bg_opa(display_page_, LV_OPA_TRANSP, 0);
    createDisplayPage();

    // Page About
    about_page_ = lv_menu_page_create(menu_, "About");
    lv_obj_set_style_bg_opa(about_page_, LV_OPA_TRANSP, 0);
    createAboutPage();
}

void LvglMenuView::createRootPageContent() {
    lv_obj_t* section = lv_menu_section_create(root_page_);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(section, 8, 0);

    // WiFi Settings
    lv_obj_t* wifi_item = lv_menu_cont_create(section);
    lv_obj_t* wifi_label = lv_label_create(wifi_item);
    lv_label_set_text(wifi_label, "WiFi Settings");
    lv_menu_set_load_page_event(menu_, wifi_item, wifi_page_);
    applyItemStyle(wifi_item);

    // Bluetooth Settings
    lv_obj_t* bt_item = lv_menu_cont_create(section);
    lv_obj_t* bt_label = lv_label_create(bt_item);
    lv_label_set_text(bt_label, "Bluetooth Settings");
    lv_menu_set_load_page_event(menu_, bt_item, bluetooth_page_);
    applyItemStyle(bt_item);

    // Audio Settings
    lv_obj_t* audio_item = lv_menu_cont_create(section);
    lv_obj_t* audio_label = lv_label_create(audio_item);
    lv_label_set_text(audio_label, "Audio Settings");
    lv_menu_set_load_page_event(menu_, audio_item, audio_page_);
    applyItemStyle(audio_item);

    // Input Settings
    lv_obj_t* input_item = lv_menu_cont_create(section);
    lv_obj_t* input_label = lv_label_create(input_item);
    lv_label_set_text(input_label, "Input Settings");
    lv_menu_set_load_page_event(menu_, input_item, input_page_);
    applyItemStyle(input_item);

    // Display Settings
    lv_obj_t* display_item = lv_menu_cont_create(section);
    lv_obj_t* display_label = lv_label_create(display_item);
    lv_label_set_text(display_label, "Display Settings");
    lv_menu_set_load_page_event(menu_, display_item, display_page_);
    applyItemStyle(display_item);

    // About
    lv_obj_t* about_item = lv_menu_cont_create(section);
    lv_obj_t* about_label = lv_label_create(about_item);
    lv_label_set_text(about_label, "About");
    lv_menu_set_load_page_event(menu_, about_item, about_page_);
    applyItemStyle(about_item);
}

void LvglMenuView::applyItemStyle(lv_obj_t* item) {
    // FOND TRANSPARENT pour les items
    lv_obj_set_style_bg_opa(item, LV_OPA_TRANSP, 0);

    // Style focus automatique via LVGL - BORDURE INTENSE
    lv_obj_set_style_border_side(item, LV_BORDER_SIDE_LEFT, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(item, 5, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(item, lv_color_hex(0x96FC6A), LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(item, LV_OPA_COVER, LV_STATE_FOCUSED);

    // Ajouter au groupe pour navigation
    lv_group_t* group = lv_group_get_default();
    if (group) {
        lv_group_add_obj(group, item);
    }
}

void LvglMenuView::createWiFiPage() {
    lv_obj_t* section = lv_menu_section_create(wifi_page_);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(section, 8, 0);

    // WiFi Enable Switch
    lv_obj_t* switch_item = lv_menu_cont_create(section);
    lv_obj_t* switch_label = lv_label_create(switch_item);
    lv_label_set_text(switch_label, "Enable WiFi");
    lv_obj_t* sw = lv_switch_create(switch_item);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -10, 0);
    applyItemStyle(switch_item);

    // Network SSID
    lv_obj_t* ssid_item = lv_menu_cont_create(section);
    lv_obj_t* ssid_label = lv_label_create(ssid_item);
    lv_label_set_text(ssid_label, "Network: MyWiFi");
    applyItemStyle(ssid_item);

    // Signal Strength
    lv_obj_t* signal_item = lv_menu_cont_create(section);
    lv_obj_t* signal_label = lv_label_create(signal_item);
    lv_label_set_text(signal_label, "Signal: Strong");
    applyItemStyle(signal_item);
}

void LvglMenuView::createBluetoothPage() {
    lv_obj_t* section = lv_menu_section_create(bluetooth_page_);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(section, 8, 0);

    // Bluetooth Enable Switch
    lv_obj_t* switch_item = lv_menu_cont_create(section);
    lv_obj_t* switch_label = lv_label_create(switch_item);
    lv_label_set_text(switch_label, "Enable Bluetooth");
    lv_obj_t* sw = lv_switch_create(switch_item);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    applyItemStyle(switch_item);

    // Paired Devices
    lv_obj_t* devices_item = lv_menu_cont_create(section);
    lv_obj_t* devices_label = lv_label_create(devices_item);
    lv_label_set_text(devices_label, "Paired Devices: 2");
    applyItemStyle(devices_item);

    // Discoverable
    lv_obj_t* discoverable_item = lv_menu_cont_create(section);
    lv_obj_t* discoverable_label = lv_label_create(discoverable_item);
    lv_label_set_text(discoverable_label, "Discoverable");
    lv_obj_t* discoverable_sw = lv_switch_create(discoverable_item);
    lv_obj_align(discoverable_sw, LV_ALIGN_RIGHT_MID, -10, 0);
    applyItemStyle(discoverable_item);
}

void LvglMenuView::createAudioPage() {
    lv_obj_t* section = lv_menu_section_create(audio_page_);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(section, 8, 0);

    // Master Volume
    lv_obj_t* volume_item = lv_menu_cont_create(section);
    lv_obj_t* volume_label = lv_label_create(volume_item);
    lv_label_set_text(volume_label, "Master Volume");
    lv_obj_t* volume_slider = lv_slider_create(volume_item);
    lv_obj_set_size(volume_slider, 120, 10);
    lv_obj_align(volume_slider, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_slider_set_value(volume_slider, 70, LV_ANIM_OFF);
    applyItemStyle(volume_item);

    // Audio Quality
    lv_obj_t* quality_item = lv_menu_cont_create(section);
    lv_obj_t* quality_label = lv_label_create(quality_item);
    lv_label_set_text(quality_label, "Quality: High");
    applyItemStyle(quality_item);

    // Sample Rate
    lv_obj_t* sample_item = lv_menu_cont_create(section);
    lv_obj_t* sample_label = lv_label_create(sample_item);
    lv_label_set_text(sample_label, "Sample Rate: 48kHz");
    applyItemStyle(sample_item);
}

void LvglMenuView::createInputPage() {
    lv_obj_t* section = lv_menu_section_create(input_page_);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(section, 8, 0);

    // Encoder Sensitivity
    lv_obj_t* sensitivity_item = lv_menu_cont_create(section);
    lv_obj_t* sensitivity_label = lv_label_create(sensitivity_item);
    lv_label_set_text(sensitivity_label, "Encoder Sensitivity");
    lv_obj_t* sensitivity_slider = lv_slider_create(sensitivity_item);
    lv_obj_set_size(sensitivity_slider, 120, 10);
    lv_obj_align(sensitivity_slider, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_slider_set_value(sensitivity_slider, 50, LV_ANIM_OFF);
    applyItemStyle(sensitivity_item);

    // Button Debounce
    lv_obj_t* debounce_item = lv_menu_cont_create(section);
    lv_obj_t* debounce_label = lv_label_create(debounce_item);
    lv_label_set_text(debounce_label, "Button Debounce: 30ms");
    applyItemStyle(debounce_item);

    // Long Press Time
    lv_obj_t* longpress_item = lv_menu_cont_create(section);
    lv_obj_t* longpress_label = lv_label_create(longpress_item);
    lv_label_set_text(longpress_label, "Long Press: 1000ms");
    applyItemStyle(longpress_item);
}

void LvglMenuView::createDisplayPage() {
    lv_obj_t* section = lv_menu_section_create(display_page_);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(section, 8, 0);

    // Brightness
    lv_obj_t* brightness_item = lv_menu_cont_create(section);
    lv_obj_t* brightness_label = lv_label_create(brightness_item);
    lv_label_set_text(brightness_label, "Brightness");
    lv_obj_t* brightness_slider = lv_slider_create(brightness_item);
    lv_obj_set_size(brightness_slider, 120, 10);
    lv_obj_align(brightness_slider, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_slider_set_value(brightness_slider, 80, LV_ANIM_OFF);
    applyItemStyle(brightness_item);

    // Auto Sleep
    lv_obj_t* sleep_item = lv_menu_cont_create(section);
    lv_obj_t* sleep_label = lv_label_create(sleep_item);
    lv_label_set_text(sleep_label, "Auto Sleep");
    lv_obj_t* sleep_sw = lv_switch_create(sleep_item);
    lv_obj_align(sleep_sw, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_state(sleep_sw, LV_STATE_CHECKED);
    applyItemStyle(sleep_item);

    // Sleep Time
    lv_obj_t* sleep_time_item = lv_menu_cont_create(section);
    lv_obj_t* sleep_time_label = lv_label_create(sleep_time_item);
    lv_label_set_text(sleep_time_label, "Sleep Time: 5 min");
    applyItemStyle(sleep_time_item);
}

void LvglMenuView::createAboutPage() {
    lv_obj_t* section = lv_menu_section_create(about_page_);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(section, 8, 0);

    // Device Info
    lv_obj_t* device_item = lv_menu_cont_create(section);
    lv_obj_t* device_label = lv_label_create(device_item);
    lv_label_set_text(device_label, "Device: MIDI Controller");
    applyItemStyle(device_item);

    // Firmware Version
    lv_obj_t* fw_item = lv_menu_cont_create(section);
    lv_obj_t* fw_label = lv_label_create(fw_item);
    lv_label_set_text(fw_label, "Firmware: v2.0.0");
    applyItemStyle(fw_item);

    // Hardware Version
    lv_obj_t* hw_item = lv_menu_cont_create(section);
    lv_obj_t* hw_label = lv_label_create(hw_item);
    lv_label_set_text(hw_label, "Hardware: Teensy 4.1");
    applyItemStyle(hw_item);

    // Memory Usage
    lv_obj_t* mem_item = lv_menu_cont_create(section);
    lv_obj_t* mem_label = lv_label_create(mem_item);
    lv_label_set_text(mem_label, "Memory: 45% used");
    applyItemStyle(mem_item);

    // Uptime
    lv_obj_t* uptime_item = lv_menu_cont_create(section);
    lv_obj_t* uptime_label = lv_label_create(uptime_item);
    lv_label_set_text(uptime_label, "Uptime: 02:30:45");
    applyItemStyle(uptime_item);
}

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