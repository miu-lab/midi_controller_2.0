#include "Ili9341TgxDisplay.hpp"

#include <Arduino.h>
#include <stdarg.h>

#include "core/utils/FlashStrings.hpp"

// Buffer interne en DMAMEM (pour l'écran)
DMAMEM static uint16_t internal_framebuffer[240 * 320];

//=============================================================================
// Configuration et constructeur
//=============================================================================

Ili9341TgxDisplay::Config Ili9341TgxDisplay::getDefaultConfig() {
    return {
        .cs_pin = 9,            // CS sur pin 9
        .dc_pin = 10,           // DC sur pin 10
        .rst_pin = 6,           // RST sur pin 6 (comme code fonctionnel)
        .mosi_pin = 11,         // MOSI sur pin 11 (SPI standard)
        .sck_pin = 13,          // SCK sur pin 13 (SPI standard)
        .miso_pin = 12,         // MISO sur pin 12 (SPI standard)
        .spi_speed = 30000000,  // 30MHz - optimal pour Teensy 4.1
        .rotation = 0           // Portrait (comme code fonctionnel)
    };
}

Ili9341TgxDisplay::Ili9341TgxDisplay(const Config& config)
    : config_(config),
      initialized_(false),
      internalBuffer_(internal_framebuffer),
      currentTextColor_(tgx::RGB565_White),
      currentTextSize_(1),
      currentCursorX_(0),
      currentCursorY_(0),
      textWrap_(false) {}

//=============================================================================
// Initialisation
//=============================================================================

bool Ili9341TgxDisplay::init() {
    if (initialized_) {
        return true;
    }

    Serial.println(F("ILI9341_TGX: Initializing display with TGX graphics..."));

    // Créer le driver ILI9341_T4
    tft_ = std::make_unique<ILI9341_T4::ILI9341Driver>(config_.cs_pin,
                                                       config_.dc_pin,
                                                       config_.sck_pin,
                                                       config_.mosi_pin,
                                                       config_.miso_pin,
                                                       config_.rst_pin);

    if (!tft_) {
        Serial.println(F("ILI9341_TGX: Failed to create ILI9341Driver"));
        return false;
    }

    // Initialiser le driver (comme code fonctionnel)
    tft_->begin();

    // Configuration selon le modèle fonctionnel
    tft_->setRotation(config_.rotation);
    tft_->setFramebuffer(internalBuffer_);  // ⚠️ CRITIQUE !

    // Créer les diff buffers
    diff1_ = std::make_unique<ILI9341_T4::DiffBuffStatic<4096>>();
    diff2_ = std::make_unique<ILI9341_T4::DiffBuffStatic<4096>>();
    tft_->setDiffBuffers(diff1_.get(), diff2_.get());

    // Configuration performance
    tft_->setRefreshRate(120);
    tft_->setVSyncSpacing(2);

    // Créer le canvas TGX autour du buffer de dessin
    canvas_ = std::make_unique<tgx::Image<tgx::RGB565>>((tgx::RGB565*)internalBuffer_,
                                                        240,   // width
                                                        320);  // height

    if (!canvas_) {
        Serial.println(F("ILI9341_TGX: Failed to create TGX canvas"));
        return false;
    }

    // Test initial - effacer l'écran
    canvas_->fillScreen(tgx::RGB565_Black);
    canvas_->drawCircle({120, 160}, 50, tgx::RGB565_White);
    tft_->update(internalBuffer_);  // Utiliser internalBuffer pour update

    initialized_ = true;

    Serial.print(F("ILI9341_TGX: Display initialized successfully ("));
    uint16_t w, h;
    getDimensions(w, h);
    Serial.print(w);
    Serial.print(F("x"));
    Serial.print(h);
    Serial.println(F(") with TGX acceleration"));

    return true;
}

//=============================================================================
// Interface DisplayPort - Méthodes de base
//=============================================================================

void Ili9341TgxDisplay::clear() {
    if (!initialized_ || !canvas_) return;

    unsigned long startTime = micros();

    canvas_->fillScreen(tgx::RGB565_Black);
    currentCursorX_ = 0;
    currentCursorY_ = 0;

    unsigned long endTime = micros();
    profiler_.recordUpdate(endTime - startTime);
}

