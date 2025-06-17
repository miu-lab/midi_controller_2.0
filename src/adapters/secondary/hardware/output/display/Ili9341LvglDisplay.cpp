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
      lvgl_buf2_(lvgl_buffer2),
      currentTextColor_(1),     // Blanc par défaut
      currentTextSize_(1),
      currentCursorX_(0),
      currentCursorY_(0),
      textWrap_(false) {
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

    // Test initial - écran noir
    clear();
    update();

    Serial.println(F("ILI9341_LVGL: Initialization complete"));
    initialized_ = true;
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
// Interface DisplayPort - Méthodes de base
//=============================================================================

void Ili9341LvglDisplay::clear() {
    if (!initialized_ || !display_) return;

    unsigned long startTime = micros();

    // Effacer l'écran LVGL
    lv_obj_t* screen = lv_screen_active();
    if (screen) {
        lv_obj_clean(screen);
    }

    // Reset curseur
    currentCursorX_ = 0;
    currentCursorY_ = 0;

    unsigned long endTime = micros();
    profiler_.recordUpdate(endTime - startTime);
}

void Ili9341LvglDisplay::drawText(int x, int y, const char* text) {
    if (!initialized_ || !display_ || !text) return;

    // Créer un label LVGL
    lv_obj_t* label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, x, y);

    // Appliquer la couleur actuelle
    if (currentTextColor_ == 0) {
        lv_obj_set_style_text_color(label, lv_color_black(), 0);
    } else {
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
    }

    // Mettre à jour la position du curseur
    currentCursorX_ = x;
    currentCursorY_ = y;
}

void Ili9341LvglDisplay::drawLine(int x0, int y0, int x1, int y1) {
    if (!initialized_ || !display_) return;

    // Créer une ligne LVGL
    lv_obj_t* line_obj = lv_line_create(lv_screen_active());
    
    static lv_point_precise_t line_points[] = {{0, 0}, {0, 0}};
    line_points[0].x = x0;
    line_points[0].y = y0;
    line_points[1].x = x1;
    line_points[1].y = y1;
    
    lv_line_set_points(line_obj, line_points, 2);
    
    // Couleur de la ligne
    if (currentTextColor_ == 0) {
        lv_obj_set_style_line_color(line_obj, lv_color_black(), 0);
    } else {
        lv_obj_set_style_line_color(line_obj, lv_color_white(), 0);
    }
}

void Ili9341LvglDisplay::drawRect(int x, int y, int width, int height, bool fill) {
    if (!initialized_ || !display_) return;

    // Créer un rectangle LVGL
    lv_obj_t* rect = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(rect, x, y);
    lv_obj_set_size(rect, width, height);

    if (fill) {
        // Rectangle rempli
        if (currentTextColor_ == 0) {
            lv_obj_set_style_bg_color(rect, lv_color_black(), 0);
        } else {
            lv_obj_set_style_bg_color(rect, lv_color_white(), 0);
        }
        lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, 0);
    } else {
        // Rectangle vide (contour seulement)
        lv_obj_set_style_bg_opa(rect, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(rect, 1, 0);
        if (currentTextColor_ == 0) {
            lv_obj_set_style_border_color(rect, lv_color_black(), 0);
        } else {
            lv_obj_set_style_border_color(rect, lv_color_white(), 0);
        }
    }
}

void Ili9341LvglDisplay::drawCircle(int x, int y, int radius, bool fill) {
    if (!initialized_ || !display_) return;

    // Créer un cercle LVGL avec arc
    lv_obj_t* circle = lv_arc_create(lv_screen_active());
    lv_obj_set_pos(circle, x - radius, y - radius);
    lv_obj_set_size(circle, radius * 2, radius * 2);
    
    // Configuration de l'arc pour faire un cercle complet
    lv_arc_set_angles(circle, 0, 360);
    lv_arc_set_bg_angles(circle, 0, 360);
    
    if (fill) {
        lv_obj_set_style_arc_width(circle, radius, 0);
    } else {
        lv_obj_set_style_arc_width(circle, 1, 0);
    }
    
    // Couleur
    if (currentTextColor_ == 0) {
        lv_obj_set_style_arc_color(circle, lv_color_black(), 0);
    } else {
        lv_obj_set_style_arc_color(circle, lv_color_white(), 0);
    }
}

void Ili9341LvglDisplay::update() {
    if (!initialized_ || !tft_) return;

    unsigned long startTime = micros();

    // LVGL se charge automatiquement du rendu via les callbacks
    // On déclenche juste la boucle de traitement LVGL
    lv_timer_handler();

    unsigned long endTime = micros();
    profiler_.recordUpdate(endTime - startTime);
}

//=============================================================================
// Interface DisplayPort - Configuration du texte
//=============================================================================

void Ili9341LvglDisplay::setTextSize(uint8_t size) {
    if (!initialized_) return;
    currentTextSize_ = size;
    // Note: Avec LVGL, la taille est gérée par les fontes, pas par un facteur multiplicateur
}

void Ili9341LvglDisplay::setTextColor(uint16_t color) {
    if (!initialized_) return;
    currentTextColor_ = color;
}

void Ili9341LvglDisplay::setTextWrap(bool wrap) {
    if (!initialized_) return;
    textWrap_ = wrap;
}

void Ili9341LvglDisplay::setCursor(int16_t x, int16_t y) {
    if (!initialized_) return;
    currentCursorX_ = x;
    currentCursorY_ = y;
}

void Ili9341LvglDisplay::getTextBounds(const char* text, uint16_t* w, uint16_t* h) {
    if (!initialized_ || !text || !w || !h) {
        if (w) *w = 0;
        if (h) *h = 0;
        return;
    }

    // Mesurer avec la fonte par défaut LVGL
    lv_point_t size;
    lv_text_get_size(&size, text, LV_FONT_DEFAULT, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
    
    *w = size.x;
    *h = size.y;
}

//=============================================================================
// Interface DisplayPort - Méthodes avancées
//=============================================================================

void Ili9341LvglDisplay::drawCenteredText(int x, int y, const char* text) {
    if (!initialized_ || !display_ || !text) return;

    // Calculer les dimensions du texte
    uint16_t w, h;
    getTextBounds(text, &w, &h);

    // Centrer horizontalement
    int centeredX = x - (w / 2);

    drawText(centeredX, y, text);
}

void Ili9341LvglDisplay::drawFormattedText(int x, int y, const char* format, ...) {
    if (!initialized_ || !format) return;

    char buffer[64];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    drawText(x, y, buffer);
}

//=============================================================================
// Performance et identification
//=============================================================================

void Ili9341LvglDisplay::getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime,
                                            unsigned long& minTime) const {
    avgTime = profiler_.getAverageUpdateTime();
    maxTime = profiler_.getMaxUpdateTime();
    minTime = profiler_.getMinUpdateTime();
}

//=============================================================================
// Méthodes étendues
//=============================================================================

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

//=============================================================================
// Méthodes privées
//=============================================================================

lv_color_t Ili9341LvglDisplay::convertMonoColor(uint16_t monoColor) const {
    return (monoColor == 0) ? lv_color_black() : lv_color_white();
}

void Ili9341LvglDisplay::updateDimensions() {
    // Les dimensions sont gérées par LVGL selon la rotation
}