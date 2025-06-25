#include "ParameterWidget.hpp"
#include "adapters/ui/lvgl/theme/UITheme.hpp"
#include "core/utils/FlashStrings.hpp"
#include "config/debug/DebugMacros.hpp"


//=============================================================================
// Constructeur et destructeur
//=============================================================================

ParameterWidget::ParameterWidget(lv_obj_t* parent, const UITheme& theme, 
                               uint16_t width, uint16_t height, uint16_t arc_size)
    : parent_(parent ? parent : lv_screen_active()),
      theme_(&theme),
      width_(width),
      height_(height),
      arc_size_(arc_size),
      current_value_(0),
      cc_number_(0),
      channel_(1),
      parameter_name_("PARAM"),
      container_(nullptr),
      arc_(nullptr),
      cc_label_(nullptr),
      channel_label_(nullptr),
      value_label_(nullptr),
      name_label_(nullptr) {
    
    DEBUG_UI("ParameterWidget: Creating widget with UITheme");

    createLvglObjects();
    positionLabels();
    updateLabels();
    updateArcValue(false);

    DEBUG_UI("ParameterWidget: Widget with UITheme created successfully");
}

ParameterWidget::ParameterWidget(lv_obj_t* parent)
    : parent_(parent ? parent : lv_screen_active()),
      theme_(nullptr),
      width_(96),
      height_(84),
      arc_size_(64),
      current_value_(0),
      cc_number_(0),
      channel_(1),
      parameter_name_("PARAM"),
      container_(nullptr),
      arc_(nullptr),
      cc_label_(nullptr),
      channel_label_(nullptr),
      value_label_(nullptr),
      name_label_(nullptr) {
    DEBUG_UI("ParameterWidget: Creating widget (legacy mode)");

    createLvglObjects();
    setupLegacyStyles();
    positionLabels();
    updateLabels();
    updateArcValue(false);

    DEBUG_UI("ParameterWidget: Widget created successfully");
}

ParameterWidget::~ParameterWidget() {
    DEBUG_UI("ParameterWidget: Destroying widget");

    // LVGL supprime automatiquement les enfants quand on supprime le parent
    if (container_) {
        lv_obj_delete(container_);
        container_ = nullptr;
        arc_ = nullptr;
        cc_label_ = nullptr;
        channel_label_ = nullptr;
        value_label_ = nullptr;
        name_label_ = nullptr;
    }

    DEBUG_UI("ParameterWidget: Widget destroyed");
}

//=============================================================================
// Création objets LVGL
//=============================================================================

void ParameterWidget::createLvglObjects() {
    // Container principal
    container_ = lv_obj_create(parent_);
    lv_obj_set_size(container_, width_, height_);
    lv_obj_set_user_data(container_, this);

    // Arc principal - utilise le widget natif lv_arc
    arc_ = lv_arc_create(container_);
    lv_obj_set_size(arc_, arc_size_, arc_size_);
    lv_obj_center(arc_);
    lv_obj_set_user_data(arc_, this);

    // Configuration de l'arc natif LVGL
    lv_arc_set_range(arc_, 0, 127);  // Range MIDI directe 0-127
    lv_arc_set_value(arc_, 0);
    lv_arc_set_bg_angles(arc_, 135, 45);  // 7h30 à 1h30
    lv_arc_set_angles(arc_, 135, 135);    // Démarrer au minimum

    // Labels toujours créés (simplification)
    cc_label_ = lv_label_create(container_);
    lv_label_set_text(cc_label_, "CC 00");
    
    channel_label_ = lv_label_create(container_);
    lv_label_set_text(channel_label_, "CH 01");
    
    value_label_ = lv_label_create(container_);
    lv_label_set_text(value_label_, "0");

    name_label_ = lv_label_create(container_);
    lv_label_set_text(name_label_, parameter_name_.c_str());
    
    // Appliquer le style approprié
    if (theme_) {
        // Nouveau style via UITheme
        theme_->applyParameterWidgetStyle(container_, arc_, value_label_, 
                                         cc_label_, channel_label_, name_label_);
    }
    
    DEBUG_UI("ParameterWidget: LVGL objects created");
}


