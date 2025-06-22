#pragma once

/**
 * @brief Configuration centralisée de l'écran
 * 
 * Toutes les dimensions et orientations sont définies ici pour assurer
 * la cohérence entre le hardware driver et LVGL.
 */
namespace DisplayConfig {
    // ORIENTATION PRINCIPALE - Changer ici pour tout le système
    static constexpr uint16_t SCREEN_WIDTH = 320;
    static constexpr uint16_t SCREEN_HEIGHT = 240;
    static constexpr uint8_t ROTATION = 1;  // 0=Portrait, 1=Paysage, 2=Portrait inversé, 3=Paysage inversé
    
    
    // FRAMEBUFFER HARDWARE (toujours taille physique native)
    static constexpr size_t FRAMEBUFFER_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;  // Taille physique ILI9341
    static constexpr size_t DIFFBUFFER_SIZE = 4096;  // Taille des buffers de différence
    
    // BUFFERS LVGL (60 lignes de cache selon orientation logique)
    static constexpr size_t LVGL_BUFFER_LINES = 40;
    static constexpr size_t LVGL_BUFFER_SIZE = SCREEN_WIDTH * LVGL_BUFFER_LINES;
    
    // CONFIGURATION PINS
    static constexpr uint8_t CS_PIN = 9;
    static constexpr uint8_t DC_PIN = 10;
    static constexpr uint8_t RST_PIN = 6;
    static constexpr uint8_t MOSI_PIN = 11;
    static constexpr uint8_t SCK_PIN = 13;
    static constexpr uint8_t MISO_PIN = 12;
    static constexpr uint32_t SPI_SPEED = 40000000;  // 40MHz
}