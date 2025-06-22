#pragma once

#include "adapters/secondary/hardware/display/Ili9341Driver.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/ui/lvgl/widgets/ParameterWidget.hpp"
#include <memory>

/**
 * @brief Suite de tests pour l'affichage et l'UI
 * 
 * Responsabilités :
 * - Tests hardware display
 * - Tests LVGL
 * - Tests widgets
 * - Benchmarks performance
 */
class DisplayTestSuite {
public:
    /**
     * @brief Constructeur
     * @param driver Driver hardware
     * @param bridge Pont LVGL
     */
    explicit DisplayTestSuite(std::shared_ptr<Ili9341Driver> driver,
                             std::shared_ptr<Ili9341LvglBridge> bridge);

    /**
     * @brief Destructeur
     */
    ~DisplayTestSuite() = default;

    // === TESTS HARDWARE ===

    /**
     * @brief Suite complète tests hardware
     */
    void runFullHardwareTestSuite();
    
    /**
     * @brief Efface l'écran
     */
    void clearScreen();
    
    /**
     * @brief Benchmark de performance
     */
    void runPerformanceBenchmark();

    /**
     * @brief Test framebuffer direct (sans LVGL)
     */
    bool testDirectFramebuffer();

    /**
     * @brief Test rotations écran
     */
    bool testAllRotations();

    /**
     * @brief Test endurance hardware
     */
    bool testEndurance(uint16_t cycles = 100);

    // === TESTS LVGL ===

    /**
     * @brief Test LVGL simple
     */
    bool testSimpleLvgl();

    /**
     * @brief Crée écran test LVGL
     */
    lv_obj_t* createTestScreen();
    
    /**
     * @brief Charge un écran de démo
     */
    void demoScreen();

    // === TESTS WIDGETS ===

    /**
     * @brief Test ParameterWidget basique
     */
    bool testParameterWidget();

    /**
     * @brief Démo visuelle ParameterWidget
     */
    bool demoParameterWidget();

    // === BENCHMARKS ===

    /**
     * @brief Affiche statistiques mémoire
     */
    void showMemoryStats();

private:
    std::shared_ptr<Ili9341Driver> driver_;
    std::shared_ptr<Ili9341LvglBridge> bridge_;

    /**
     * @brief Helper : affiche pattern test sur framebuffer direct
     */
    void drawTestPattern(uint16_t* framebuffer, int pattern_type);

    /**
     * @brief Helper : force refresh LVGL
     */
    void forceLvglRefresh();
};