void Ili9341TgxDisplay::drawText(int x, int y, const char* text) {
    if (!initialized_ || !canvas_ || !text) return;

    // TGX nécessite une font - pour simplifier, on dessine pixel par pixel
    // ou on utilise une approche alternative
    // Pour l'instant, utilisons drawPixel pour dessiner du texte basique
    int startX = x;
    int currentX = x;
    int currentY = y;

    // Dessin simple lettre par lettre (approximation)
    for (const char* p = text; *p; p++) {
        char c = *p;
        if (c == '\n') {
            currentX = startX;
            currentY += 12;  // Hauteur approximative
            continue;
        }

        // Dessiner un rectangle simple pour chaque caractère (placeholder)
        tgx::iBox2 charRect = {currentX, currentX + 5, currentY, currentY + 7};
        canvas_->drawRect(charRect, currentTextColor_);

        currentX += 6;  // Largeur approximative
    }

    // Mettre à jour la position du curseur si textWrap activé
    if (textWrap_) {
        currentCursorX_ = currentX;
        currentCursorY_ = currentY;
    }
}

void Ili9341TgxDisplay::drawLine(int x0, int y0, int x1, int y1) {
    if (!initialized_ || !canvas_) return;
    canvas_->drawLine({x0, y0}, {x1, y1}, currentTextColor_);
}

void Ili9341TgxDisplay::drawRect(int x, int y, int width, int height, bool fill) {
    if (!initialized_ || !canvas_) return;

    tgx::iBox2 rect = {x, x + width - 1, y, y + height - 1};

    if (fill) {
        canvas_->fillRect(rect, currentTextColor_);
    } else {
        canvas_->drawRect(rect, currentTextColor_);
    }
}

void Ili9341TgxDisplay::drawCircle(int x, int y, int radius, bool fill) {
    if (!initialized_ || !canvas_) return;

    if (fill) {
        // fillCircle nécessite interior_color ET outline_color
        canvas_->fillCircle({x, y}, radius, currentTextColor_, currentTextColor_);
    } else {
        // drawCircle doit utiliser une méthode différente
        canvas_->drawCircle({x, y}, radius, currentTextColor_);
    }
}

void Ili9341TgxDisplay::drawCircle(int x, int y, int radius, bool fill, uint16_t color) {
    if (!initialized_ || !canvas_) return;

    auto tgxColor = convertToTgxColor(color);

    if (fill) {
        // fillCircle nécessite interior_color ET outline_color
        canvas_->fillCircle({x, y}, radius, tgxColor, tgxColor);
    } else {
        canvas_->drawCircle({x, y}, radius, tgxColor);
    }
}

void Ili9341TgxDisplay::update() {
    if (!initialized_ || !tft_) return;

    unsigned long startTime = micros();

    // Utiliser le buffer de dessin pour l'update (comme code fonctionnel)
    tft_->update(internalBuffer_);

    unsigned long endTime = micros();
    profiler_.recordUpdate(endTime - startTime);
}

//=============================================================================
// Interface DisplayPort - Configuration du texte
//=============================================================================

void Ili9341TgxDisplay::setTextSize(uint8_t size) {
    if (!initialized_) return;
    currentTextSize_ = size;
}

void Ili9341TgxDisplay::setTextColor(uint16_t color) {
    if (!initialized_) return;
    currentTextColor_ = convertMonoColor(color);
}

void Ili9341TgxDisplay::setTextWrap(bool wrap) {
    if (!initialized_) return;
    textWrap_ = wrap;
}

void Ili9341TgxDisplay::setCursor(int16_t x, int16_t y) {
    if (!initialized_) return;
    currentCursorX_ = x;
    currentCursorY_ = y;
}

void Ili9341TgxDisplay::getTextBounds(const char* text, uint16_t* w, uint16_t* h) {
    if (!initialized_ || !text || !w || !h) {
        if (w) *w = 0;
        if (h) *h = 0;
        return;
    }

    // Estimation basique basée sur la taille du texte
    // TGX a des méthodes plus précises mais nécessitent une font spécifique
    int len = strlen(text);
    *w = len * currentTextSize_ * 6;  // Approximation
    *h = currentTextSize_ * 8;        // Approximation
}

//=============================================================================
// Interface DisplayPort - Méthodes avancées
//=============================================================================

