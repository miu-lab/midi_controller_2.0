#include "MenuPageBuilder.hpp"

// ====================
// MENUPAGE BUILDER
// ====================

MenuPageBuilder::MenuPageBuilder(lv_obj_t* menu) 
    : menu_(menu) {
}

lv_obj_t* MenuPageBuilder::createPage(const char* title) {
    lv_obj_t* page = lv_menu_page_create(menu_, title);
    configurePageDefaults(page);
    return page;
}

lv_obj_t* MenuPageBuilder::createSection(lv_obj_t* page) {
    lv_obj_t* section = lv_menu_section_create(page);
    configureSectionDefaults(section);
    return section;
}

lv_obj_t* MenuPageBuilder::createLabelItem(lv_obj_t* section, const char* text) {
    lv_obj_t* item = createBaseItem(section);
    lv_obj_t* label = lv_label_create(item);
    lv_label_set_text(label, text);
    applyStandardItemStyle(item);
    return item;
}

lv_obj_t* MenuPageBuilder::createSwitchItem(lv_obj_t* section, const char* label, bool checked) {
    lv_obj_t* item = createBaseItem(section);
    
    // Label
    lv_obj_t* label_obj = lv_label_create(item);
    lv_label_set_text(label_obj, label);
    
    // Switch
    lv_obj_t* sw = lv_switch_create(item);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, SystemConstants::UI::MENU_PADDING, 0);
    
    if (checked) {
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    }
    
    applyStandardItemStyle(item);
    return item;
}

lv_obj_t* MenuPageBuilder::createSliderItem(lv_obj_t* section, const char* label, int value, int min, int max) {
    lv_obj_t* item = createBaseItem(section);
    
    // Label
    lv_obj_t* label_obj = lv_label_create(item);
    lv_label_set_text(label_obj, label);
    
    // Slider
    lv_obj_t* slider = lv_slider_create(item);
    lv_obj_set_size(slider, 100, 20);
    lv_obj_align(slider, LV_ALIGN_RIGHT_MID, SystemConstants::UI::MENU_PADDING, 0);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, value, LV_ANIM_OFF);
    
    applyStandardItemStyle(item);
    return item;
}

lv_obj_t* MenuPageBuilder::createNavigationItem(lv_obj_t* section, const char* label, lv_obj_t* target_page) {
    lv_obj_t* item = createBaseItem(section);
    lv_obj_t* label_obj = lv_label_create(item);
    lv_label_set_text(label_obj, label);
    
    // Configurer la navigation vers la page cible
    lv_menu_set_load_page_event(menu_, item, target_page);
    
    applyStandardItemStyle(item);
    return item;
}

