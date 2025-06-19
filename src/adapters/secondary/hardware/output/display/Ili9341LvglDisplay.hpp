#pragma once

#include "core/utils/DisplayProfiler.hpp"

#include <ILI9341_T4.h>
#include <lvgl.h>
#include <memory>

/**
 * @brief Driver hardware LVGL + ILI9341_t4 pur
 * 
 * Cette classe fournit uniquement l'interface hardware pour LVGL.
 * Architecture: LVGL (rendu) → Buffer LVGL → ILI9341_t4 (DMA) → Écran
 */
class Ili9341LvglDisplay {
public:
    /**
     * @brief Configuration hardware pour ILI9341
     */
    struct Config {
        uint8_t cs_pin = 9;            ///< SPI Chip Select
        uint8_t dc_pin = 10;           ///< Data/Command 
        uint8_t rst_pin = 6;           ///< Reset
        uint8_t mosi_pin = 11;         ///< SPI MOSI
        uint8_t sck_pin = 13;          ///< SPI Clock
        uint8_t miso_pin = 12;         ///< SPI MISO
        uint32_t spi_speed = 40000000; ///< Vitesse SPI en Hz (40MHz optimal)
        uint8_t rotation = 0;          ///< Rotation écran (0-3)
    };

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration des pins et paramètres
     */
    explicit Ili9341LvglDisplay(const Config& config = getDefaultConfig());

    /**
     * @brief Destructeur
     */
    ~Ili9341LvglDisplay();

    // === INTERFACE LVGL HARDWARE ===

    /**
     * @brief Initialise l'écran et LVGL
     * @return true si l'initialisation a réussi
     */
    bool init();

    /**
     * @brief Obtient le display LVGL pour utilisation directe
     * @return Pointeur vers le display LVGL
     */
    lv_display_t* getLvglDisplay() const { return display_; }

    /**
     * @brief Définit la rotation de l'écran
     * @param rotation 0-3 pour les différentes orientations
     */
    void setRotation(uint8_t rotation);

    /**
     * @brief Obtient les dimensions actuelles
     */
    void getDimensions(uint16_t& width, uint16_t& height) const;

    /**
     * @brief Obtient le type d'affichage
     */
    const char* getDisplayType() const {
        return "ILI9341_LVGL";
    }

    // === PERFORMANCE ===

    void getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime,
                             unsigned long& minTime) const;

    /**
     * @brief Configuration par défaut optimisée pour Teensy 4.1
     */
    static Config getDefaultConfig();

    /**
     * @brief Créer une vue LVGL de test simple (temporaire pour validation)
     * @return Pointeur vers l'écran LVGL créé
     */
    lv_obj_t* createTestScreen();

    /**
     * @brief Test ParameterWidget pour validation Phase 2
     * @return true si le test réussit
     */
    bool testParameterWidget();

    /**
     * @brief Démo visuelle interactive du ParameterWidget
     * @return true si le test réussit
     */
    bool demoParameterWidget();

    /**
     * @brief Ajouter debug mémoire (temporaire pour Phase 1)
     */
    void debugMemory() const;

    /**
     * @brief Profiler pour zones de rendu (Phase 1)
     */
    class FlushProfiler {
    private:
        uint32_t total_pixels_updated_;
        uint32_t flush_count_;
        uint32_t full_screen_updates_;
        uint32_t partial_updates_;
        unsigned long total_flush_time_;
        unsigned long max_flush_time_;
        unsigned long min_flush_time_;
        
    public:
        FlushProfiler() : total_pixels_updated_(0), flush_count_(0), 
                         full_screen_updates_(0), partial_updates_(0),
                         total_flush_time_(0), max_flush_time_(0), min_flush_time_(ULONG_MAX) {}
        
        void recordFlush(const lv_area_t* area, unsigned long duration);
        void printStats() const;
        void reset();
        float getAveragePixelsPerFlush() const;
        float getAverageFlushTime() const;
    };
    
    /**
     * @brief Obtient le profiler de flush (temporaire Phase 1)
     */
    FlushProfiler& getFlushProfiler() { return flush_profiler_; }

    /**
     * @brief Benchmark performance patterns (temporaire Phase 1)
     */
    void runPerformanceBenchmark();

    /**
     * @brief Tests hardware robustesse (Phase 1)
     */
    bool testMultipleInit();
    bool testAllRotations(); 
    bool testEndurance(uint16_t cycles = 1000);
    void runFullHardwareTestSuite();

private:
    // Hardware et configuration
    Config config_;
    bool initialized_;

    // ILI9341_T4 driver pour SPI optimisé
    std::unique_ptr<ILI9341_T4::ILI9341Driver> tft_;

    // Buffers selon modèle ILI9341_t4
    uint16_t* framebuffer_;  // Buffer principal 240x320 (alloué en DMAMEM)

    // Diff buffers pour performance ILI9341_t4 (déplacés en EXTMEM)
    std::unique_ptr<ILI9341_T4::DiffBuff> diff1_;
    std::unique_ptr<ILI9341_T4::DiffBuff> diff2_;

    // LVGL
    lv_display_t* display_;
    lv_draw_buf_t draw_buf_;
    lv_color_t* lvgl_buf1_;  // Buffer LVGL ligne par ligne (40 lignes)
    lv_color_t* lvgl_buf2_;  // Double buffer LVGL

    // Dimensions écran ILI9341
    static constexpr int SCREEN_WIDTH = 240;
    static constexpr int SCREEN_HEIGHT = 320;
    static constexpr int LVGL_BUFFER_LINES = 40;  // Nombre de lignes dans le buffer LVGL

    // Performance tracking
    DisplayProfiler profiler_;
    FlushProfiler flush_profiler_;  // Tracking zones de rendu (Phase 1)

    // === CALLBACKS LVGL STATIQUES ===

    /**
     * @brief Callback de flush LVGL - transfère le buffer vers l'écran
     * @param disp Display LVGL
     * @param area Zone à transférer
     * @param px_map Buffer de pixels
     */
    static void flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);

    /**
     * @brief Callback de rounder LVGL - ajuste les zones de rendu
     * @param disp Display LVGL
     * @param area Zone à ajuster
     */
    static void rounder_cb(lv_display_t* disp, lv_area_t* area);

    // === MÉTHODES PRIVÉES ===

    /**
     * @brief Alloue les buffers en DMAMEM
     */
    bool allocateBuffers();

    /**
     * @brief Libère les buffers
     */
    void freeBuffers();

    /**
     * @brief Configure LVGL avec le display
     */
    bool setupLvgl();

    /**
     * @brief Convertit couleur mono (0/1) vers lv_color_t
     */
    lv_color_t convertMonoColor(uint16_t monoColor) const;

    /**
     * @brief Met à jour les dimensions selon la rotation
     */
    void updateDimensions();

    /**
     * @brief Obtient l'instance depuis le display LVGL
     */
    static Ili9341LvglDisplay* getInstance(lv_display_t* disp);
};