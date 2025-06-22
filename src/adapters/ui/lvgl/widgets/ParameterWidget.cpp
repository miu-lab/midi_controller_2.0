#include "ParameterWidget.hpp"
#include "core/utils/FlashStrings.hpp"

//=============================================================================
// Configuration par défaut
//=============================================================================

ParameterWidget::Config ParameterWidget::getDefaultConfig() {
    return {.width = 240,
            .height = 120,
            .arc_size = 80,
            .arc_width = 6,
            .arc_color = lv_color_hex(0x0080FF),
            .arc_bg_color = lv_color_hex(0x404040),
            .show_cc_number = true,
            .show_channel = true,
            .show_value = true};
}

//=============================================================================
// Constructeur et destructeur
//=============================================================================

ParameterWidget::ParameterWidget(lv_obj_t* parent, const Config& config)
    : config_(config),
      current_value_(0),
      cc_number_(0),
      channel_(1),
      parameter_name_("PARAM"),
      container_(nullptr),
      arc_(nullptr),
      cc_label_(nullptr),
      channel_label_(nullptr),
      value_label_(nullptr),
      name_label_(nullptr),
      is_animating_(false),
      value_changed_callback_(nullptr) {
    
    Serial.println(F("ParameterWidget: Creating widget"));
    
    createLvglObjects();
    setupStyles();
    updateLabels();
    
    Serial.println(F("ParameterWidget: Widget created successfully"));
}

ParameterWidget::~ParameterWidget() {
    Serial.println(F("ParameterWidget: Destroying widget"));
    
    // Arrêter animation si en cours
    if (is_animating_) {
        lv_anim_delete(arc_, anim_arc_exec_cb);
    }
    
    // LVGL se charge de supprimer automatiquement les enfants
    if (container_) {
        lv_obj_delete(container_);
    }
}

//=============================================================================
// Création objets LVGL
//=============================================================================

