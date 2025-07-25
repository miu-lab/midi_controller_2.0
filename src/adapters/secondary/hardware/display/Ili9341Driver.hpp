#pragma once

#include <ILI9341_T4.h>

#include <memory>

#include "config/SystemConstants.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Driver hardware pur pour écran ILI9341 via ILI9341_T4
 * 
 * Responsabilités :
 * - Configuration SPI et pins
 * - Gestion framebuffer et diff buffers
 * - Interface hardware uniquement (pas de LVGL)
 */
class Ili9341Driver {
public:
    /**
     * @brief Configuration hardware pour ILI9341
     */
    struct Config {
        uint8_t cs_pin = SystemConstants::Display::CS_PIN;         ///< SPI Chip Select
        uint8_t dc_pin = SystemConstants::Display::DC_PIN;         ///< Data/Command
        uint8_t rst_pin = SystemConstants::Display::RST_PIN;       ///< Reset
        uint8_t mosi_pin = SystemConstants::Display::MOSI_PIN;     ///< SPI MOSI
        uint8_t sck_pin = SystemConstants::Display::SCK_PIN;       ///< SPI Clock
        uint8_t miso_pin = SystemConstants::Display::MISO_PIN;     ///< SPI MISO
        uint32_t spi_speed = SystemConstants::Display::SPI_SPEED;  ///< Vitesse SPI en Hz
        uint8_t rotation = SystemConstants::Display::ROTATION;     ///< Rotation écran (0-3)
    };

    /**
     * @brief Constructeur avec configuration
     */
    explicit Ili9341Driver(const Config& config = getDefaultConfig());

    /**
     * @brief Destructeur
     */
    ~Ili9341Driver() = default;

    /**
     * @brief Initialise le hardware
     * @return Result<void> avec détails d'erreur en cas d'échec
     */
    Result<void> initialize();

    /**
     * @brief Met à jour une région de l'écran
     * @param redraw_now Forcer redraw immédiat
     * @param pixels Buffer pixels RGB565
     * @param x1,x2,y1,y2 Coordonnées région
     */
    void updateRegion(bool redraw_now, uint16_t* pixels, 
                      int x1, int x2, int y1, int y2);

    /**
     * @brief Force update complet framebuffer
     */
    void updateFullScreen();

    /**
     * @brief Change rotation écran
     * @param rotation 0-3
     */
    void setRotation(uint8_t rotation);

    /**
     * @brief Obtient dimensions selon rotation
     */
    void getDimensions(uint16_t& width, uint16_t& height) const;

    /**
     * @brief Configuration par défaut optimisée
     */
    static Config getDefaultConfig();

    /**
     * @brief Accès au framebuffer pour tests directs
     */
    uint16_t* getFramebuffer() const { return framebuffer_; }

    /**
     * @brief Diagnostics mémoire
     */
    void debugMemory() const;

private:
    Config config_;
    bool initialized_;

    // ILI9341_T4 driver
    std::unique_ptr<ILI9341_T4::ILI9341Driver> tft_;

    // Buffers hardware
    uint16_t* framebuffer_;  ///< Framebuffer principal DMAMEM
    std::unique_ptr<ILI9341_T4::DiffBuff> diff1_;
    std::unique_ptr<ILI9341_T4::DiffBuff> diff2_;

    /**
     * @brief Configure les pins hardware
     */
    void configurePins();

    /**
     * @brief Configure les buffers de différence
     */
    void setupDiffBuffers();

    /**
     * @brief Configure les paramètres de performance
     */
    void setupPerformance();
};