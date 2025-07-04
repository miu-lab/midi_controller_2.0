/**
 * @file Ili9341LvglBridge.cpp
 * @brief Implementation of LVGL bridge for ILI9341 display driver
 * 
 * This file provides a minimal bridge between LVGL and the hardware display driver.
 * It handles LVGL initialization, display setup, and memory management with optimized
 * buffer configuration for real-time performance.
 */

#include "Ili9341LvglBridge.hpp"

#include <Arduino.h>
#include <lvgl.h>

#include "config/DisplayConfig.hpp"


// Static DMAMEM buffers pour performance optimale - Taille selon orientation configurée
DMAMEM static lv_color_t lvgl_buffer_1[DisplayConfig::LVGL_BUFFER_SIZE];
DMAMEM static lv_color_t lvgl_buffer_2[DisplayConfig::LVGL_BUFFER_SIZE];

// Instance statique pour callbacks
static Ili9341LvglBridge* bridge_instance_ = nullptr;

Ili9341LvglBridge::Ili9341LvglBridge(std::shared_ptr<Ili9341Driver> driver,
                                     const LvglConfig& config)
    : config_(config), driver_(std::move(driver)), initialized_(false),
      display_(nullptr), lvgl_buf1_(nullptr), lvgl_buf2_(nullptr) {
    bridge_instance_ = this;
    // TODO DEBUG MSG
}

Ili9341LvglBridge::~Ili9341LvglBridge() {
    if (display_) {
        lv_display_delete(display_);
    }
    freeLvglBuffers();
    bridge_instance_ = nullptr;
    // TODO DEBUG MSG
}

bool Ili9341LvglBridge::initialize() {
    if (initialized_) {
        Serial.println("LvglBridge already initialized");
        return true;
    }
    
    if (!driver_) {
        Serial.println("ERROR: LvglBridge - Driver is null");
        return false;
    }
    
    Serial.println("LvglBridge: Starting initialization...");
    
    // Setup LVGL core
    Serial.println("LvglBridge: Setting up LVGL core...");
    if (!setupLvglCore()) {
        Serial.println("ERROR: LvglBridge - setupLvglCore failed");
        return false;
    }
    
    // Allouer buffers LVGL
    Serial.println("LvglBridge: Allocating LVGL buffers...");
    if (!allocateLvglBuffers()) {
        Serial.println("ERROR: LvglBridge - allocateLvglBuffers failed");
        return false;
    }
    
    // Setup display LVGL
    Serial.println("LvglBridge: Setting up LVGL display...");
    if (!setupLvglDisplay()) {
        Serial.println("ERROR: LvglBridge - setupLvglDisplay failed");
        freeLvglBuffers();
        return false;
    }
    
    initialized_ = true;
    Serial.println("LvglBridge: Initialization completed successfully");
    return true;
}

void Ili9341LvglBridge::refreshDisplay() {
    if (!initialized_) {
        return;
    }

    lv_timer_handler();
}

bool Ili9341LvglBridge::setupLvglCore() {
    lv_init();
    
    // Setup tick callback
    lv_tick_set_cb([]() -> uint32_t {
        return millis();
    });
    
    // TODO DEBUG MSG
    return true;
}

bool Ili9341LvglBridge::setupLvglDisplay() {
    // Créer display LVGL v9 - Dimensions selon configuration centralisée
    display_ = lv_display_create(DisplayConfig::SCREEN_WIDTH, DisplayConfig::SCREEN_HEIGHT);
    if (!display_) {
        // TODO DEBUG MSG
        return false;
    }
    
    // Configurer les buffers
    lv_display_set_buffers(display_,
                           lvgl_buf1_,
                           lvgl_buf2_,
                           DisplayConfig::LVGL_BUFFER_SIZE * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Définir callback flush
    lv_display_set_flush_cb(display_, flush_callback);
    
    // Stocker référence dans user_data pour callback
    lv_display_set_user_data(display_, this);

    // TODO DEBUG MSG
    return true;
}

bool Ili9341LvglBridge::allocateLvglBuffers() {
    // Utiliser les buffers statiques DMAMEM
    lvgl_buf1_ = lvgl_buffer_1;
    lvgl_buf2_ = config_.double_buffering ? lvgl_buffer_2 : nullptr;
    
    // TODO DEBUG MSG
    return true;
}

void Ili9341LvglBridge::freeLvglBuffers() {
    // Buffers statiques, pas de libération nécessaire
    lvgl_buf1_ = nullptr;
    lvgl_buf2_ = nullptr;
}

Ili9341LvglBridge::LvglConfig Ili9341LvglBridge::getDefaultLvglConfig() {
    return {.buffer_lines = DisplayConfig::LVGL_BUFFER_LINES,  ///< Lignes dans buffer LVGL
            .double_buffering = true};
}

Ili9341LvglBridge* Ili9341LvglBridge::getInstance(lv_display_t* disp) {
    return static_cast<Ili9341LvglBridge*>(lv_display_get_user_data(disp));
}

// Static flush callback pour LVGL v9
void Ili9341LvglBridge::flush_callback(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
    auto* bridge = getInstance(disp);
    if (!bridge || !bridge->driver_) {
        lv_display_flush_ready(disp);
        return;
    }

    // Mesurer le temps de flush pour profiling
    unsigned long flush_start = micros();

    // Conversion coordinates LVGL vers hardware
    int x1 = area->x1;
    int y1 = area->y1;
    int x2 = area->x2;
    int y2 = area->y2;
    
    // Appel driver hardware
    bridge->driver_->updateRegion(true, reinterpret_cast<uint16_t*>(px_map), x1, x2, y1, y2);

    // Mesurer et enregistrer le temps de flush
    unsigned long flush_end = micros();
    unsigned long flush_duration = flush_end - flush_start;

    bridge->flush_call_count_++;
    bridge->total_flush_time_ += flush_duration;
    bridge->total_flush_calls_++;
    bridge->total_flush_time_period_ += flush_duration;

    // Signal LVGL que le flush est terminé
    lv_display_flush_ready(disp);
}