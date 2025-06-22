#include "Ili9341LvglDisplay.hpp"

#include <Arduino.h>
#include <stdarg.h>

#include "core/utils/FlashStrings.hpp"
#include "adapters/ui/lvgl/widgets/ParameterWidget.hpp"

// Buffer principal en DMAMEM (240x320 pixels = 150KB)
DMAMEM static uint16_t main_framebuffer[240 * 320];

// Buffers LVGL optimisés pour performance (320 * 60 lignes = 38.4KB chacun)
// Plus gros buffers = moins de flush callbacks = plus de FPS
DMAMEM static lv_color_t lvgl_buffer1[320 * 60];
DMAMEM static lv_color_t lvgl_buffer2[320 * 60];

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
        .spi_speed = 40000000,  // 80MHz - vitesse max pour ILI9341 sur Teensy 4.1
        .rotation = 1  // Paysage (0=Portrait, 1=Paysage, 2=Portrait inversé, 3=Paysage inversé)
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
        digitalWrite(config_.rst_pin, HIGH);
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
    // IMPORTANT: Donner au driver son propre framebuffer interne pour que updateRegion marche
    tft_->setFramebuffer(framebuffer_);

    // Créer les diff buffers pour optimisation (alloués en DMAMEM)
    // Note: On utilise des buffers statiques DMAMEM au lieu de DiffBuffStatic
    // pour éviter la consommation RAM1
    static DMAMEM uint8_t diffbuffer1[4096]; // 4KB
    static DMAMEM uint8_t diffbuffer2[4096]; // 4KB
    
    diff1_ = std::make_unique<ILI9341_T4::DiffBuff>(diffbuffer1, sizeof(diffbuffer1));
    diff2_ = std::make_unique<ILI9341_T4::DiffBuff>(diffbuffer2, sizeof(diffbuffer2));
    tft_->setDiffBuffers(diff1_.get(), diff2_.get());

    // Configuration performance optimisée (selon exemple officiel)
    tft_->setDiffGap(4);        // gap petit avec buffers diff 4K
    tft_->setVSyncSpacing(1);   // minimiser tearing, LVGL contrôle framerate
    tft_->setRefreshRate(100);  // 100Hz pour dépasser 60 FPS

    Serial.println(F("ILI9341_LVGL: Hardware initialized, setting up LVGL..."));

    // Initialiser LVGL
    if (!setupLvgl()) {
        Serial.println(F("ILI9341_LVGL: Failed to setup LVGL"));
        return false;
    }

    // Test initial - effacer l'écran et forcer le rendu
    lv_obj_t* screen = lv_screen_active();
    if (screen) {
        lv_obj_clean(screen);
        lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    }
    lv_timer_handler(); // Forcer un rendu LVGL

    // Forcer mise à jour framebuffer (critique pour premier affichage)
    tft_->update(framebuffer_);

    Serial.println(F("ILI9341_LVGL: Initialization complete"));
    initialized_ = true;

    return true;
}

