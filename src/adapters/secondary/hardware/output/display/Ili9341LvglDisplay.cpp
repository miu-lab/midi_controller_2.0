#include "Ili9341LvglDisplay.hpp"

#include <Arduino.h>
#include <stdarg.h>

#include "core/utils/FlashStrings.hpp"

// Buffer principal en DMAMEM (240x320 pixels = 150KB)
DMAMEM static uint16_t main_framebuffer[240 * 320];

// Buffers LVGL (40 lignes = 19.2KB chacun)
DMAMEM static lv_color_t lvgl_buffer1[240 * 40];
DMAMEM static lv_color_t lvgl_buffer2[240 * 40];

//=============================================================================
// Configuration et constructeur
//=============================================================================

Ili9341LvglDisplay::Config Ili9341LvglDisplay::getDefaultConfig() {
    return {
        .cs_pin = 9,            // CS sur pin 9
        .dc_pin = 10,           // DC sur pin 10
        .rst_pin = 6,           // RST sur pin 6
        .mosi_pin = 11,         // MOSI sur pin 11 (SPI standard)
        .sck_pin = 13,          // SCK sur pin 13 (SPI standard)
        .miso_pin = 12,         // MISO sur pin 12 (SPI standard)
        .spi_speed = 40000000,  // 40MHz - optimal pour Teensy 4.1 + LVGL
        .rotation = 0           // Portrait
    };
}

Ili9341LvglDisplay::Ili9341LvglDisplay(const Config& config)
    : config_(config),
      initialized_(false),
      framebuffer_(main_framebuffer),
      display_(nullptr),
      lvgl_buf1_(lvgl_buffer1),
      lvgl_buf2_(lvgl_buffer2) {
    Serial.println(F("Ili9341LvglDisplay: Constructor called"));
    Serial.print(F("Ili9341LvglDisplay: framebuffer_ = 0x"));
    Serial.println((unsigned long)framebuffer_, HEX);
}

Ili9341LvglDisplay::~Ili9341LvglDisplay() {
    Serial.println(F("Ili9341LvglDisplay: Destructor called"));
    if (display_) {
        lv_display_delete(display_);
    }
}

//=============================================================================
// Initialisation
//=============================================================================

bool Ili9341LvglDisplay::init() {
    if (initialized_) {
        return true;
    }

    Serial.println(F("ILI9341_LVGL: Initializing hardware..."));

    // Configuration des pins
    pinMode(config_.cs_pin, OUTPUT);
    pinMode(config_.dc_pin, OUTPUT);
    if (config_.rst_pin != 255) {
        pinMode(config_.rst_pin, OUTPUT);
        // Reset de l'écran
        digitalWrite(config_.rst_pin, LOW);
        delay(10);
        digitalWrite(config_.rst_pin, HIGH);
        delay(100);
    }

    // Créer le driver ILI9341_T4
    tft_ = std::make_unique<ILI9341_T4::ILI9341Driver>(
        config_.cs_pin,
        config_.dc_pin,
        config_.sck_pin,
        config_.mosi_pin,
        config_.miso_pin,
        config_.rst_pin
    );

    if (!tft_) {
        Serial.println(F("ILI9341_LVGL: Failed to create ILI9341Driver"));
        return false;
    }

    // Initialiser le driver
    tft_->begin(config_.spi_speed);
    tft_->setRotation(config_.rotation);
    tft_->setFramebuffer(framebuffer_);

    // Créer les diff buffers pour optimisation (alloués en DMAMEM)
    // Note: On utilise des buffers statiques DMAMEM au lieu de DiffBuffStatic
    // pour éviter la consommation RAM1
    static DMAMEM uint8_t diffbuffer1[4096]; // 4KB
    static DMAMEM uint8_t diffbuffer2[4096]; // 4KB
    
    diff1_ = std::make_unique<ILI9341_T4::DiffBuff>(diffbuffer1, sizeof(diffbuffer1));
    diff2_ = std::make_unique<ILI9341_T4::DiffBuff>(diffbuffer2, sizeof(diffbuffer2));
    tft_->setDiffBuffers(diff1_.get(), diff2_.get());

    // Configuration performance
    tft_->setRefreshRate(120);
    tft_->setVSyncSpacing(2);

    Serial.println(F("ILI9341_LVGL: Hardware initialized, setting up LVGL..."));

    // Initialiser LVGL
    if (!setupLvgl()) {
        Serial.println(F("ILI9341_LVGL: Failed to setup LVGL"));
        return false;
    }

    // Test initial - effacer l'écran
    lv_obj_t* screen = lv_screen_active();
    if (screen) {
        lv_obj_clean(screen);
    }
    lv_timer_handler(); // Forcer un rendu LVGL

    Serial.println(F("ILI9341_LVGL: Initialization complete"));
    initialized_ = true;
    
    // DEBUG: Afficher diagnostics mémoire
    debugMemory();
    
    return true;
}

