#pragma once

#include "core/ports/output/DisplayPort.hpp"
#include "core/utils/DisplayProfiler.hpp"

#include <ILI9341_T4.h>
#include <lvgl.h>
#include <memory>

/**
 * @brief Implémentation LVGL + ILI9341_t4 du DisplayPort
 * 
 * Cette classe utilise LVGL pour le rendu et ILI9341_t4 pour le transfert hardware optimisé.
 * Architecture: LVGL (rendu) → Buffer LVGL → ILI9341_t4 (DMA) → Écran
 */
class Ili9341LvglDisplay : public DisplayPort {
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
    ~Ili9341LvglDisplay() override;

    // === INTERFACE DisplayPort ===

    /**
     * @brief Initialise l'écran et LVGL
     * @return true si l'initialisation a réussi
     */
    bool init();

    void clear() override;
    void drawText(int x, int y, const char* text) override;
    void drawLine(int x0, int y0, int x1, int y1) override;
    void drawRect(int x, int y, int width, int height, bool fill = false) override;
    void drawCircle(int x, int y, int radius, bool fill = false) override;
    void update() override;
    void setTextSize(uint8_t size) override;
    void setTextColor(uint16_t color) override;
    void setTextWrap(bool wrap) override;
    void setCursor(int16_t x, int16_t y) override;
    void getTextBounds(const char* text, uint16_t* w, uint16_t* h) override;
    void drawCenteredText(int x, int y, const char* text) override;
    void drawFormattedText(int x, int y, const char* format, ...) override;

    // === IDENTIFICATION ===

    const char* getDisplayType() const override {
        return "ILI9341_LVGL";
    }

    bool isSsd1306Display() const override {
        return false;
    }

    // === PERFORMANCE ===

    void getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime,
                             unsigned long& minTime) const override;

    /**
     * @brief Configuration par défaut optimisée pour Teensy 4.1
     */
    static Config getDefaultConfig();

    // === MÉTHODES ÉTENDUES LVGL ===

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
     * @brief Créer une vue LVGL de test simple
     * @return Pointeur vers l'écran LVGL créé
     */
    lv_obj_t* createTestScreen();

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

    // État du texte (pour compatibilité DisplayPort)
    uint16_t currentTextColor_;
    uint8_t currentTextSize_;
    int16_t currentCursorX_, currentCursorY_;
    bool textWrap_;

    // Performance tracking
    DisplayProfiler profiler_;

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