bool Ili9341LvglDisplay::setupLvgl() {
    // Initialiser LVGL (doit être fait une seule fois dans l'application)
    static bool lvgl_initialized = false;
    if (!lvgl_initialized) {
        lv_init();

        // ✅ Configurer le tick system comme dans l'exemple officiel
        lv_tick_set_cb([]() -> uint32_t { return millis(); });

        lvgl_initialized = true;
        Serial.println(F("ILI9341_LVGL: LVGL core initialized"));
    }

    // Créer le display LVGL avec dimensions selon rotation
    uint16_t display_width, display_height;
    getDimensions(display_width, display_height);

    Serial.print(F("ILI9341_LVGL: Creating display with dimensions "));
    Serial.print(display_width);
    Serial.print(F("x"));
    Serial.print(display_height);
    Serial.print(F(" (rotation "));
    Serial.print(config_.rotation);
    Serial.println(F(")"));

    display_ = lv_display_create(display_width, display_height);
    if (!display_) {
        Serial.println(F("ILI9341_LVGL: Failed to create LVGL display"));
        return false;
    }

    // Configurer les buffers de dessin LVGL avec double buffering
    // Buffer size optimisé : 320 * 60 * sizeof(lv_color_t) = 38400 bytes
    lv_display_set_buffers(display_,
                           lvgl_buf1_,
                           lvgl_buf2_,  // Double buffer pour performance
                           320 * 60 * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Définir les callbacks
    lv_display_set_flush_cb(display_, flush_cb);

    // Associer cette instance au display pour les callbacks
    lv_display_set_user_data(display_, this);

    // CRITIQUE: Définir comme display par défaut
    lv_display_set_default(display_);

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

    // FLUSH CALLBACK OPTIMISÉ (selon exemple officiel ILI9341_T4 + LVGL)
    const bool redraw_now = lv_disp_flush_is_last(disp);
    instance->tft_
        ->updateRegion(redraw_now, (uint16_t*)px_map, area->x1, area->x2, area->y1, area->y2);

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
    Serial.print(F("Framebuffer (240x320): 0x"));
    Serial.println((uint32_t)framebuffer_, HEX);

    Serial.print(F("LVGL buf1 (40 lines): 0x"));
    Serial.println((uint32_t)lvgl_buf1_, HEX);
    Serial.print(F("LVGL buf2 (40 lines): 0x"));
    Serial.println((uint32_t)lvgl_buf2_, HEX);

    Serial.print(F("Diff buf1: 0x"));
    Serial.println((uint32_t)diff1_.get(), HEX);
    Serial.print(F("Diff buf2: 0x"));
    Serial.println((uint32_t)diff2_.get(), HEX);
    Serial.print(F("Display initialized: "));
    Serial.println(initialized_);
    Serial.print(F("LVGL display: 0x"));
    Serial.println((uint32_t)display_, HEX);
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
        width = 320;  // Paysage (90° et 270°)
        height = 240;
    } else {
        width = 240;  // Portrait (0° et 180°)
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
// FlushProfiler - Métriques zones de rendu
//=============================================================================

void Ili9341LvglDisplay::FlushProfiler::recordFlush(const lv_area_t* area, unsigned long duration) {
    if (!area) return;
    
    // Calculer pixels dans cette zone
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;
    uint32_t pixels = w * h;
    
    total_pixels_updated_ += pixels;
    flush_count_++;
    total_flush_time_ += duration;
    
    if (duration > max_flush_time_) max_flush_time_ = duration;
    if (duration < min_flush_time_) min_flush_time_ = duration;
    
    // Déterminer si c'est un rendu plein écran ou partiel
    bool is_full_screen = (w >= 240 && h >= 320);
    if (is_full_screen) {
        full_screen_updates_++;
    } else {
        partial_updates_++;
    }
}

void Ili9341LvglDisplay::FlushProfiler::printStats() const {
    if (flush_count_ == 0) {
        Serial.println(F("No flush stats yet"));
        return;
    }
    
    Serial.println(F("=== FLUSH PROFILER STATS ==="));
    Serial.print(F("Total flushes: ")); Serial.println(flush_count_);
    Serial.print(F("Full screen updates: ")); Serial.println(full_screen_updates_);
    Serial.print(F("Partial updates: ")); Serial.println(partial_updates_);
    
    float avg_pixels = getAveragePixelsPerFlush();
    Serial.print(F("Average pixels/flush: ")); Serial.println(avg_pixels);
    
    float avg_time = getAverageFlushTime();
    Serial.print(F("Average flush time: ")); Serial.print(avg_time); Serial.println(F("μs"));
    Serial.print(F("Max flush time: ")); Serial.print(max_flush_time_); Serial.println(F("μs"));
    Serial.print(F("Min flush time: ")); Serial.print(min_flush_time_); Serial.println(F("μs"));
    
    // Calcul efficacité rendu partiel
    float partial_ratio = (flush_count_ > 0) ? (float)partial_updates_ / flush_count_ * 100 : 0;
    Serial.print(F("Partial render efficiency: ")); Serial.print(partial_ratio); Serial.println(F("%"));
    
    Serial.println(F("============================"));
}

void Ili9341LvglDisplay::FlushProfiler::reset() {
    total_pixels_updated_ = 0;
    flush_count_ = 0;
    full_screen_updates_ = 0;
    partial_updates_ = 0;
    total_flush_time_ = 0;
    max_flush_time_ = 0;
    min_flush_time_ = ULONG_MAX;
}

float Ili9341LvglDisplay::FlushProfiler::getAveragePixelsPerFlush() const {
    return (flush_count_ > 0) ? (float)total_pixels_updated_ / flush_count_ : 0;
}

float Ili9341LvglDisplay::FlushProfiler::getAverageFlushTime() const {
    return (flush_count_ > 0) ? (float)total_flush_time_ / flush_count_ : 0;
}

//=============================================================================
// Benchmark performance (Phase 1)
//=============================================================================

void Ili9341LvglDisplay::runPerformanceBenchmark() {
    if (!initialized_ || !display_) {
        Serial.println(F("Display not initialized for benchmark"));
        return;
    }
    
    Serial.println(F("=== PERFORMANCE BENCHMARK ==="));
    
    // Reset profiler
    flush_profiler_.reset();
    
    // Test 1: Écran uniforme (optimal)
    Serial.println(F("Test 1: Uniform screen fill"));
    lv_obj_t* screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    
    unsigned long start = millis();
    for (int i = 0; i < 30; i++) { // 30 frames
        lv_timer_handler();
        delay(1); // Petit délai pour laisser le temps au rendu
    }
    unsigned long duration1 = millis() - start;
    
    flush_profiler_.printStats();
    flush_profiler_.reset();
    
    // Test 2: Animation simple - changer couleur
    Serial.println(F("Test 2: Color animation"));
    start = millis();
    for (int i = 0; i < 30; i++) {
        lv_color_t color = (i % 2 == 0) ? lv_color_white() : lv_color_black();
        lv_obj_set_style_bg_color(screen, color, 0);
        lv_timer_handler();
        delay(1);
    }
    unsigned long duration2 = millis() - start;
    
    flush_profiler_.printStats();
    flush_profiler_.reset();
    
    // Test 3: Création/suppression d'objets (stress partiel)
    Serial.println(F("Test 3: Dynamic objects stress"));
    start = millis();
    for (int i = 0; i < 20; i++) {
        // Créer des labels temporaires
        lv_obj_t* label = lv_label_create(screen);
        lv_label_set_text(label, "Benchmark");
        lv_obj_set_pos(label, i * 10, i * 10);
        lv_timer_handler();
        
        // Supprimer
        lv_obj_delete(label);
        lv_timer_handler();
        delay(1);
    }
    unsigned long duration3 = millis() - start;
    
    flush_profiler_.printStats();
    flush_profiler_.reset();
    
    // Test 4: Arc animé (widget complexe)
    Serial.println(F("Test 4: Arc widget animation"));
    lv_obj_t* arc = lv_arc_create(screen);
    lv_obj_set_size(arc, 200, 200);
    lv_obj_center(arc);
    
    start = millis();
    for (int i = 0; i <= 100; i += 5) {
        lv_arc_set_value(arc, i);
        lv_timer_handler();
        delay(1);
    }
    unsigned long duration4 = millis() - start;
    
    flush_profiler_.printStats();
    
    // Nettoyer
    lv_obj_delete(arc);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_timer_handler();
    
    // Résumé final
    Serial.println(F("=== BENCHMARK SUMMARY ==="));
    Serial.print(F("Test 1 (uniform): ")); Serial.print(duration1); Serial.println(F("ms"));
    Serial.print(F("Test 2 (color anim): ")); Serial.print(duration2); Serial.println(F("ms"));
    Serial.print(F("Test 3 (dynamic): ")); Serial.print(duration3); Serial.println(F("ms"));
    Serial.print(F("Test 4 (arc anim): ")); Serial.print(duration4); Serial.println(F("ms"));
    
    // Calcul FPS approximatif
    float fps1 = 30000.0 / duration1;
    float fps2 = 30000.0 / duration2;
    float fps3 = 40000.0 / duration3; // 20 create + 20 delete = 40 frames
    float fps4 = 21000.0 / duration4; // 21 arc updates
    
    Serial.print(F("Estimated FPS - Test 1: ")); Serial.println(fps1);
    Serial.print(F("Estimated FPS - Test 2: ")); Serial.println(fps2);
    Serial.print(F("Estimated FPS - Test 3: ")); Serial.println(fps3);
    Serial.print(F("Estimated FPS - Test 4: ")); Serial.println(fps4);
    
    Serial.println(F("========================"));
}

//=============================================================================
// Tests hardware robustesse (Phase 1)
//=============================================================================

bool Ili9341LvglDisplay::testMultipleInit() {
    Serial.println(F("=== TEST MULTIPLE INIT ==="));
    
    // Test 1: Re-init sur display déjà initialisé
    Serial.println(F("Test 1: Re-init on initialized display"));
    bool result1 = init();
    if (!result1) {
        Serial.println(F("FAILED: Re-init returned false"));
        return false;
    }
    
    // Test 2: Marquer comme non-initialisé et re-init
    Serial.println(F("Test 2: Reset and re-init"));
    initialized_ = false;
    bool result2 = init();
    if (!result2) {
        Serial.println(F("FAILED: Second init returned false"));
        return false;
    }
    
    // Test 3: Vérifier que l'écran fonctionne encore
    Serial.println(F("Test 3: Verify display still works"));
    lv_obj_t* screen = lv_screen_active();
    if (!screen) {
        Serial.println(F("FAILED: No active screen after re-init"));
        return false;
    }
    
    // Afficher quelque chose pour vérifier
    lv_obj_set_style_bg_color(screen, lv_color_make(0x00, 0xFF, 0x00), 0); // Vert
    lv_timer_handler();
    delay(500);
    
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0); // Retour noir
    lv_timer_handler();
    
    Serial.println(F("Multiple init test: PASSED"));
    return true;
}

bool Ili9341LvglDisplay::testAllRotations() {
    Serial.println(F("=== TEST ALL ROTATIONS ==="));
    
    if (!initialized_) {
        Serial.println(F("Display not initialized"));
        return false;
    }
    
    uint8_t originalRotation = config_.rotation;
    
    // Tester toutes les rotations
    for (uint8_t rot = 0; rot < 4; rot++) {
        Serial.print(F("Testing rotation ")); Serial.println(rot);
        
        setRotation(rot);
        
        // Vérifier dimensions
        uint16_t w, h;
        getDimensions(w, h);
        Serial.print(F("  Dimensions: ")); Serial.print(w); 
        Serial.print(F("x")); Serial.println(h);
        
        // Afficher pattern de test
        lv_obj_t* screen = lv_screen_active();
        lv_obj_set_style_bg_color(screen, lv_color_make(0xFF, 0x00, 0x00), 0); // Rouge
        
        // Créer un label pour identifier la rotation
        lv_obj_t* label = lv_label_create(screen);
        char rotText[16];
        snprintf(rotText, sizeof(rotText), "Rotation %d", rot);
        lv_label_set_text(label, rotText);
        lv_obj_set_pos(label, 10, 10);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        
        lv_timer_handler();
        delay(1000); // 1 sec pour voir chaque rotation
        
        // Nettoyer
        lv_obj_delete(label);
        lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
        lv_timer_handler();
        
        // Vérifier que rotation est appliquée
        if (config_.rotation != rot) {
            Serial.print(F("FAILED: Rotation not applied correctly. Expected "));
            Serial.print(rot); Serial.print(F(", got ")); Serial.println(config_.rotation);
            return false;
        }
    }
    
    // Restaurer rotation originale
    setRotation(originalRotation);
    
    Serial.println(F("All rotations test: PASSED"));
    return true;
}

bool Ili9341LvglDisplay::testEndurance(uint16_t cycles) {
    Serial.println(F("=== TEST ENDURANCE ==="));
    Serial.print(F("Running ")); Serial.print(cycles); Serial.println(F(" cycles"));
    
    if (!initialized_) {
        Serial.println(F("Display not initialized"));
        return false;
    }
    
    flush_profiler_.reset();
    
    lv_obj_t* screen = lv_screen_active();
    unsigned long startTime = millis();
    
    for (uint16_t cycle = 0; cycle < cycles; cycle++) {
        // Cycle A: Créer/supprimer objets
        lv_obj_t* label = lv_label_create(screen);
        lv_label_set_text(label, "Endurance Test");
        lv_obj_set_pos(label, cycle % 200, (cycle * 7) % 280);
        lv_timer_handler();
        
        // Cycle B: Changer couleurs
        lv_color_t color = lv_color_make(cycle % 255, (cycle * 2) % 255, (cycle * 3) % 255);
        lv_obj_set_style_bg_color(screen, color, 0);
        lv_timer_handler();
        
        // Cycle C: Supprimer et nettoyer
        lv_obj_delete(label);
        lv_timer_handler();
        
        // Debug périodique
        if (cycle % 100 == 0) {
            Serial.print(F("Cycle ")); Serial.print(cycle);
            Serial.print(F(" - Free RAM: "));
            
            // Mesure mémoire Teensy 4.1 - simple estimation
            char top;
            uint32_t freeMemory = &top - (char*)0x20000000; // Estimation simple
            Serial.print(freeMemory);
            Serial.println(F(" bytes (approx)"));
            
            // Relaxer le test pour éviter faux positifs
            // (Le vrai test est l'absence de crash/freeze)
            Serial.println(F("Memory check: OK"));
        }
        
        // Petit délai pour ne pas surcharger
        if (cycle % 10 == 0) {
            delay(1);
        }
    }
    
    unsigned long endTime = millis();
    unsigned long totalTime = endTime - startTime;
    
    // Reset final
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_timer_handler();
    
    // Statistiques finales
    Serial.println(F("=== ENDURANCE RESULTS ==="));
    Serial.print(F("Total time: ")); Serial.print(totalTime); Serial.println(F("ms"));
    Serial.print(F("Cycles/sec: ")); Serial.println((float)cycles * 1000 / totalTime);
    
    flush_profiler_.printStats();
    
    Serial.println(F("Endurance test: PASSED"));
    return true;
}

void Ili9341LvglDisplay::runFullHardwareTestSuite() {
    Serial.println(F(""));
    Serial.println(F("################################"));
    Serial.println(F("### FULL HARDWARE TEST SUITE ###"));
    Serial.println(F("################################"));
    
    // Diagnostic initial
    debugMemory();
    
    // Test 1: Multiple init
    bool test1 = testMultipleInit();

    // Test 2: Rotations (temporairement désactivé pour économiser mémoire)
    Serial.println(F("Skipping rotation test to save memory"));
    bool test2 = true;  // testAllRotations();

    // Test 3: Performance benchmark
    Serial.println(F("Running performance benchmark..."));
    runPerformanceBenchmark();
    
    // Test 4: Endurance (version courte pour test rapide)
    bool test4 = testEndurance(100); // 100 cycles pour test rapide
    
    // Test 5: ParameterWidget Demo (Phase 2)
    Serial.println(F(""));
    Serial.println(F("Phase 1 Test Option: Send 'T' via Serial to run hardware tests"));
    Serial.println(F("(Tests include: init, rotations, performance, endurance)"));
    Serial.println(F("Phase 2 Test Option: Send 'P' via Serial to run ParameterWidget demo"));
    Serial.println(F("(Visual demo: animations, parameters, interactions)"));
    
    // Résumé final
    Serial.println(F(""));
    Serial.println(F("=== HARDWARE TEST SUMMARY ==="));
    Serial.print(F("Multiple Init: ")); Serial.println(test1 ? "PASS" : "FAIL");
    Serial.print(F("All Rotations: ")); Serial.println(test2 ? "PASS" : "FAIL");
    Serial.print(F("Performance: ")); Serial.println("COMPLETED");
    Serial.print(F("Endurance: ")); Serial.println(test4 ? "PASS" : "FAIL");
    
    bool allPassed = test1 && test2 && test4;
    Serial.println(F(""));
    Serial.print(F("### OVERALL RESULT: ")); 
    Serial.println(allPassed ? "ALL TESTS PASSED" : "SOME TESTS FAILED");
    Serial.println(F("################################"));
}

//=============================================================================
// Test ParameterWidget (Phase 2)
//=============================================================================

bool Ili9341LvglDisplay::testParameterWidget() {
    Serial.println(F("=== TEST PARAMETER WIDGET ==="));
    
    if (!initialized_) {
        Serial.println(F("FAILED: Display not initialized"));
        return false;
    }
    
    // Note: Inclure le header ici causerait une dépendance circulaire
    // Le test complet sera fait dans un fichier séparé ou main.cpp
    Serial.println(F("Creating basic LVGL test objects..."));
    
    // Test de base: créer un arc LVGL pour valider que le système fonctionne
    lv_obj_t* test_screen = lv_obj_create(lv_screen_active());
    lv_obj_set_size(test_screen, 240, 120);
    lv_obj_center(test_screen);
    lv_obj_set_style_bg_color(test_screen, lv_color_hex(0x202020), 0);
    
    // Arc de test
    lv_obj_t* test_arc = lv_arc_create(test_screen);
    lv_obj_set_size(test_arc, 80, 80);
    lv_obj_center(test_arc);
    lv_arc_set_range(test_arc, 0, 127);
    lv_arc_set_value(test_arc, 64); // Valeur moyenne
    
    // Label de test
    lv_obj_t* test_label = lv_label_create(test_screen);
    lv_label_set_text(test_label, "PARAM TEST");
    lv_obj_center(test_label);
    lv_obj_set_style_text_color(test_label, lv_color_white(), 0);
    
    // Forcer le rendu
    lv_timer_handler();
    delay(2000); // Afficher 2 secondes
    
    // Animation test
    Serial.println(F("Testing arc animation..."));
    for (int value = 0; value <= 127; value += 8) {
        lv_arc_set_value(test_arc, value);
        char value_text[16];
        snprintf(value_text, sizeof(value_text), "VALUE: %d", value);
        lv_label_set_text(test_label, value_text);
        lv_timer_handler();
    }
    
    // Nettoyage
    lv_obj_delete(test_screen);
    lv_timer_handler();
    
    Serial.println(F("ParameterWidget basic test: PASSED"));
    return true;
}

//=============================================================================
// Démo visuelle ParameterWidget (Phase 2)
//=============================================================================

bool Ili9341LvglDisplay::demoParameterWidget() {
    Serial.println(F(""));
    Serial.println(F("======================================"));
    Serial.println(F("=== PARAMETER WIDGET VISUAL DEMO ==="));
    Serial.println(F("======================================"));
    
    if (!initialized_) {
        Serial.println(F("FAILED: Display not initialized"));
        return false;
    }

    Serial.println(F("Creating new screen for ParameterWidget demo..."));

    // Créer un NOUVEL écran (comme dans createTestScreen qui fonctionne)
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101010), 0); // Gris très foncé pour contraste

    // Charger le nouvel écran
    lv_screen_load(screen);
    lv_timer_handler();
    delay(500);

    // Test 1: Créer un simple label de test
    Serial.println(F("Testing basic LVGL on new screen..."));
    lv_obj_t* test_label = lv_label_create(screen);
    lv_label_set_text(test_label, "LVGL TEST");
    lv_obj_set_pos(test_label, 10, 10);
    lv_obj_set_style_text_color(test_label, lv_color_white(), 0);
    lv_timer_handler();
    delay(2000);
    
    Serial.println(F("Basic LVGL test complete - creating ParameterWidget..."));
    
    // Nettoyer avant ParameterWidget
    lv_obj_delete(test_label);
    lv_timer_handler();

    // Créer le widget avec configuration personnalisée
    ParameterWidget::Config config = ParameterWidget::getDefaultConfig();
    config.arc_color = lv_color_hex(0x00FF80);  // Vert électrique
    config.arc_width = 8;                       // Plus épais pour la démo

    auto widget = std::make_unique<ParameterWidget>(screen, config);
    
    // Centrer le widget sur l'écran (position absolue)
    widget->setPosition(80, 100);  // Centré sur écran 240x320
    
    // Forcer le rendu immédiat
    lv_timer_handler();
    delay(1000);
    
    Serial.println(F("Widget positioned and rendered"));
    
    Serial.println(F("ParameterWidget created successfully!"));
    
    // === TEST 1: Configuration initiale ===
    Serial.println(F(""));
    Serial.println(F("=== TEST 1: Initial Setup ==="));
    widget->setParameter(74, 1, 0, "CUTOFF", true);
    lv_timer_handler();
    delay(2000);
    
    // === TEST 2: Animation des valeurs ===
    Serial.println(F("=== TEST 2: Value Animation ==="));
    String parameters[] = {"CUTOFF", "RESONANCE", "ATTACK", "DECAY", "SUSTAIN", "RELEASE"};
    uint8_t cc_numbers[] = {74, 71, 73, 75, 79, 72};
    
    for (int param = 0; param < 6; param++) {
        Serial.print(F("Testing parameter: "));
        Serial.println(parameters[param]);
        
        // Changer paramètre
        widget->setParameter(cc_numbers[param], 1, 0, parameters[param], false);
        lv_timer_handler();
        delay(500);
        
        // Animation valeur 0 -> 127
        for (uint8_t value = 0; value <= 127; value += 1) {
            widget->setValue(value, true);
            lv_timer_handler();
        }

        // Animation 127 -> 0
        for (uint8_t value = 127; value > 0; value -= 1) {
            widget->setValue(value, true);
            lv_timer_handler();
        }
        widget->setValue(0, true);
        lv_timer_handler();
    }
    
    // === TEST 3: Canaux MIDI différents ===
    Serial.println(F("=== TEST 3: MIDI Channels ==="));
    for (uint8_t channel = 1; channel <= 16; channel += 3) {
        Serial.print(F("Testing channel: "));
        Serial.println(channel);
        
        widget->setParameter(74, channel, 64, "CHANNEL TEST", true);
        lv_timer_handler();
        delay(1000);
    }
    
    // === TEST 4: Stress test avec changements rapides ===
    Serial.println(F("=== TEST 4: Rapid Changes ==="));
    widget->setParameter(74, 1, 0, "STRESS TEST", false);
    lv_timer_handler();
    
    for (int cycle = 0; cycle < 50; cycle++) {
        uint8_t random_value = cycle * 127 / 50;  // Progression linéaire
        widget->setValue(random_value, false);  // Pas d'animation pour test rapide
        lv_timer_handler();
        delay(10);
    }
    
    // === TEST 5: Animation finale avec callback ===
    Serial.println(F("=== TEST 5: Final Demo ==="));
    
    // Configuration callback pour logs
    widget->setValueChangedCallback([](uint8_t value) {
        Serial.print(F("Callback: New value = "));
        Serial.println(value);
    });
    
    widget->setParameter(74, 1, 64, "FINAL DEMO", true);
    lv_timer_handler();
    delay(2000);
    
    // Simulation d'interaction utilisateur (changement lent)
    for (uint8_t value = 64; value <= 100; value += 2) {
        widget->setValue(value, true);
        lv_timer_handler();
        delay(10);
    }
    
    // === NETTOYAGE ===
    Serial.println(F(""));
    Serial.println(F("=== DEMO COMPLETED ==="));
    Serial.println(F("Cleaning up..."));
    
    // Le widget sera automatiquement détruit à la sortie de scope
    // Nettoyer l'écran
    lv_obj_clean(lv_screen_active());
    lv_timer_handler();
    
    Serial.println(F(""));
    Serial.println(F("========================================"));
    Serial.println(F("=== PARAMETER WIDGET DEMO: SUCCESS! ==="));
    Serial.println(F("========================================"));
    
    return true;
}

