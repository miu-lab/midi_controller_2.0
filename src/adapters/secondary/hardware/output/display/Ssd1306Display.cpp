#include "Ssd1306Display.hpp"

#include <Arduino.h>
#include <stdarg.h>

#include "core/utils/AppStrings.hpp"
#include "core/utils/FlashStrings.hpp"

Ssd1306Display::Ssd1306Display(uint16_t width, uint16_t height, uint8_t i2cAddress,
                               TwoWire& wireInstance)
    : display_(width, height, &wireInstance, -1),
      width_(width),
      height_(height),
      i2cAddress_(i2cAddress),
      wireInstance_(wireInstance),
      initialized_(false) {}

bool Ssd1306Display::init(int8_t resetPin) {
    if (initialized_) {
        return true;
    }

    Serial.print(F("SSD1306:"));
    Serial.println(F(" Initializing display..."));

    if (!display_.begin(SSD1306_SWITCHCAPVCC, i2cAddress_, resetPin)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }

    Serial.print(F("SSD1306:"));
    Serial.println(F(" Display initialized successfully"));

    display_.clearDisplay();
    display_.display();
    display_.clearDisplay();
    display_.display();

    display_.setTextColor(SSD1306_WHITE);
    display_.setTextSize(1);
    display_.setCursor(0, 0);

    initialized_ = true;
    isDirty_ = true;

    display_.clearDisplay();
    display_.display();

    Serial.print(F("SSD1306:"));
    Serial.println(F(" Display ready"));
    return true;
}

void Ssd1306Display::clear() {
    if (!initialized_) return;

    display_.clearDisplay();
    display_.setCursor(0, 0);
    isDirty_ = true;
}

void Ssd1306Display::drawText(int x, int y, const char* text) {
    if (!initialized_) return;
    display_.setCursor(x, y);
    display_.print(text);
    isDirty_ = true;
}

void Ssd1306Display::drawLine(int x0, int y0, int x1, int y1) {
    if (!initialized_) return;
    display_.drawLine(x0, y0, x1, y1, SSD1306_WHITE);
    isDirty_ = true;
}

void Ssd1306Display::drawRect(int x, int y, int width, int height, bool fill) {
    if (!initialized_) return;
    if (fill) {
        display_.fillRect(x, y, width, height, SSD1306_WHITE);
    } else {
        display_.drawRect(x, y, width, height, SSD1306_WHITE);
    }
    isDirty_ = true;
}

void Ssd1306Display::drawCircle(int x, int y, int radius, bool fill) {
    if (!initialized_) return;
    if (fill) {
        display_.fillCircle(x, y, radius, SSD1306_WHITE);
    } else {
        display_.drawCircle(x, y, radius, SSD1306_WHITE);
    }
    isDirty_ = true;
}

void Ssd1306Display::drawCircle(int x, int y, int radius, bool fill, uint16_t color) {
    if (!initialized_) return;
    if (fill) {
        display_.fillCircle(x, y, radius, color);
    } else {
        display_.drawCircle(x, y, radius, color);
    }
    isDirty_ = true;
}

void Ssd1306Display::update() {
    if (!initialized_) {
        Serial.print(F("SSD1306:"));
        Serial.println(F(" Update called but display not initialized"));
    }

    display_.display();
    isDirty_ = false;
}

void Ssd1306Display::setTextSize(uint8_t size) {
    if (!initialized_) return;
    display_.setTextSize(size);
}

void Ssd1306Display::setTextColor(uint16_t color) {
    if (!initialized_) return;
    display_.setTextColor(color);
}

void Ssd1306Display::setTextWrap(bool wrap) {
    if (!initialized_) return;
    display_.setTextWrap(wrap);
}

void Ssd1306Display::setCursor(int16_t x, int16_t y) {
    if (!initialized_) return;
    display_.setCursor(x, y);
}

void Ssd1306Display::getTextBounds(const char* text, uint16_t* w, uint16_t* h) {
    if (!initialized_) {
        *w = 0;
        *h = 0;
        return;
    }

    int16_t x1, y1;
    display_.getTextBounds(text, 0, 0, &x1, &y1, w, h);
}

void Ssd1306Display::drawCenteredText(int x, int y, const char* text) {
    if (!initialized_) return;

    int16_t x1, y1;
    uint16_t w, h;
    display_.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    int posX = x - (w / 2);

    display_.setCursor(posX, y);
    display_.print(text);

    isDirty_ = true;
}

void Ssd1306Display::drawFormattedText(int x, int y, const char* format, ...) {
    if (!initialized_) return;

    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    drawText(x, y, buffer);
}

unsigned long Ssd1306Display::getAverageUpdateTime() const {
    return profiler_.getAverageUpdateTime();
}

unsigned long Ssd1306Display::getMaxUpdateTime() const {
    return profiler_.getMaxUpdateTime();
}

unsigned long Ssd1306Display::getMinUpdateTime() const {
    return profiler_.getMinUpdateTime();
}

void Ssd1306Display::resetPerformanceCounters() {
    profiler_.reset();
}

// APPROCHE SIMPLE ET ROBUSTE POUR LES ARCS
void Ssd1306Display::drawArc(int x, int y, int radius, int startAngle, int endAngle, uint16_t color,
                             uint8_t thickness) {
    if (!initialized_) return;

    // Normaliser les angles pour être entre 0 et 359
    while (startAngle < 0) startAngle += 360;
    while (endAngle < 0) endAngle += 360;
    while (startAngle >= 360) startAngle -= 360;
    while (endAngle >= 360) endAngle -= 360;

    // Calculer la plage d'angles
    int totalAngle =
        (endAngle >= startAngle) ? (endAngle - startAngle) : (360 - startAngle + endAngle);

    // Dessiner chaque niveau d'épaisseur
    for (int t = 0; t < thickness; t++) {
        int currentRadius = radius - t;
        if (currentRadius <= 0) break;

        // Dessiner pixel par pixel avec un pas de 1 degré pour éviter les trous
        for (int angleStep = 0; angleStep <= totalAngle; angleStep++) {
            int currentAngle = (startAngle + angleStep) % 360;

            // Convertir en radians et calculer la position
            float rad = currentAngle * DEG_TO_RAD;
            int px = x + cos(rad) * currentRadius;
            int py = y + sin(rad) * currentRadius;

            // Dessiner le pixel
            display_.drawPixel(px, py, color);
        }
    }

    isDirty_ = true;
}
