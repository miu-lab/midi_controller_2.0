#include "LvglModalView.hpp"
#include "config/DisplayConfig.hpp"

LvglModalView::LvglModalView(std::shared_ptr<Ili9341LvglBridge> bridge)
    :  bridge_(bridge),
      initialized_(false), active_(false), message_(""),
      modal_screen_(nullptr), bg_overlay_(nullptr), 
      message_box_(nullptr), message_label_(nullptr) {
}

LvglModalView::~LvglModalView() {
    setActive(false);
    cleanupLvglObjects();
}

bool LvglModalView::init() {
    if (initialized_) {
        return true;
    }
    
    if (!bridge_) {
        return false;
    }
    
    setupModalScreen();
    
    initialized_ = true;
    return true;
}

void LvglModalView::update() {
    // Modal statique, pas de mise à jour nécessaire
}

void LvglModalView::render() {
    if (!active_ || !initialized_) {
        return;
    }
    
    if (modal_screen_) {
        lv_screen_load(modal_screen_);
    }
    
    lv_timer_handler();
    
    if (bridge_) {
        bridge_->refreshDisplay();
    }
}

bool LvglModalView::handleEvent(uint8_t eventType, int32_t data) {
    return false;
}

bool LvglModalView::isActive() const {
    return active_;
}

void LvglModalView::setActive(bool active) {
    active_ = active;
}

void LvglModalView::setMessage(const char* message) {
    message_ = String(message);
    updateMessage();
}

void LvglModalView::setupModalScreen() {
    // Créer l'écran modal
    modal_screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(modal_screen_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(modal_screen_, LV_OPA_50, 0);  // Semi-transparent
    
    // Overlay semi-transparent
    bg_overlay_ = lv_obj_create(modal_screen_);
    lv_obj_set_size(bg_overlay_, DisplayConfig::SCREEN_WIDTH, DisplayConfig::SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(bg_overlay_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(bg_overlay_, LV_OPA_50, 0);
    lv_obj_set_style_border_opa(bg_overlay_, LV_OPA_TRANSP, 0);
    
    // Boîte de message
    message_box_ = lv_obj_create(modal_screen_);
    lv_obj_set_size(message_box_, DisplayConfig::SCREEN_WIDTH - 40, 80);
    lv_obj_center(message_box_);
    lv_obj_set_style_bg_color(message_box_, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(message_box_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(message_box_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(message_box_, 2, 0);
    
    // Label du message
    message_label_ = lv_label_create(message_box_);
    lv_obj_center(message_label_);
    lv_obj_set_style_text_color(message_label_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(message_label_, &lv_font_montserrat_14, 0);
    lv_label_set_text(message_label_, "");
}

void LvglModalView::updateMessage() {
    if (message_label_) {
        lv_label_set_text(message_label_, message_.c_str());
    }
}

void LvglModalView::cleanupLvglObjects() {
    if (modal_screen_) {
        lv_obj_del(modal_screen_);
        modal_screen_ = nullptr;
        bg_overlay_ = nullptr;
        message_box_ = nullptr;
        message_label_ = nullptr;
    }
}