bool Ili9341LvglDisplay::setupLvgl() {
    // Initialiser LVGL (doit être fait une seule fois dans l'application)
    static bool lvgl_initialized = false;
    if (!lvgl_initialized) {
        lv_init();
        lvgl_initialized = true;
        Serial.println(F("ILI9341_LVGL: LVGL core initialized"));
    }

    // Créer le display LVGL
    display_ = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display_) {
        Serial.println(F("ILI9341_LVGL: Failed to create LVGL display"));
        return false;
    }

    // Configurer les buffers de dessin LVGL
    lv_display_set_buffers(display_, lvgl_buf1_, lvgl_buf2_, 
                          sizeof(lvgl_buffer1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Définir les callbacks
    lv_display_set_flush_cb(display_, flush_cb);

    // Associer cette instance au display pour les callbacks
    lv_display_set_user_data(display_, this);

    Serial.println(F("ILI9341_LVGL: LVGL display configured"));
    return true;
}

//=============================================================================
// Callbacks LVGL statiques
//=============================================================================

void Ili9341LvglDisplay::flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
    Ili9341LvglDisplay* instance = getInstance(disp);
    if (!instance || !instance->tft_) {
        lv_display_flush_ready(disp);
        return;
    }

    unsigned long startTime = micros();

    // Calculer dimensions de la zone (variables utilisées pour debug futur)
    // int32_t w = area->x2 - area->x1 + 1;
    // int32_t h = area->y2 - area->y1 + 1;

    // Transférer via ILI9341_T4 (optimisé DMA)
    // Note: ILI9341_T4 utilise update() avec le framebuffer complet
    // Pour une version simple, on copie vers le framebuffer puis on update
    uint16_t* fb = instance->framebuffer_;
    uint16_t* src = (uint16_t*)px_map;
    
    for (int32_t y = area->y1; y <= area->y2; y++) {
        for (int32_t x = area->x1; x <= area->x2; x++) {
            fb[y * 240 + x] = *src++;
        }
    }
    
    // Mettre à jour l'écran
    instance->tft_->update(fb);

    unsigned long endTime = micros();
    instance->profiler_.recordUpdate(endTime - startTime);

    // Indiquer à LVGL que le transfert est terminé
    lv_display_flush_ready(disp);
}

void Ili9341LvglDisplay::rounder_cb(lv_display_t* disp, lv_area_t* area) {
    // Pas d'arrondi spécial nécessaire pour ILI9341
    // LVGL peut dessiner pixel par pixel
}

Ili9341LvglDisplay* Ili9341LvglDisplay::getInstance(lv_display_t* disp) {
    return static_cast<Ili9341LvglDisplay*>(lv_display_get_user_data(disp));
}

//=============================================================================
// Méthodes test et debug (temporaires Phase 1)
//=============================================================================

