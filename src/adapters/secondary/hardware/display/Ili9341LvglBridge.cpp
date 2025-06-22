/**
 * @file Ili9341LvglBridge.cpp
 * @brief Implementation of LVGL bridge for ILI9341 display driver
 * 
 * This file provides a minimal bridge between LVGL and the hardware display driver.
 * It handles LVGL initialization, display setup, and memory management with optimized
 * buffer configuration for real-time performance.
 */

#include "Ili9341LvglBridge.hpp"
#include <lvgl.h>
#include <Arduino.h>

// Static DMAMEM buffers pour performance optimale (320 * 60 lignes paysage)
static constexpr size_t BUFFER_WIDTH = 320;
static constexpr size_t BUFFER_HEIGHT = 60;
static constexpr size_t BUFFER_SIZE = BUFFER_WIDTH * BUFFER_HEIGHT;

DMAMEM static lv_color_t lvgl_buffer_1[BUFFER_SIZE];
DMAMEM static lv_color_t lvgl_buffer_2[BUFFER_SIZE];

// Instance statique pour callbacks
static Ili9341LvglBridge* bridge_instance_ = nullptr;

Ili9341LvglBridge::Ili9341LvglBridge(std::shared_ptr<Ili9341Driver> driver,
                                     const LvglConfig& config)
    : config_(config), driver_(std::move(driver)), initialized_(false),
      display_(nullptr), lvgl_buf1_(nullptr), lvgl_buf2_(nullptr) {
    bridge_instance_ = this;
    Serial.println(F("Ili9341LvglBridge: Constructor called"));
}

Ili9341LvglBridge::~Ili9341LvglBridge() {
    if (display_) {
        lv_display_delete(display_);
    }
    freeLvglBuffers();
    bridge_instance_ = nullptr;
    Serial.println(F("Ili9341LvglBridge: Destructor called"));
}

bool Ili9341LvglBridge::initialize() {
    if (initialized_) {
        return true;
    }
    
    if (!driver_) {
        Serial.println(F("Bridge: No hardware driver"));
        return false;
    }
    
    Serial.println(F("Bridge: Initializing LVGL..."));
    
    // Setup LVGL core
    if (!setupLvglCore()) {
        Serial.println(F("Bridge: LVGL core setup failed"));
        return false;
    }
    
    // Allouer buffers LVGL
    if (!allocateLvglBuffers()) {
        Serial.println(F("Bridge: Buffer allocation failed"));
        return false;
    }
    
    // Setup display LVGL
    if (!setupLvglDisplay()) {
        Serial.println(F("Bridge: Display setup failed"));
        freeLvglBuffers();
        return false;
    }
    
    initialized_ = true;
    Serial.println(F("Bridge: LVGL initialized successfully"));
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
    
    Serial.println(F("Bridge: LVGL core configured"));
    return true;
}

bool Ili9341LvglBridge::setupLvglDisplay() {
    // Créer display LVGL v9
    display_ = lv_display_create(320, 240);
    if (!display_) {
        Serial.println(F("Bridge: Failed to create display"));
        return false;
    }
    
    // Configurer les buffers
    lv_display_set_buffers(display_, lvgl_buf1_, lvgl_buf2_, 
                          BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    // Définir callback flush
    lv_display_set_flush_cb(display_, flush_callback);
    
    // Stocker référence dans user_data pour callback
    lv_display_set_user_data(display_, this);
    
    Serial.println(F("Bridge: Display configured"));
    return true;
}

bool Ili9341LvglBridge::allocateLvglBuffers() {
    // Utiliser les buffers statiques DMAMEM
    lvgl_buf1_ = lvgl_buffer_1;
    lvgl_buf2_ = config_.double_buffering ? lvgl_buffer_2 : nullptr;
    
    Serial.print(F("Bridge: Allocated buffers - Size: "));
    Serial.print(BUFFER_SIZE * sizeof(lv_color_t));
    Serial.println(F(" bytes"));
    return true;
}

void Ili9341LvglBridge::freeLvglBuffers() {
    // Buffers statiques, pas de libération nécessaire
    lvgl_buf1_ = nullptr;
    lvgl_buf2_ = nullptr;
}

Ili9341LvglBridge::LvglConfig Ili9341LvglBridge::getDefaultLvglConfig() {
    return {
        .buffer_lines = 60,
        .double_buffering = true,
        .lvgl_memory_kb = 48
    };
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
    
    // Conversion coordinates LVGL vers hardware
    int x1 = area->x1;
    int y1 = area->y1;
    int x2 = area->x2;
    int y2 = area->y2;
    
    // Appel driver hardware
    bridge->driver_->updateRegion(true, reinterpret_cast<uint16_t*>(px_map), x1, x2, y1, y2);
    
    // Signal LVGL que le flush est terminé
    lv_display_flush_ready(disp);
}