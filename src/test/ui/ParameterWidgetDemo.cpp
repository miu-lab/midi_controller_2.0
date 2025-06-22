#include "ParameterWidgetDemo.hpp"
#include <Arduino.h>

ParameterWidgetDemo::ParameterWidgetDemo(std::shared_ptr<Ili9341LvglBridge> bridge)
    : bridge_(bridge), widget_(nullptr), auto_demo_active_(false), 
      last_update_time_(0), demo_value_(64), demo_direction_(1) {
}

ParameterWidgetDemo::~ParameterWidgetDemo() {
    stopAutoDemo();
    widget_.reset();
}

bool ParameterWidgetDemo::init() {
    if (!bridge_) {
        Serial.println(F("ParameterWidgetDemo: Bridge LVGL non disponible"));
        return false;
    }
    
    Serial.println(F("ParameterWidgetDemo: Initialisation..."));
    
    // Créer le widget avec configuration par défaut
    ParameterWidget::Config config;
    config.width = 240;
    config.height = 200;
    config.arc_size = 100;
    config.arc_color = lv_color_hex(0x00AA00);  // Vert pour la démo
    config.anim_duration = 300;
    config.enable_animations = true;
    
    try {
        widget_ = std::make_unique<ParameterWidget>(lv_scr_act(), config);
        
        // Configurer le paramètre de démo
        setupDemoParameter();
        
        // Forcer l'affichage
        bridge_->refreshDisplay();
        
        Serial.println(F("ParameterWidgetDemo: Initialisé avec succès"));
        return true;
        
    } catch (const std::exception& e) {
        Serial.print(F("ParameterWidgetDemo: Erreur d'initialisation - "));
        Serial.println(e.what());
        return false;
    } catch (...) {
        Serial.println(F("ParameterWidgetDemo: Erreur d'initialisation inconnue"));
        return false;
    }
}

void ParameterWidgetDemo::update() {
    if (!widget_ || !auto_demo_active_) {
        return;
    }
    
    unsigned long current_time = millis();
    if (current_time - last_update_time_ >= AUTO_DEMO_INTERVAL_MS) {
        last_update_time_ = current_time;
        
        // Animer la valeur de démo
        demo_value_ += demo_direction_;
        
        // Rebondir aux limites
        if (demo_value_ >= 127) {
            demo_value_ = 127;
            demo_direction_ = -1;
        } else if (demo_value_ <= 0) {
            demo_value_ = 0;
            demo_direction_ = 1;
        }
        
        // Mettre à jour le widget avec animation
        widget_->setValue(demo_value_, true);
        
        // Log périodique pour debug
        static uint8_t log_counter = 0;
        if (++log_counter >= 40) {  // Log toutes les 2 secondes (40 * 50ms)
            Serial.print(F("ParameterWidgetDemo: Valeur = "));
            Serial.print(demo_value_);
            Serial.print(F(" Direction = "));
            Serial.println(demo_direction_);
            log_counter = 0;
        }
    }
}

void ParameterWidgetDemo::simulateMidiParameter(uint8_t cc_number, uint8_t channel, uint8_t value, const String& parameter_name) {
    if (!widget_) {
        Serial.println(F("ParameterWidgetDemo: Widget non initialisé"));
        return;
    }
    
    Serial.print(F("ParameterWidgetDemo: Simulation CC"));
    Serial.print(cc_number);
    Serial.print(F(" Ch"));
    Serial.print(channel);
    Serial.print(F(" = "));
    Serial.print(value);
    Serial.print(F(" ("));
    Serial.print(parameter_name);
    Serial.println(F(")"));
    
    widget_->setParameter(cc_number, channel, value, parameter_name, true);
    
    // Forcer rafraîchissement
    if (bridge_) {
        bridge_->refreshDisplay();
    }
}

void ParameterWidgetDemo::updateParameterValue(uint8_t value) {
    if (!widget_) {
        return;
    }
    
    widget_->setValue(value, true);
}

void ParameterWidgetDemo::startAutoDemo() {
    if (!widget_) {
        Serial.println(F("ParameterWidgetDemo: Impossible de démarrer auto-démo, widget non initialisé"));
        return;
    }
    
    Serial.println(F("ParameterWidgetDemo: Démarrage auto-démo animation"));
    auto_demo_active_ = true;
    last_update_time_ = millis();
    demo_value_ = 64;  // Commencer au milieu
    demo_direction_ = 1;
}

void ParameterWidgetDemo::stopAutoDemo() {
    if (auto_demo_active_) {
        Serial.println(F("ParameterWidgetDemo: Arrêt auto-démo"));
        auto_demo_active_ = false;
    }
}

void ParameterWidgetDemo::setupDemoParameter() {
    if (!widget_) {
        return;
    }
    
    // Configurer avec un paramètre de synthé classique
    widget_->setParameter(DEMO_CC_NUMBER, DEMO_CHANNEL, demo_value_, DEMO_PARAM_NAME, false);
    widget_->show();
    
    Serial.println(F("ParameterWidgetDemo: Paramètre de démo configuré"));
}