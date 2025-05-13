#include "Ssd1306Display.hpp"

#include <Arduino.h>

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

    // Initialisation de l'écran SSD1306
    if (!display_.begin(SSD1306_SWITCHCAPVCC, i2cAddress_, resetPin)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }

    // Effacer complètement l'écran et le buffer
    display_.clearDisplay();
    display_.display();  // Mettre à jour l'écran physique

    // Attendre un court instant pour stabiliser l'écran
    delay(50);

    // Effacer à nouveau pour s'assurer que tout est propre
    display_.clearDisplay();
    display_.display();

    // Configuration initiale
    display_.setTextColor(SSD1306_WHITE);
    display_.setTextSize(1);
    display_.setCursor(0, 0);

    initialized_ = true;
    return true;
}

void Ssd1306Display::clear() {
    if (!initialized_) return;

    // Effacer tout le buffer en RAM
    display_.clearDisplay();

    // S'assurer que le curseur est remis à la position initiale
    display_.setCursor(0, 0);

    // En cas de problème persistant, on peut essayer de réinitialiser complètement l'affichage
    // Attention: c'est une solution radicale et lente!
    static bool deepCleanDone = false;
    if (!deepCleanDone) {
        deepCleanDone = true;
        Serial.println(F("Performing deep clean of SSD1306 display..."));

        // Dessiner un motif de test
        for (int i = 0; i < 64; i += 2) {
            for (int j = 0; j < 128; j += 2) {
                display_.drawPixel(j, i, SSD1306_WHITE);
            }
        }
        display_.display();
        delay(100);

        // Effacer complètement
        display_.clearDisplay();
        display_.display();
        delay(100);
    }
}

void Ssd1306Display::drawText(int x, int y, const char* text) {
    if (!initialized_) return;
    display_.setCursor(x, y);
    display_.print(text);
}

void Ssd1306Display::drawLine(int x0, int y0, int x1, int y1) {
    if (!initialized_) return;
    display_.drawLine(x0, y0, x1, y1, SSD1306_WHITE);
}

void Ssd1306Display::drawRect(int x, int y, int width, int height, bool fill) {
    if (!initialized_) return;
    if (fill) {
        display_.fillRect(x, y, width, height, SSD1306_WHITE);
    } else {
        display_.drawRect(x, y, width, height, SSD1306_WHITE);
    }
}

void Ssd1306Display::drawCircle(int x, int y, int radius, bool fill) {
    if (!initialized_) return;
    if (fill) {
        display_.fillCircle(x, y, radius, SSD1306_WHITE);
    } else {
        display_.drawCircle(x, y, radius, SSD1306_WHITE);
    }
}

void Ssd1306Display::update() {
    if (!initialized_) return;

    // Envoyer les données du buffer à l'écran physique
    display_.display();
}

void Ssd1306Display::setTextSize(uint8_t size) {
    if (!initialized_) return;
    display_.setTextSize(size);
}

void Ssd1306Display::setTextColor(uint16_t color) {
    if (!initialized_) return;
    display_.setTextColor(color);
}
