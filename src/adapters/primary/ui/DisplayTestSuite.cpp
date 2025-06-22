#include "DisplayTestSuite.hpp"
#include "config/DisplayConfig.hpp"
#include "core/utils/FlashStrings.hpp"
#include <Arduino.h>

//=============================================================================
// Constructeur
//=============================================================================

DisplayTestSuite::DisplayTestSuite(std::shared_ptr<Ili9341Driver> driver,
                                 std::shared_ptr<Ili9341LvglBridge> bridge)
    : driver_(driver), bridge_(bridge) {
    Serial.println(F("DisplayTestSuite: Initialized"));
}

//=============================================================================
// Tests Hardware
//=============================================================================

void DisplayTestSuite::clearScreen() {
    Serial.println(F("Clearing screen..."));
    lv_obj_t* screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    bridge_->refreshDisplay();
    Serial.println(F("Screen cleared"));
}

void DisplayTestSuite::runPerformanceBenchmark() {
    Serial.println(F("=== PERFORMANCE BENCHMARK ==="));
    
    // Test simple: mesurer FPS
    unsigned long start = millis();
    int frames = 0;
    
    while (millis() - start < 1000) {
        // Changer couleur de fond
        lv_obj_t* screen = lv_screen_active();
        lv_color_t color = (frames % 2) ? lv_color_white() : lv_color_black();
        lv_obj_set_style_bg_color(screen, color, 0);
        bridge_->refreshDisplay();
        frames++;
    }
    
    Serial.print(F("FPS: "));
    Serial.println(frames);
    Serial.println(F("Performance benchmark completed"));
}

void DisplayTestSuite::runFullHardwareTestSuite() {
    Serial.println(F(""));
    Serial.println(F("################################"));
    Serial.println(F("### FULL HARDWARE TEST SUITE ###"));
    Serial.println(F("################################"));
    
    // Diagnostic initial
    driver_->debugMemory();
    
    // Test 1: Framebuffer direct
    bool test1 = testDirectFramebuffer();
    
    // Test 2: Test LVGL simple
    bool test2 = testSimpleLvgl();
    
    // Test 3: Performance benchmark
    Serial.println(F("Running performance benchmark..."));
    runPerformanceBenchmark();
    
    // Test 4: Endurance (version courte)
    bool test4 = testEndurance(50);
    
    // Résumé final
    Serial.println(F(""));
    Serial.println(F("=== HARDWARE TEST SUMMARY ==="));
    Serial.print(F("Direct Framebuffer: ")); Serial.println(test1 ? "PASS" : "FAIL");
    Serial.print(F("LVGL Simple: ")); Serial.println(test2 ? "PASS" : "FAIL");
    Serial.print(F("Performance: ")); Serial.println("COMPLETED");
    Serial.print(F("Endurance: ")); Serial.println(test4 ? "PASS" : "FAIL");
    
    bool allPassed = test1 && test2 && test4;
    Serial.println(F(""));
    Serial.print(F("### OVERALL RESULT: ")); 
    Serial.println(allPassed ? "ALL TESTS PASSED" : "SOME TESTS FAILED");
    Serial.println(F("################################"));
}

bool DisplayTestSuite::testDirectFramebuffer() {
    Serial.println(F("=== DIRECT FRAMEBUFFER TEST ==="));
    
    uint16_t* framebuffer = driver_->getFramebuffer();
    if (!framebuffer) {
        Serial.println(F("FAILED: No framebuffer access"));
        return false;
    }
    
    Serial.println(F("Drawing test patterns..."));
    
    // Test 1: Rouge
    drawTestPattern(framebuffer, 1);
    driver_->updateFullScreen();
    delay(1000);
    
    // Test 2: Vert
    drawTestPattern(framebuffer, 2);
    driver_->updateFullScreen();
    delay(1000);
    
    // Test 3: Bleu
    drawTestPattern(framebuffer, 3);
    driver_->updateFullScreen();
    delay(1000);
    
    // Clear
    drawTestPattern(framebuffer, 0);
    driver_->updateFullScreen();
    
    Serial.println(F("Direct framebuffer test: PASSED"));
    return true;
}

