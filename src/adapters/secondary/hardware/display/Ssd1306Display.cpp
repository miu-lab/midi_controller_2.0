#include "Ssd1306Display.hpp"

#include <Arduino.h>
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
    // Si déjà initialisé, retourner directement
    if (initialized_) {
        return true;
    }

    char buffer[64]; // Buffer temporaire pour les chaînes Flash
    FlashStrings::copy(buffer, sizeof(buffer), INIT_SSD1306);
    Serial.println(buffer);

    // Initialisation de l'écran SSD1306
    if (!display_.begin(SSD1306_SWITCHCAPVCC, i2cAddress_, resetPin)) {
        FlashStrings::copy(buffer, sizeof(buffer), INIT_SSD1306_FAILED);
        Serial.println(buffer);
        return false;
    }

    FlashStrings::copy(buffer, sizeof(buffer), INIT_SSD1306_SUCCESS);
    Serial.println(buffer);

    // Effacer complètement l'écran et le buffer
    display_.clearDisplay();
    display_.display();  // Mettre à jour l'écran physique

    // Effacer à nouveau pour s'assurer que tout est propre
    display_.clearDisplay();
    display_.display();

    // Configuration initiale
    display_.setTextColor(SSD1306_WHITE);
    display_.setTextSize(1);
    display_.setCursor(0, 0);

    // Définir le flag d'initialisation
    initialized_ = true;

    // Définir le dirty flag pour forcer une première mise à jour
    isDirty_ = true;

    // Dessiner un pattern test pour confirmer que l'écran fonctionne
    display_.clearDisplay();
    display_.display();

    FlashStrings::copy(buffer, sizeof(buffer), INIT_SSD1306_READY);
    Serial.println(buffer);
    return true;
}

void Ssd1306Display::clear() {
    if (!initialized_) return;

    // Effacer tout le buffer en RAM
    display_.clearDisplay();

    // S'assurer que le curseur est remis à la position initiale
    display_.setCursor(0, 0);

    // Marquer l'affichage comme modifié
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

void Ssd1306Display::update() {
    if (!initialized_) {
        char buffer[64];
        FlashStrings::copy(buffer, sizeof(buffer), SSD1306_NOT_INITIALIZED);
        Serial.println(buffer);
        return;
    }

    // Envoyer les données du buffer à l'écran physique
    display_.display();

    // Réinitialiser le dirty flag
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