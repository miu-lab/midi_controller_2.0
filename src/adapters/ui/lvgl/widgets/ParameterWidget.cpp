#include "ParameterWidget.hpp"
#include "adapters/ui/lvgl/theme/UITheme.hpp"
#include "adapters/ui/lvgl/widgets/ButtonIndicator.hpp"
#include "core/utils/FlashStrings.hpp"



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
      name_label_(nullptr),
      button_indicator_(nullptr) {
    

    createLvglObjects();
    positionLabels();
    updateLabels();
    updateArcValue(false);

}

ParameterWidget::ParameterWidget(lv_obj_t* parent, 
                               uint16_t width, uint16_t height, uint16_t arc_size)
    : parent_(parent ? parent : lv_screen_active()),
      theme_(nullptr),
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
      name_label_(nullptr),
      button_indicator_(nullptr) {
    
    createLvglObjects();
    setupLegacyStyles();
    positionLabels();
    updateLabels();
    updateArcValue(false);
}

ParameterWidget::~ParameterWidget() {

    // LVGL supprime automatiquement les enfants quand on supprime le parent
    if (container_) {
        lv_obj_delete(container_);
        container_ = nullptr;
        arc_ = nullptr;
        cc_label_ = nullptr;
        channel_label_ = nullptr;
        name_label_ = nullptr;
    }

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

    // Labels simplifiés pour économie mémoire
    cc_label_ = nullptr;
    channel_label_ = nullptr;

    name_label_ = lv_label_create(container_);
    lv_label_set_text(name_label_, parameter_name_.c_str());
    
    // Pas de thème pour éviter les erreurs
    
}

void ParameterWidget::positionLabels() {
    // Utiliser un système de layout automatique LVGL au lieu de calculs manuels
    
    // Container avec padding automatique
    lv_obj_set_style_pad_all(container_, 5, 0);
    
    // Pas de header nécessaire avec les labels simplifiés
    // Économie mémoire: pas de container supplémentaire

    // Arc centré automatiquement
    lv_obj_center(arc_);

    // Nom paramètre en haut
    if (name_label_) {
        lv_obj_set_width(name_label_, lv_pct(100));
        lv_obj_align(name_label_, LV_ALIGN_TOP_MID, 0, 5);
        lv_obj_set_style_text_align(name_label_, LV_TEXT_ALIGN_CENTER, 0);
    }
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
}

void ParameterWidget::setValue(uint8_t value, bool animate) {
    if (current_value_ != value) {
        current_value_ = value;
        updateArcValue(animate);
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
    // Seulement name_label_ utilisé
    if (name_label_) {
        String display_name = parameter_name_ + " (CC" + String(cc_number_) + ")";
        lv_label_set_text(name_label_, display_name.c_str());
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

}

void ParameterWidget::setupLegacyStyles() {
    // Styles hardcodés pour le constructeur legacy
    
    // Style container - fond sombre et pas de bordure
    // lv_obj_set_style_bg_color(container_, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_bg_opa(container_, LV_OPA_0, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_radius(container_, 0, 0);
    
    // Style arc - vert néon
    lv_color_t arc_color = lv_color_hex(0x00FF88);
    lv_obj_set_style_arc_color(arc_, arc_color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, 4, LV_PART_MAIN);
    
    // Style labels - blanc
    lv_color_t text_color = lv_color_hex(0xFFFFFF);
    
    if (name_label_) {
        lv_obj_set_style_text_color(name_label_, text_color, 0);
        lv_obj_set_style_text_font(name_label_, &lv_font_montserrat_10, 0);
        lv_obj_set_style_text_align(name_label_, LV_TEXT_ALIGN_CENTER, 0);
    }
}

//=============================================================================
// Gestion ButtonIndicator
//=============================================================================

bool ParameterWidget::addButtonIndicator(lv_coord_t size) {
    // Ne pas créer un indicateur s'il existe déjà
    if (button_indicator_) {
        return true;
    }
    
    // Vérifier que le container existe
    if (!container_) {
        return false;
    }
    
    // CHANGEMENT CRUCIAL : Créer l'indicateur comme enfant de l'ARC au lieu du container
    if (theme_) {
        button_indicator_ = std::make_unique<ButtonIndicator>(arc_, *theme_, size);
    } else {
        button_indicator_ = std::make_unique<ButtonIndicator>(arc_, size);
    }
    
    // Vérifier que la création a réussi
    if (!button_indicator_) {
        return false;
    }
    
    // Positionner l'indicateur
    positionButtonIndicator();
    
    return true;
}

void ParameterWidget::removeButtonIndicator() {
    button_indicator_.reset();
}

bool ParameterWidget::hasButtonIndicator() const {
    return button_indicator_ != nullptr;
}

void ParameterWidget::setButtonState(bool pressed, bool animate) {
    if (button_indicator_) {
        ButtonIndicator::State state = pressed ? ButtonIndicator::State::PRESSED : ButtonIndicator::State::OFF;
        button_indicator_->setState(state, animate);
    }
}

ButtonIndicator* ParameterWidget::getButtonIndicator() const {
    return button_indicator_.get();
}

void ParameterWidget::positionButtonIndicator() {
    if (!button_indicator_ || !arc_) {
        return;
    }
    
    lv_obj_t* led_obj = button_indicator_->getLedObject();
    if (!led_obj) {
        return;
    }
    
    // S'assurer que l'indicateur est visible
    button_indicator_->setVisible(true);
    
    // Centrer la LED directement sur l'arc (parent)
    lv_obj_center(led_obj);
    
    // Mettre la LED au premier plan
    lv_obj_move_foreground(led_obj);
}

