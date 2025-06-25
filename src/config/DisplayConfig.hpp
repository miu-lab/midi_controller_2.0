#pragma once

#include "PerformanceConfig.hpp"
#include "HardwareConfig.hpp"

/**
 * @brief Configuration complète de l'affichage ILI9341
 * 
 * Centralise TOUTE la configuration liée à l'affichage :
 * dimensions, buffers LVGL, pins hardware, performance.
 */
namespace DisplayConfig {

// === DIMENSIONS ÉCRAN ===
static constexpr uint16_t SCREEN_WIDTH = 320;
static constexpr uint16_t SCREEN_HEIGHT = 240;
static constexpr uint8_t ROTATION = 1;  // 0=Portrait, 1=Paysage, 2=Portrait inversé, 3=Paysage inversé

// === BUFFERS HARDWARE ===
static constexpr size_t FRAMEBUFFER_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;  // 76800 pixels
static constexpr size_t DIFFBUFFER_SIZE = 4096;  // Buffer pour différences

// === BUFFERS LVGL ===
static constexpr size_t LVGL_BUFFER_LINES = 60;                               // Lignes par buffer (réduit pour performance)
static constexpr size_t LVGL_BUFFER_SIZE = SCREEN_WIDTH * LVGL_BUFFER_LINES;  // 9600 pixels

// === PERFORMANCE AFFICHAGE ===
static constexpr unsigned long REFRESH_PERIOD_MS = PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS;
static constexpr int REFRESH_RATE_HZ = PerformanceConfig::DISPLAY_REFRESH_RATE_HZ;

// === PINS HARDWARE ILI9341 ===
static constexpr uint8_t CS_PIN = 9;   // Chip Select
static constexpr uint8_t DC_PIN = 10;  // Data/Command
static constexpr uint8_t RST_PIN = 6;  // Reset
static constexpr uint8_t MOSI_PIN = 11; // Master Out Slave In
static constexpr uint8_t SCK_PIN = 13;  // Serial Clock
static constexpr uint8_t MISO_PIN = 12; // Master In Slave Out

// === COMMUNICATION SPI ===
static constexpr uint32_t SPI_SPEED = HardwareConfig::SPI_SPEED_DISPLAY;  // 40MHz

}  // namespace DisplayConfig