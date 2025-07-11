#include "Ili9341Driver.hpp"

#include <Arduino.h>

#include "config/SystemConstants.hpp"

// Buffers statiques DMAMEM - Taille selon orientation configurée
DMAMEM static uint16_t main_framebuffer[SystemConstants::Display::FRAMEBUFFER_SIZE];

// Buffers de différence statiques DMAMEM pour optimisation (4KB chacun)
DMAMEM static uint8_t diffbuffer1[SystemConstants::Display::DIFFBUFFER_SIZE];
DMAMEM static uint8_t diffbuffer2[SystemConstants::Display::DIFFBUFFER_SIZE];

//=============================================================================
// Configuration et constructeur
//=============================================================================

Ili9341Driver::Config Ili9341Driver::getDefaultConfig() {
    return {.cs_pin = SystemConstants::Display::CS_PIN,
            .dc_pin = SystemConstants::Display::DC_PIN,
            .rst_pin = SystemConstants::Display::RST_PIN,
            .mosi_pin = SystemConstants::Display::MOSI_PIN,
            .sck_pin = SystemConstants::Display::SCK_PIN,
            .miso_pin = SystemConstants::Display::MISO_PIN,
            .spi_speed = SystemConstants::Display::SPI_SPEED,
            .rotation = SystemConstants::Display::ROTATION};
}

Ili9341Driver::Ili9341Driver(const Config& config)
    : config_(config), initialized_(false), framebuffer_(main_framebuffer) {
    // TODO DEBUG MSG
    // TODO DEBUG MSG
}

//=============================================================================
// Initialisation hardware
//=============================================================================

Result<void> Ili9341Driver::initialize() {
    if (initialized_) {
        Serial.println("Ili9341Driver already initialized");
        return Result<void>::success();
    }

    Serial.println("Ili9341Driver: Starting initialization...");

    // Configuration des pins
    Serial.println("Ili9341Driver: Configuring pins...");
    configurePins();

    // Créer le driver ILI9341_T4
    tft_ = std::make_unique<ILI9341_T4::ILI9341Driver>(config_.cs_pin,
                                                       config_.dc_pin,
                                                       config_.sck_pin,
                                                       config_.mosi_pin,
                                                       config_.miso_pin,
                                                       config_.rst_pin);

    if (!tft_) {
        Serial.println("ERROR: Ili9341Driver - Failed to create ILI9341_T4 driver");
        return Result<void>::error(
            {ErrorCode::HardwareError, "Failed to create ILI9341_T4 driver"});
    }

    // Initialiser le driver
    Serial.println("Ili9341Driver: Initializing TFT driver...");
    tft_->begin(config_.spi_speed);
    tft_->setRotation(config_.rotation);
    // IMPORTANT: Donner au driver son propre framebuffer interne
    tft_->setFramebuffer(framebuffer_);

    // Configurer les diff buffers pour optimisation
    Serial.println("Ili9341Driver: Setting up diff buffers...");
    setupDiffBuffers();

    // Configurer les paramètres de performance
    Serial.println("Ili9341Driver: Configuring performance settings...");
    setupPerformance();

    Serial.println("Ili9341Driver: Initialization completed successfully");
    initialized_ = true;

    return Result<void>::success();
}

void Ili9341Driver::configurePins() {
    pinMode(config_.cs_pin, OUTPUT);
    pinMode(config_.dc_pin, OUTPUT);
    if (config_.rst_pin != 255) {
        pinMode(config_.rst_pin, OUTPUT);
        // Reset de l'écran
        digitalWrite(config_.rst_pin, LOW);
        digitalWrite(config_.rst_pin, HIGH);
    }
}

void Ili9341Driver::setupDiffBuffers() {
    // Créer les diff buffers pour optimisation (alloués en DMAMEM)
    diff1_ = std::make_unique<ILI9341_T4::DiffBuff>(diffbuffer1, sizeof(diffbuffer1));
    diff2_ = std::make_unique<ILI9341_T4::DiffBuff>(diffbuffer2, sizeof(diffbuffer2));
    tft_->setDiffBuffers(diff1_.get(), diff2_.get());
}

void Ili9341Driver::setupPerformance() {
    // Configuration performance optimisée (selon exemple officiel)
    tft_->setDiffGap(SystemConstants::Performance::DIFF_GAP);            // gap petit avec buffers diff 4K
    tft_->setVSyncSpacing(SystemConstants::Performance::VSYNC_SPACING);  // minimiser tearing
    tft_->setRefreshRate(
        SystemConstants::Performance::DISPLAY_REFRESH_RATE_HZ);  // 100Hz pour performance élevée
}

//=============================================================================
// Méthodes d'affichage
//=============================================================================

void Ili9341Driver::updateRegion(bool redraw_now, uint16_t* pixels, int x1, int x2, int y1,
                                 int y2) {
    if (!initialized_ || !tft_) {
        return;
    }

    tft_->updateRegion(redraw_now, pixels, x1, x2, y1, y2);
}

void Ili9341Driver::updateFullScreen() {
    if (!initialized_ || !tft_) {
        return;
    }

    tft_->update(framebuffer_);
}

//=============================================================================
// Configuration et propriétés
//=============================================================================

void Ili9341Driver::setRotation(uint8_t rotation) {
    if (!initialized_ || !tft_) return;

    config_.rotation = rotation;
    tft_->setRotation(rotation);

    // TODO DEBUG MSG
}

void Ili9341Driver::getDimensions(uint16_t& width, uint16_t& height) const {
    // Dimensions selon rotation
    if (config_.rotation == 1 || config_.rotation == 3) {
        width = SystemConstants::Display::SCREEN_WIDTH;  // Paysage (90° et 270°)
        height = SystemConstants::Display::SCREEN_HEIGHT;
    } else {
        width = SystemConstants::Display::SCREEN_HEIGHT;  // Portrait (0° et 180°)
        height = SystemConstants::Display::SCREEN_WIDTH;
    }
}

//=============================================================================
// Debug et diagnostics
//=============================================================================

void Ili9341Driver::debugMemory() const {
    uint16_t width, height;
    getDimensions(width, height);
}