#pragma once

#include "core/ports/output/DisplayPort.hpp"
#include "core/domain/IDisplay.hpp"
#include <Arduino.h>

/**
 * @brief Mock pour DisplayPort utilisé dans les tests
 */
class DisplayMock : public DisplayPort {
public:
    DisplayMock() = default;
    ~DisplayMock() override = default;
    
    void clear() override {}
    void drawText(int x, int y, const char* text) override {}
    void drawLine(int x0, int y0, int x1, int y1) override {}
    void drawRect(int x, int y, int width, int height, bool fill = false) override {}
    void drawCircle(int x, int y, int radius, bool fill = false) override {}
    void update() override {}
};

/**
 * @brief Implémentation concrète de IDisplay pour les tests
 * 
 * Cette classe a été renommée pour éviter la confusion avec l'interface IDisplay
 */
class MockDisplay : public IDisplay {
public:
    MockDisplay() = default;
    ~MockDisplay() override = default;
    
    // Implémentation des méthodes pures de DisplayPort
    void clear() override {}
    void drawText(int x, int y, const char* text) override {}
    void update() override {}
    
    // Ces méthodes ont déjà une implémentation par défaut dans IDisplay,
    // nous pouvons les remplacer si nécessaire
    void drawLine(int x0, int y0, int x1, int y1) override {}
    void drawRect(int x, int y, int width, int height, bool fill = false) override {}
    void drawCircle(int x, int y, int radius, bool fill = false) override {}
};
