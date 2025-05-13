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

    Serial.println(F("SSD1306: Initializing display..."));

    // Initialisation de l'écran SSD1306
    if (!display_.begin(SSD1306_SWITCHCAPVCC, i2cAddress_, resetPin)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }

    Serial.println(F("SSD1306: Display initialized successfully"));

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

    // Définir le flag d'initialisation
    initialized_ = true;
    
    // Définir le dirty flag pour forcer une première mise à jour
    isDirty_ = true;
    
    // Dessiner un pattern test pour confirmer que l'écran fonctionne
    display_.clearDisplay();
    for (int i = 0; i < 32; i += 4) {
        display_.drawRect(i, i, 128-i*2, 64-i*2, SSD1306_WHITE);
    }
    display_.display();
    delay(500);
    
    // Effacer à nouveau après le test
    display_.clearDisplay();
    display_.display();
    
    Serial.println(F("SSD1306: Display ready"));
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
        Serial.println(F("SSD1306: Update called but display not initialized"));
        return;
    }

    // Vérifier le dirty flag, mais permettre des mises à jour forcées pour des éléments critiques
    if (!isDirty_) {
        // Montrer un petit indicateur d'activité dans le coin inférieur droit
        // même si rien d'autre n'a changé - c'est utile pour débugger l'activité du scheduler
        static uint8_t activityIndicator = 0;
        activityIndicator = (activityIndicator + 1) % 4;
        
        // Dessiner un petit indicateur d'activité dans le coin inférieur droit
        // sans utiliser clear() pour ne pas effacer l'affichage actuel
        switch (activityIndicator) {
            case 0:
                display_.fillRect(124, 60, 4, 4, SSD1306_BLACK);
                display_.drawPixel(125, 61, SSD1306_WHITE);
                break;
            case 1:
                display_.fillRect(124, 60, 4, 4, SSD1306_BLACK);
                display_.drawPixel(126, 61, SSD1306_WHITE);
                break;
            case 2:
                display_.fillRect(124, 60, 4, 4, SSD1306_BLACK);
                display_.drawPixel(126, 62, SSD1306_WHITE);
                break;
            case 3:
                display_.fillRect(124, 60, 4, 4, SSD1306_BLACK);
                display_.drawPixel(125, 62, SSD1306_WHITE);
                break;
        }
        
        // Forcer l'affichage même si rien d'autre n'a changé
        profiler_.startMeasurement();
        display_.display();
        // Appeler endMeasurement mais ne pas stocker le résultat puisqu'on ne l'utilise pas
        profiler_.endMeasurement();
        return;
    }

    // Mesurer le temps d'update
    profiler_.startMeasurement();

    // Envoyer les données du buffer à l'écran physique
    display_.display();
    
    // Réinitialiser le dirty flag
    isDirty_ = false;

    unsigned long duration = profiler_.endMeasurement();

    // Afficher le temps d'update uniquement si nécessaire
    if (duration > 30000) { // Seuil arbitraire de 30ms
        Serial.printf("Display update: %lu µs (slow)\n", duration);
    }
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