void Ili9341TgxDisplay::drawCenteredText(int x, int y, const char* text) {
    if (!initialized_ || !canvas_ || !text) return;

    // Calculer les dimensions du texte
    uint16_t w, h;
    getTextBounds(text, &w, &h);

    // Centrer horizontalement
    int centeredX = x - (w / 2);

    drawText(centeredX, y, text);
}

void Ili9341TgxDisplay::drawFormattedText(int x, int y, const char* format, ...) {
    if (!initialized_ || !format) return;

    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    drawText(x, y, buffer);
}

//=============================================================================
// Performance et identification
//=============================================================================

void Ili9341TgxDisplay::getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime,
                                            unsigned long& minTime) const {
    avgTime = profiler_.getAverageUpdateTime();
    maxTime = profiler_.getMaxUpdateTime();
    minTime = profiler_.getMinUpdateTime();
}

//=============================================================================
// Méthodes étendues spécifiques
//=============================================================================

void Ili9341TgxDisplay::setRotation(uint8_t rotation) {
    if (!initialized_ || !tft_) return;

    config_.rotation = rotation;
    // Note: ILI9341_T4 gère la rotation différemment de ILI9341_t3
    // Il faut possiblement recréer le canvas avec nouvelles dimensions

    Serial.print(F("ILI9341_TGX: Rotation set to "));
    Serial.println(rotation);
}

void Ili9341TgxDisplay::getDimensions(uint16_t& width, uint16_t& height) const {
    // Dimensions selon rotation (modèle ILI9341_T4: 240x320)
    if (config_.rotation == 1 || config_.rotation == 3) {
        width = 320;  // Paysage
        height = 240;
    } else {
        width = 240;  // Portrait
        height = 320;
    }
}

void Ili9341TgxDisplay::drawProgressBar(int x, int y, int width, int height, int value,
                                        int maxValue) {
    if (!initialized_ || !canvas_) return;

    // Calculer la largeur de la barre remplie
    int fillWidth = (value * width) / maxValue;
    fillWidth = constrain(fillWidth, 0, width);

    // Fond de la barre (noir)
    tgx::iBox2 bgRect = {x, x + width - 1, y, y + height - 1};
    canvas_->fillRect(bgRect, tgx::RGB565_Black);

    // Partie remplie (vert pour valeurs normales, rouge pour valeurs élevées)
    if (fillWidth > 0) {
        tgx::iBox2 fillRect = {x, x + fillWidth - 1, y, y + height - 1};
        tgx::RGB565 fillColor = (value > maxValue * 0.8) ? tgx::RGB565_Red : tgx::RGB565_Green;
        canvas_->fillRect(fillRect, fillColor);
    }

    // Contour de la barre (blanc)
    tgx::iBox2 borderRect = {x, x + width - 1, y, y + height - 1};
    canvas_->drawRect(borderRect, tgx::RGB565_White);
}

void Ili9341TgxDisplay::drawTextWithSize(int x, int y, const char* text, uint8_t size) {
    if (!initialized_) return;

    uint8_t oldSize = currentTextSize_;
    setTextSize(size);
    drawText(x, y, text);
    setTextSize(oldSize);
}

//=============================================================================
// Méthodes privées
//=============================================================================

tgx::RGB565 Ili9341TgxDisplay::convertMonoColor(uint16_t monoColor) const {
    // Conversion des couleurs monochromes (0/1) vers RGB565 TGX
    switch (monoColor) {
    case 0:
        return tgx::RGB565_Black;
    case 1:
    default:
        return tgx::RGB565_White;
    }
}

tgx::RGB565 Ili9341TgxDisplay::convertToTgxColor(uint16_t color) const {
    // Convertir RGB565 brut vers format TGX
    return tgx::RGB565(color);
}

const void* Ili9341TgxDisplay::getFontForSize(int size) const {
    // Retourner des fonts TGX selon la taille demandée
    // Note: Nécessite l'inclusion de fonts spécifiques de tgx-font
    // Pour l'instant, retourner nullptr et utiliser la font par défaut

    switch (size) {
    case 1:
        // return &font_tgx_Arial_8; // Si disponible
        break;
    case 2:
        // return &font_tgx_Arial_12; // Si disponible
        break;
    case 3:
        // return &font_tgx_Arial_16; // Si disponible
        break;
    default:
        break;
    }

    return nullptr;  // Utiliser font par défaut TGX
}