bool DisplayTestSuite::testAllRotations() {
    Serial.println(F("=== TEST ALL ROTATIONS ==="));
    
    if (!driver_) {
        Serial.println(F("FAILED: No hardware driver"));
        return false;
    }
    
    // Test des 4 rotations
    for (int rotation = 0; rotation < 4; rotation++) {
        Serial.print(F("Testing rotation: "));
        Serial.println(rotation);
        
        driver_->setRotation(rotation);
        
        // Test simple: remplir le framebuffer avec un pattern
        uint16_t* fb = driver_->getFramebuffer();
        if (fb) {
            // Remplir de noir
            for (int i = 0; i < 240 * 320; i++) {
                fb[i] = 0x0000;
            }
            // Dessiner un carré rouge
            for (int y = 10; y < 60; y++) {
                for (int x = 10; x < 60; x++) {
                    if (x < 240 && y < 320) {
                        fb[y * 240 + x] = 0xF800;
                    }
                }
            }
            driver_->updateFullScreen();
        }
        delay(500);
    }
    
    // Retour à rotation par défaut
    driver_->setRotation(0);
    // Effacer l'écran
    uint16_t* fb = driver_->getFramebuffer();
    if (fb) {
        for (int i = 0; i < 240 * 320; i++) {
            fb[i] = 0x0000;
        }
        driver_->updateFullScreen();
    }
    
    Serial.println(F("All rotations test: PASSED"));
    return true;
}

bool DisplayTestSuite::testEndurance(uint16_t cycles) {
    Serial.println(F("=== ENDURANCE TEST ==="));
    Serial.print(F("Running ")); Serial.print(cycles); Serial.println(F(" cycles"));
    
    uint16_t* framebuffer = driver_->getFramebuffer();
    unsigned long startTime = millis();
    
    for (uint16_t cycle = 0; cycle < cycles; cycle++) {
        // Cycle pattern
        drawTestPattern(framebuffer, (cycle % 4) + 1);
        driver_->updateFullScreen();
        
        if (cycle % 10 == 0) {
            Serial.print(F("Cycle ")); Serial.println(cycle);
        }
        
        delay(10);
    }
    
    unsigned long endTime = millis();
    Serial.print(F("Total time: ")); Serial.print(endTime - startTime); Serial.println(F("ms"));
    Serial.println(F("Endurance test: PASSED"));
    return true;
}

//=============================================================================
// Tests LVGL
//=============================================================================

bool DisplayTestSuite::testSimpleLvgl() {
    Serial.println(F("=== SIMPLE LVGL TEST ==="));
    
    if (!bridge_->getLvglDisplay()) {
        Serial.println(F("FAILED: No LVGL display"));
        return false;
    }
    
    // Test écran noir
    lv_obj_t* screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    forceLvglRefresh();
    delay(1000);
    
    // Test écran rouge
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFF0000), 0);
    forceLvglRefresh();
    delay(1000);
    
    // Test label
    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, "LVGL TEST OK");
    lv_obj_center(label);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    forceLvglRefresh();
    delay(2000);
    
    // Nettoyage
    lv_obj_delete(label);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    forceLvglRefresh();
    
    Serial.println(F("Simple LVGL test: PASSED"));
    return true;
}

void DisplayTestSuite::demoScreen() {
    Serial.println(F("Loading demo screen..."));
    lv_obj_t* demo = createTestScreen();
    if (demo) {
        lv_screen_load(demo);
        bridge_->refreshDisplay();
        Serial.println(F("Demo screen loaded"));
    } else {
        Serial.println(F("Failed to create demo screen"));
    }
}