void Ili9341LvglDisplay::debugMemory() const {
    Serial.println(F("=== DEBUG MEMORY ==="));
    Serial.print(F("Framebuffer (240x320): 0x")); Serial.println((uint32_t)framebuffer_, HEX);
    Serial.print(F("Framebuffer size: ")); Serial.println(240 * 320 * 2); // 153,600 bytes
    
    Serial.print(F("LVGL buf1 (40 lines): 0x")); Serial.println((uint32_t)lvgl_buf1_, HEX);
    Serial.print(F("LVGL buf2 (40 lines): 0x")); Serial.println((uint32_t)lvgl_buf2_, HEX);
    Serial.print(F("LVGL buffer size each: ")); Serial.println(240 * 40 * 2); // 19,200 bytes each
    
    Serial.print(F("Diff buf1: 0x")); Serial.println((uint32_t)diff1_.get(), HEX);
    Serial.print(F("Diff buf2: 0x")); Serial.println((uint32_t)diff2_.get(), HEX);
    
    Serial.print(F("Display initialized: ")); Serial.println(initialized_);
    Serial.print(F("LVGL display: 0x")); Serial.println((uint32_t)display_, HEX);
    
    // Vérifier si c'est bien en DMAMEM (addresses 0x2020xxxx pour Teensy 4.1)
    bool fb_in_dmamem = ((uint32_t)framebuffer_ >= 0x20200000) && ((uint32_t)framebuffer_ < 0x20280000);
    bool buf1_in_dmamem = ((uint32_t)lvgl_buf1_ >= 0x20200000) && ((uint32_t)lvgl_buf1_ < 0x20280000);
    bool buf2_in_dmamem = ((uint32_t)lvgl_buf2_ >= 0x20200000) && ((uint32_t)lvgl_buf2_ < 0x20280000);
    
    Serial.print(F("Framebuffer in DMAMEM: ")); Serial.println(fb_in_dmamem ? "YES" : "NO");
    Serial.print(F("LVGL buf1 in DMAMEM: ")); Serial.println(buf1_in_dmamem ? "YES" : "NO");
    Serial.print(F("LVGL buf2 in DMAMEM: ")); Serial.println(buf2_in_dmamem ? "YES" : "NO");
    
    // Total DMAMEM utilisé
    uint32_t total_dmamem = (240 * 320 * 2) + (240 * 40 * 2) + (240 * 40 * 2) + 4096 + 4096;
    Serial.print(F("Total DMAMEM used: ")); Serial.println(total_dmamem);
    Serial.print(F("DMAMEM available on T4.1: ")); Serial.println(524288); // 512KB
    
    Serial.println(F("=================="));
}

//=============================================================================
// Performance et métriques
//=============================================================================

void Ili9341LvglDisplay::getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime,
                                            unsigned long& minTime) const {
    avgTime = profiler_.getAverageUpdateTime();
    maxTime = profiler_.getMaxUpdateTime();
    minTime = profiler_.getMinUpdateTime();
}

void Ili9341LvglDisplay::setRotation(uint8_t rotation) {
    if (!initialized_ || !tft_) return;

    config_.rotation = rotation;
    tft_->setRotation(rotation);

    Serial.print(F("ILI9341_LVGL: Rotation set to "));
    Serial.println(rotation);
}

void Ili9341LvglDisplay::getDimensions(uint16_t& width, uint16_t& height) const {
    // Dimensions selon rotation
    if (config_.rotation == 1 || config_.rotation == 3) {
        width = 320;  // Paysage
        height = 240;
    } else {
        width = 240;  // Portrait
        height = 320;
    }
}

lv_obj_t* Ili9341LvglDisplay::createTestScreen() {
    if (!initialized_ || !display_) return nullptr;

    // Créer un nouvel écran
    lv_obj_t* screen = lv_obj_create(NULL);
    
    // Ajouter un label de test
    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, "Hello LVGL!\nMIDI Controller 2.0");
    lv_obj_center(label);
    
    // Style du label
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    
    // Ajouter un bouton de test
    lv_obj_t* btn = lv_button_create(screen);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_set_pos(btn, 60, 200);
    
    lv_obj_t* btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Test Button");
    lv_obj_center(btn_label);
    
    return screen;
}