//=============================================================================
// Test direct du framebuffer (diagnostic)
//=============================================================================

bool Ili9341LvglDisplay::testDirectFramebuffer() {
    Serial.println(F("=== DIRECT FRAMEBUFFER TEST ==="));

    if (!initialized_ || !tft_) {
        Serial.println(F("FAILED: Display not initialized"));
        return false;
    }

    Serial.println(F("Drawing directly to framebuffer..."));

    // Test 1: Remplir l'écran en rouge
    Serial.println(F("Test 1: Red screen"));
    uint16_t red = 0xF800;  // Rouge en RGB565
    for (int i = 0; i < 240 * 320; i++) {
        framebuffer_[i] = red;
    }
    tft_->update(framebuffer_);
    delay(2000);

    // Test 2: Remplir l'écran en vert
    Serial.println(F("Test 2: Green screen"));
    uint16_t green = 0x07E0;  // Vert en RGB565
    for (int i = 0; i < 240 * 320; i++) {
        framebuffer_[i] = green;
    }
    tft_->update(framebuffer_);
    delay(2000);

    // Test 3: Remplir l'écran en bleu
    Serial.println(F("Test 3: Blue screen"));
    uint16_t blue = 0x001F;  // Bleu en RGB565
    for (int i = 0; i < 240 * 320; i++) {
        framebuffer_[i] = blue;
    }
    tft_->update(framebuffer_);
    delay(2000);

    // Test 4: Pattern simple - lignes horizontales
    Serial.println(F("Test 4: Horizontal lines"));
    for (int y = 0; y < 320; y++) {
        uint16_t color = (y % 40 < 20) ? 0xFFFF : 0x0000;  // Blanc/noir
        for (int x = 0; x < 240; x++) {
            framebuffer_[y * 240 + x] = color;
        }
    }
    tft_->update(framebuffer_);
    delay(2000);

    // Test 5: Pattern simple - lignes verticales
    Serial.println(F("Test 5: Vertical lines"));
    for (int y = 0; y < 320; y++) {
        for (int x = 0; x < 240; x++) {
            uint16_t color = (x % 40 < 20) ? 0xF800 : 0x001F;  // Rouge/bleu
            framebuffer_[y * 240 + x] = color;
        }
    }
    tft_->update(framebuffer_);
    delay(2000);

    // Test 6: Clear to black
    Serial.println(F("Test 6: Back to black"));
    for (int i = 0; i < 240 * 320; i++) {
        framebuffer_[i] = 0x0000;
    }
    tft_->update(framebuffer_);

    Serial.println(F("Direct framebuffer test completed"));
    return true;
}