lv_obj_t* DisplayTestSuite::createTestScreen() {
    lv_obj_t* screen = lv_obj_create(NULL);
    
    // Label test
    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, "Test Screen\\nMIDI Controller 2.0");
    lv_obj_center(label);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    
    // Bouton test
    lv_obj_t* btn = lv_button_create(screen);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_set_pos(btn, 100, 150);
    
    lv_obj_t* btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Test Button");
    lv_obj_center(btn_label);
    
    return screen;
}

//=============================================================================
// Tests Widgets
//=============================================================================

bool DisplayTestSuite::testParameterWidget() {
    Serial.println(F("=== PARAMETER WIDGET TEST ==="));
    
    if (!bridge_->getLvglDisplay()) {
        Serial.println(F("FAILED: No LVGL display"));
        return false;
    }
    
    // Créer widget test
    ParameterWidget::Config config = ParameterWidget::getDefaultConfig();
    auto widget = std::make_unique<ParameterWidget>(lv_screen_active(), config);
    
    // Test paramètre
    widget->setParameter(74, 1, 64, "TEST PARAM", false);
    forceLvglRefresh();
    delay(2000);
    
    // Test animation valeur
    for (uint8_t value = 0; value <= 127; value += 1) {
        widget->setValue(value, false);
        forceLvglRefresh();
        delay(16);
    }
    
    Serial.println(F("ParameterWidget test: PASSED"));
    return true;
}

bool DisplayTestSuite::demoParameterWidget() {
    Serial.println(F("=== PARAMETER WIDGET DEMO ==="));
    
    // Créer widget démo
    ParameterWidget::Config config = ParameterWidget::getDefaultConfig();
    config.arc_color = lv_color_hex(0x00FF80);
    config.arc_width = 8;
    // Configuration widget sans anim_duration
    
    auto widget = std::make_unique<ParameterWidget>(lv_screen_active(), config);
    widget->setPosition(100, 60);
    
    // Test paramètres MIDI
    String parameters[] = {"CUTOFF", "RESONANCE", "ATTACK", "DECAY"};
    uint8_t cc_numbers[] = {74, 71, 73, 75};
    
    for (int param = 0; param < 4; param++) {
        Serial.print(F("Testing: ")); Serial.println(parameters[param]);
        
        widget->setParameter(cc_numbers[param], 1, 0, parameters[param], false);
        forceLvglRefresh();
        delay(500);
        
        // Animation 0->127
        for (uint8_t value = 0; value <= 127; value += 1) {
            widget->setValue(value, false);
            forceLvglRefresh();
            delay(16);
        }
        delay(1000);
    }
    
    Serial.println(F("ParameterWidget demo: COMPLETED"));
    return true;
}

//=============================================================================
// Benchmarks
//=============================================================================


void DisplayTestSuite::showMemoryStats() {
    Serial.println(F("=== MEMORY STATS ==="));
    driver_->debugMemory();
    
    // Stats mémoire simple Teensy
    char top;
    uint32_t freeMemory = &top - (char*)0x20000000;
    Serial.print(F("Free RAM estimate: ")); Serial.print(freeMemory); Serial.println(F(" bytes"));
}

//=============================================================================
// Helpers privés
//=============================================================================

void DisplayTestSuite::drawTestPattern(uint16_t* framebuffer, int pattern_type) {
    const int width = 240, height = 320; // Framebuffer toujours en portrait
    
    uint16_t color = 0x0000; // Noir par défaut
    switch (pattern_type) {
        case 1: color = 0xF800; break; // Rouge
        case 2: color = 0x07E0; break; // Vert  
        case 3: color = 0x001F; break; // Bleu
        case 4: // Pattern stripes
            for (int y = 0; y < height; y++) {
                uint16_t line_color = (y % 40 < 20) ? 0xFFFF : 0x0000;
                for (int x = 0; x < width; x++) {
                    framebuffer[y * width + x] = line_color;
                }
            }
            return;
    }
    
    // Couleur uniforme
    for (int i = 0; i < width * height; i++) {
        framebuffer[i] = color;
    }
}

void DisplayTestSuite::forceLvglRefresh() {
    lv_timer_handler();
}