void ParameterWidget::createLvglObjects() {
    // Container principal
    container_ = lv_obj_create(lv_screen_active());
    lv_obj_set_size(container_, config_.width, config_.height);
    lv_obj_set_user_data(container_, this); // Pour retrouver l'instance dans callbacks
    
    // Arc principal (centre du container)
    arc_ = lv_arc_create(container_);
    lv_obj_set_size(arc_, config_.arc_size, config_.arc_size);
    lv_obj_center(arc_);
    lv_obj_set_user_data(arc_, this);
    
    // Configuration arc
    lv_arc_set_range(arc_, 0, 1000); // 0-1000 pour plus de précision que 0-127
    lv_arc_set_value(arc_, 0);
    lv_arc_set_bg_angles(arc_, 0, 360); // Arc complet
    lv_arc_set_angles(arc_, 270, 270); // Start à 12h (270°)
    
    // Événements arc pour interaction utilisateur
    lv_obj_add_event_cb(arc_, arc_event_cb, LV_EVENT_VALUE_CHANGED, this);
    
    // Labels - positionnés selon le design de ParameterFocusView
    if (config_.show_cc_number) {
        cc_label_ = lv_label_create(container_);
        lv_obj_set_pos(cc_label_, 5, 5); // Haut gauche
        lv_label_set_text(cc_label_, "CC 00");
    }
    
    if (config_.show_channel) {
        channel_label_ = lv_label_create(container_);
        lv_obj_set_pos(channel_label_, config_.width - 40, 5); // Haut droite
        lv_label_set_text(channel_label_, "CH 01");
    }
    
    if (config_.show_value) {
        value_label_ = lv_label_create(container_);
        lv_obj_center(value_label_); // Centre de l'arc
        lv_label_set_text(value_label_, "0");
    }
    
    // Nom paramètre (bas centre)
    name_label_ = lv_label_create(container_);
    lv_obj_set_pos(name_label_, 0, config_.height - 25);
    lv_obj_set_width(name_label_, config_.width);
    lv_obj_set_style_text_align(name_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(name_label_, parameter_name_.c_str());
    
    Serial.println(F("ParameterWidget: LVGL objects created"));
}

void ParameterWidget::setupStyles() {
    // Style container - transparent
    lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_all(container_, 0, 0);
    
    // Style arc - couleurs configurables
    lv_obj_set_style_arc_color(arc_, config_.arc_color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_, config_.arc_bg_color, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, config_.arc_width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_, config_.arc_width, LV_PART_MAIN);
    
    // Knob invisible (on veut juste l'arc)
    lv_obj_set_style_bg_opa(arc_, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_border_width(arc_, 0, LV_PART_KNOB);
    lv_obj_set_style_pad_all(arc_, 0, LV_PART_KNOB);
    
    // Style labels - police et couleur
    if (cc_label_) {
        lv_obj_set_style_text_color(cc_label_, lv_color_white(), 0);
        lv_obj_set_style_text_font(cc_label_, &lv_font_montserrat_12, 0);
    }
    
    if (channel_label_) {
        lv_obj_set_style_text_color(channel_label_, lv_color_white(), 0);
        lv_obj_set_style_text_font(channel_label_, &lv_font_montserrat_12, 0);
    }
    
    if (value_label_) {
        lv_obj_set_style_text_color(value_label_, lv_color_white(), 0);
        lv_obj_set_style_text_font(value_label_, &lv_font_montserrat_16, 0);
    }
    
    if (name_label_) {
        lv_obj_set_style_text_color(name_label_, lv_color_white(), 0);
        lv_obj_set_style_text_font(name_label_, &lv_font_montserrat_14, 0);
    }
    
    Serial.println(F("ParameterWidget: Styles configured"));
}

//=============================================================================
// Interface publique
//=============================================================================

void ParameterWidget::setParameter(uint8_t cc_number, uint8_t channel, uint8_t value, 
                                  const String& parameter_name, bool animate) {
    cc_number_ = cc_number;
    channel_ = channel;
    current_value_ = value;
    parameter_name_ = parameter_name;
    
    updateLabels();
    updateArcValue(animate);
    
    Serial.print(F("ParameterWidget: Parameter set - CC"));
    Serial.print(cc_number_);
    Serial.print(F(" CH"));
    Serial.print(channel_);
    Serial.print(F(" Value:"));
    Serial.print(current_value_);
    Serial.print(F(" Name:"));
    Serial.println(parameter_name_);
}

void ParameterWidget::setValue(uint8_t value, bool animate) {
    if (current_value_ != value) {
        current_value_ = value;
        updateArcValue(animate);
        
        // Mettre à jour label valeur
        if (value_label_) {
            char value_text[8];
            snprintf(value_text, sizeof(value_text), "%d", current_value_);
            lv_label_set_text(value_label_, value_text);
        }
        
        Serial.print(F("ParameterWidget: Value updated to "));
        Serial.println(current_value_);
    }
}

void ParameterWidget::setParameterName(const String& parameter_name) {
    parameter_name_ = parameter_name;
    if (name_label_) {
        lv_label_set_text(name_label_, parameter_name_.c_str());
    }
}

void ParameterWidget::setVisible(bool visible) {
    if (container_) {
        if (visible) {
            lv_obj_clear_flag(container_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(container_, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

bool ParameterWidget::isVisible() const {
    if (container_) {
        return !lv_obj_has_flag(container_, LV_OBJ_FLAG_HIDDEN);
    }
    return false;
}

void ParameterWidget::setPosition(lv_coord_t x, lv_coord_t y) {
    if (container_) {
        lv_obj_set_pos(container_, x, y);
    }
}

//=============================================================================
// Méthodes privées
//=============================================================================

void ParameterWidget::updateLabels() {
    // CC Label
    if (cc_label_) {
        char cc_text[8];
        snprintf(cc_text, sizeof(cc_text), "CC %02d", cc_number_);
        lv_label_set_text(cc_label_, cc_text);
    }
    
    // Channel Label  
    if (channel_label_) {
        char ch_text[8];
        snprintf(ch_text, sizeof(ch_text), "CH %02d", channel_);
        lv_label_set_text(channel_label_, ch_text);
    }
    
    // Value Label
    if (value_label_) {
        char value_text[8];
        snprintf(value_text, sizeof(value_text), "%d", current_value_);
        lv_label_set_text(value_label_, value_text);
    }
    
    // Name Label
    if (name_label_) {
        lv_label_set_text(name_label_, parameter_name_.c_str());
    }
}

void ParameterWidget::updateArcValue(bool animate) {
    if (!arc_) return;

    int16_t arc_value = midiToArcValue(current_value_);
    // Mise à jour immédiate
    lv_arc_set_value(arc_, arc_value);

    Serial.print(F("ParameterWidget: Immediate arc update to "));
    Serial.println(arc_value);
}

//=============================================================================
// Utilitaires conversion
//=============================================================================

int16_t ParameterWidget::midiToArcValue(uint8_t midi_value) {
    // Convertir 0-127 vers 0-1000 pour plus de précision
    return (int16_t)((uint32_t)midi_value * 1000 / 127);
}

uint8_t ParameterWidget::arcToMidiValue(int16_t arc_value) {
    // Convertir 0-1000 vers 0-127
    return (uint8_t)((uint32_t)arc_value * 127 / 1000);
}

//=============================================================================
// Callbacks LVGL statiques
//=============================================================================

void ParameterWidget::arc_event_cb(lv_event_t* e) {
    lv_obj_t* arc = static_cast<lv_obj_t*>(lv_event_get_target(e));
    ParameterWidget* widget = getInstanceFromObj(arc);
    
    if (!widget) {
        Serial.println(F("ParameterWidget: ERROR - No widget instance in arc callback"));
        return;
    }
    
    // Obtenir nouvelle valeur depuis l'arc
    int16_t arc_value = lv_arc_get_value(arc);
    uint8_t new_midi_value = arcToMidiValue(arc_value);
    
    // Éviter les callbacks en boucle
    if (new_midi_value != widget->current_value_) {
        widget->current_value_ = new_midi_value;
        
        // Mettre à jour label valeur
        if (widget->value_label_) {
            char value_text[8];
            snprintf(value_text, sizeof(value_text), "%d", widget->current_value_);
            lv_label_set_text(widget->value_label_, value_text);
        }
        
        // Callback application si configuré
        if (widget->value_changed_callback_) {
            widget->value_changed_callback_(widget->current_value_);
        }
        
        Serial.print(F("ParameterWidget: User changed value to "));
        Serial.println(widget->current_value_);
    }
}

void ParameterWidget::anim_ready_cb(lv_anim_t* a) {
    ParameterWidget* widget = static_cast<ParameterWidget*>(lv_anim_get_user_data(a));
    if (widget) {
        widget->is_animating_ = false;
        Serial.println(F("ParameterWidget: Animation completed"));
    }
}

void ParameterWidget::anim_arc_exec_cb(void* obj, int32_t value) {
    lv_obj_t* arc = static_cast<lv_obj_t*>(obj);
    lv_arc_set_value(arc, value);
}

ParameterWidget* ParameterWidget::getInstanceFromObj(lv_obj_t* obj) {
    return static_cast<ParameterWidget*>(lv_obj_get_user_data(obj));
}