void ParameterWidget::setupLegacyStyles() {
    // Styles hardcodés pour le constructeur legacy
    
    // Style container - fond et pas de bordure
    lv_obj_set_style_bg_color(container_, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_bg_opa(container_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_all(container_, 5, 0);

    // Style arc - utiliser des couleurs par défaut
    lv_obj_set_style_arc_color(arc_, lv_color_hex(0x00AA00), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_, 6, LV_PART_MAIN);

    // Désactiver l'interaction pour legacy (read-only)
    lv_obj_remove_flag(arc_, LV_OBJ_FLAG_CLICKABLE);

    // Styles des labels
    const lv_font_t* font_small = &lv_font_montserrat_12;
    const lv_font_t* font_medium = &lv_font_montserrat_14;
    const lv_font_t* font_large = &lv_font_montserrat_16;
    lv_color_t text_color = lv_color_hex(0xFFFFFF);

    if (cc_label_) {
        lv_obj_set_style_text_color(cc_label_, text_color, 0);
        lv_obj_set_style_text_font(cc_label_, font_small, 0);
    }
    
    if (channel_label_) {
        lv_obj_set_style_text_color(channel_label_, text_color, 0);
        lv_obj_set_style_text_font(channel_label_, font_small, 0);
    }
    
    if (value_label_) {
        lv_obj_set_style_text_color(value_label_, text_color, 0);
        lv_obj_set_style_text_font(value_label_, font_large, 0);
        lv_obj_set_style_text_align(value_label_, LV_TEXT_ALIGN_CENTER, 0);
    }
    
    if (name_label_) {
        lv_obj_set_style_text_color(name_label_, text_color, 0);
        lv_obj_set_style_text_font(name_label_, font_medium, 0);
        lv_obj_set_style_text_align(name_label_, LV_TEXT_ALIGN_CENTER, 0);
    }
    
    DEBUG_UI("ParameterWidget: Legacy styles configured");
}

void ParameterWidget::positionLabels() {
    // Utiliser un système de layout automatique LVGL au lieu de calculs manuels
    
    // Container avec padding automatique
    lv_obj_set_style_pad_all(container_, 5, 0);
    
    // Header avec CC et Channel - utiliser flex horizontal
    lv_obj_t* header = lv_obj_create(container_);
    lv_obj_set_size(header, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_layout(header, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(header, 0, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    
    // Déplacer les labels dans le header
    lv_obj_set_parent(cc_label_, header);
    lv_obj_set_parent(channel_label_, header);

    // Arc centré automatiquement
    lv_obj_center(arc_);

    // Valeur au centre de l'arc (pas de changement nécessaire)
    lv_obj_align_to(value_label_, arc_, LV_ALIGN_CENTER, 0, 0);

    // Nom paramètre en bas avec largeur automatique
    lv_obj_set_width(name_label_, lv_pct(100));
    lv_obj_align(name_label_, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_text_align(name_label_, LV_TEXT_ALIGN_CENTER, 0);

    DEBUG_UI("ParameterWidget: Labels positioned with native LVGL layout");
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

    DEBUG_UI("ParameterWidget: Parameter set - CC%d CH%d Value:%d Name:%s",
             cc_number_, channel_, current_value_, parameter_name_.c_str());
}

void ParameterWidget::setValue(uint8_t value, bool animate) {
    if (current_value_ != value) {
        current_value_ = value;
        
        // Mise à jour directe (throttling géré par EventBatcher)
        updateArcValue(animate);
        
        // Mettre à jour label valeur
        if (value_label_) {
            char value_text[8];
            snprintf(value_text, sizeof(value_text), "%d", current_value_);
            lv_label_set_text(value_label_, value_text);
        }
        
        DEBUG_UI("ParameterWidget: Value updated to %d", current_value_);
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

void ParameterWidget::setSize(lv_coord_t width, lv_coord_t height) {
    if (container_) {
        width_ = width;
        height_ = height;
        lv_obj_set_size(container_, width, height);
        positionLabels();  // Repositionner les labels
    }
}

void ParameterWidget::center() {
    if (container_) {
        lv_obj_center(container_);
    }
}

void ParameterWidget::setInteractionEnabled(bool enable) {
    if (arc_) {
        if (enable) {
            lv_obj_add_flag(arc_, LV_OBJ_FLAG_CLICKABLE);
            // Remettre le style du knob avec une couleur par défaut
            if (theme_) {
                lv_obj_set_style_bg_color(arc_, theme_->colors.midi_cc, LV_PART_KNOB);
            } else {
                lv_obj_set_style_bg_color(arc_, lv_color_hex(0x00AA00), LV_PART_KNOB);
            }
            lv_obj_set_style_bg_opa(arc_, LV_OPA_COVER, LV_PART_KNOB);
        } else {
            lv_obj_remove_style(arc_, nullptr, LV_PART_KNOB);
            lv_obj_remove_flag(arc_, LV_OBJ_FLAG_CLICKABLE);
        }
    }
}

void ParameterWidget::processPendingUpdates() {
    // Plus besoin de pending updates avec EventBatcher
    // Méthode conservée pour compatibilité mais vide
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

    // Vérifier si la valeur a réellement changé pour éviter redraws inutiles
    int32_t current_arc_value = lv_arc_get_value(arc_);
    if (current_arc_value == current_value_) {
        return; // Pas de changement, éviter le redraw
    }

    // Utiliser directement la valeur MIDI (0-127) car l'arc est configuré avec cette plage
    lv_arc_set_value(arc_, current_value_);
    
    // Invalider seulement la zone de l'arc pour optimiser le redraw
    lv_obj_invalidate(arc_);

    DEBUG_UI("ParameterWidget: Arc value updated to %d (animate: %s)",
             current_value_, animate ? "true" : "false");
}