void MenuPageBuilder::applyStandardItemStyle(lv_obj_t* item) {
    // Fond transparent pour les items
    lv_obj_set_style_bg_opa(item, LV_OPA_TRANSP, 0);

    // Style focus automatique via LVGL - Bordure intense
    lv_obj_set_style_border_side(item, LV_BORDER_SIDE_LEFT, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(item, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(item, lv_color_hex(SystemConstants::UI::COLOR_GREEN_NEON), LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(item, LV_OPA_COVER, LV_STATE_FOCUSED);

    makeFocusable(item);
}

void MenuPageBuilder::makeFocusable(lv_obj_t* item) {
    lv_group_t* group = lv_group_get_default();
    if (group) {
        lv_group_add_obj(group, item);
    }
}

lv_obj_t* MenuPageBuilder::createBaseItem(lv_obj_t* section) {
    return lv_menu_cont_create(section);
}

void MenuPageBuilder::configureSectionDefaults(lv_obj_t* section) {
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(section, SystemConstants::UI::MENU_PADDING, 0);
    lv_obj_set_style_pad_hor(section, SystemConstants::UI::MENU_PADDING, 0);
}

void MenuPageBuilder::configurePageDefaults(lv_obj_t* page) {
    lv_obj_set_style_bg_opa(page, LV_OPA_TRANSP, 0);
}

// ====================
// MENUPAGE FACTORY
// ====================

MenuPageFactory::MenuPageFactory(MenuPageBuilder& builder) 
    : builder_(builder) {
}

lv_obj_t* MenuPageFactory::createWiFiPage(lv_obj_t* parent_page) {
    lv_obj_t* page = builder_.createPage(SystemConstants::Labels::WIFI_SETTINGS);
    lv_obj_t* section = builder_.createSection(page);

    // WiFi Enable Switch
    builder_.createSwitchItem(section, "Enable WiFi", false);
    
    // Network SSID
    builder_.createLabelItem(section, "Network: MyWiFi");
    
    // Signal Strength
    builder_.createLabelItem(section, "Signal: Strong");
    
    return page;
}

lv_obj_t* MenuPageFactory::createBluetoothPage(lv_obj_t* parent_page) {
    lv_obj_t* page = builder_.createPage(SystemConstants::Labels::BLUETOOTH_SETTINGS);
    lv_obj_t* section = builder_.createSection(page);

    // Bluetooth Enable Switch
    builder_.createSwitchItem(section, "Enable Bluetooth", true);
    
    // Paired Devices
    builder_.createLabelItem(section, "Paired Devices: 2");
    
    // Discoverable
    builder_.createSwitchItem(section, "Discoverable", false);
    
    return page;
}

lv_obj_t* MenuPageFactory::createAudioPage(lv_obj_t* parent_page) {
    lv_obj_t* page = builder_.createPage(SystemConstants::Labels::AUDIO_SETTINGS);
    lv_obj_t* section = builder_.createSection(page);

    // Master Volume
    builder_.createSliderItem(section, "Master Volume", SystemConstants::Audio::VOLUME_DEFAULT);
    
    // Audio Quality
    builder_.createLabelItem(section, "Quality: High");
    
    // Sample Rate
    builder_.createLabelItem(section, "Sample Rate: 48kHz");
    
    return page;
}

lv_obj_t* MenuPageFactory::createInputPage(lv_obj_t* parent_page) {
    lv_obj_t* page = builder_.createPage("Input Settings");
    lv_obj_t* section = builder_.createSection(page);

    // Encoder Sensitivity
    builder_.createSliderItem(section, SystemConstants::Labels::ENCODER_SENSITIVITY, SystemConstants::Audio::AUDIO_SENSITIVITY_DEFAULT);
    
    // Button Debounce
    builder_.createLabelItem(section, "Button Debounce: 30ms");
    
    // Long Press Time
    builder_.createLabelItem(section, "Long Press: 1000ms");
    
    return page;
}

lv_obj_t* MenuPageFactory::createDisplayPage(lv_obj_t* parent_page) {
    lv_obj_t* page = builder_.createPage("Display Settings");
    lv_obj_t* section = builder_.createSection(page);

    // Brightness
    builder_.createSliderItem(section, "Brightness", 75);
    
    // Auto Sleep
    builder_.createSwitchItem(section, "Auto Sleep", true);
    
    // Sleep Time
    builder_.createLabelItem(section, "Sleep Time: 5 min");
    
    return page;
}

lv_obj_t* MenuPageFactory::createAboutPage(lv_obj_t* parent_page) {
    lv_obj_t* page = builder_.createPage("About");
    lv_obj_t* section = builder_.createSection(page);

    // Device Info
    builder_.createLabelItem(section, "MIDI Controller v2.0");
    
    // Firmware Version
    builder_.createLabelItem(section, "Firmware: v1.0.0");
    
    // Hardware Version
    builder_.createLabelItem(section, "Hardware: Teensy 4.1");
    
    // Memory Usage
    builder_.createLabelItem(section, "Memory: 45% used");
    
    // Uptime
    builder_.createLabelItem(section, "Uptime: 02:30:45");
    
    return page;
}