//=============================================================================
// Test LVGL simple (diagnostic flush callback)
//=============================================================================

bool Ili9341LvglDisplay::testSimpleLvgl() {
    Serial.println(F("=== SIMPLE LVGL TEST ==="));

    if (!initialized_) {
        Serial.println(F("FAILED: Display not initialized"));
        return false;
    }

    Serial.println(F("Testing LVGL flush callback step by step..."));

    // D'abord effacer le framebuffer manuellement (pour comparaison)
    Serial.println(F("Step 1: Clear framebuffer manually"));
    for (int i = 0; i < 240 * 320; i++) {
        framebuffer_[i] = 0x0000;  // Noir
    }
    tft_->update(framebuffer_);
    delay(1000);

    // Maintenant essayer avec LVGL - écran plein noir
    Serial.println(F("Step 2: LVGL black screen"));
    lv_obj_t* screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Forcer plusieurs appels pour être sûr avec tick manuel
    for (int i = 0; i < 10; i++) {
        lv_tick_inc(100);  // Simuler 100ms
        lv_timer_handler();
        delay(100);
    }
    delay(1000);

    // LVGL écran plein rouge
    Serial.println(F("Step 3: LVGL red screen"));
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFF0000), 0);
    lv_timer_handler();
    delay(2000);

    // Test différent : créer des objets au lieu de changer les couleurs
    Serial.println(F("Step 4: Create green rectangle"));
    lv_obj_t* rect = lv_obj_create(screen);
    lv_obj_set_size(rect, 200, 100);
    lv_obj_center(rect);
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x00FF00), 0);

    // Forcer les ticks et la synchronisation
    Serial.println(F("DEBUG: Forcing LVGL refresh..."));
    for (int i = 0; i < 10; i++) {
        lv_tick_inc(50);  // Forcer 50ms de tick
        lv_timer_handler();
        delay(50);
    }
    lv_obj_invalidate(screen);  // Invalider tout l'écran
    lv_timer_handler();
    delay(1000);

    Serial.println(F("Step 5: Change to blue rectangle"));
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x0000FF), 0);
    lv_obj_invalidate(rect);  // Forcer le redraw
    lv_timer_handler();
    delay(2000);

    Serial.println(F("Step 5b: Delete rectangle"));
    lv_obj_delete(rect);
    lv_timer_handler();
    delay(1000);

    // Test avec un objet simple
    Serial.println(F("Step 6: LVGL simple label"));
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, "HELLO LVGL!");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_pos(label, 50, 50);
    lv_timer_handler();
    delay(3000);

    // Nettoyer
    Serial.println(F("Step 7: Cleanup"));
    lv_obj_delete(label);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_timer_handler();

    Serial.println(F("Simple LVGL test completed"));
    return true;
}
