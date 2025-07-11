#pragma once

#include "Ili9341Driver.hpp"
#include "config/SystemConstants.hpp"
#include "core/utils/Result.hpp"
#include <lvgl.h>
#include <memory>

/**
 * @brief Pont minimal entre LVGL et Ili9341Driver
 * 
 * Responsabilités :
 * - Configuration LVGL display
 * - Callbacks LVGL vers hardware
 * - Gestion buffers LVGL
 */
class Ili9341LvglBridge {
public:
    /**
     * @brief Configuration LVGL
     */
    struct LvglConfig {
        uint16_t buffer_lines = SystemConstants::Display::LVGL_BUFFER_LINES;  ///< Utilise SystemConstants
        bool double_buffering = true;      ///< Activer double buffering
    };

    /**
     * @brief Constructeur
     * @param driver Driver hardware ILI9341 
     * @param config Configuration LVGL
     */
    explicit Ili9341LvglBridge(std::shared_ptr<Ili9341Driver> driver,
                               const LvglConfig& config = getDefaultLvglConfig());

    /**
     * @brief Destructeur
     */
    ~Ili9341LvglBridge();

    /**
     * @brief Initialise LVGL et connecte au hardware
     * @return Result<void> avec détails d'erreur en cas d'échec
     */
    Result<void> initialize();

    /**
     * @brief Obtient le display LVGL
     */
    lv_display_t* getLvglDisplay() const { return display_; }

    /**
     * @brief Obtient le driver hardware
     */
    std::shared_ptr<Ili9341Driver> getHardwareDriver() const { return driver_; }

    /**
     * @brief Configuration LVGL par défaut optimisée
     */
    static LvglConfig getDefaultLvglConfig();

    /**
     * @brief Force refresh LVGL
     */
    void refreshDisplay();

private:
    // Configuration
    LvglConfig config_;
    std::shared_ptr<Ili9341Driver> driver_;
    bool initialized_;

    // LVGL objets
    lv_display_t* display_;
    lv_color_t* lvgl_buf1_;
    lv_color_t* lvgl_buf2_;

    /**
     * @brief Configure LVGL global (tick, mémoire)
     */
    bool setupLvglCore();

    /**
     * @brief Configure display LVGL et buffers
     */
    bool setupLvglDisplay();

    /**
     * @brief Alloue buffers LVGL en DMAMEM
     */
    bool allocateLvglBuffers();

    /**
     * @brief Libère buffers LVGL
     */
    void freeLvglBuffers();

    // === CALLBACKS LVGL STATIQUES ===

    /**
     * @brief Callback flush LVGL vers hardware
     */
    static void flush_callback(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);

    /**
     * @brief Récupère instance depuis display LVGL
     */
    static Ili9341LvglBridge* getInstance(lv_display_t* disp);
};