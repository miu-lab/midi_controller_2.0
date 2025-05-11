#pragma once

#include <Arduino.h>

#include "core/ports/output/DisplayPort.hpp"

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
 * @brief Implémentation concrète de DisplayPort pour les tests
 *
 * Cette classe a été renommée pour éviter la confusion avec l'interface DisplayPort
 */
class MockDisplay : public DisplayPort {
public:
    MockDisplay() = default;
    ~MockDisplay() override = default;

    // Implémentation des méthodes pures de DisplayPort
    void clear() override {}

    void drawText(int x, int y, const char* text) override {}

    void update() override {}

    // Ces méthodes ont déjà une implémentation par défaut dans DisplayPort,
    // nous pouvons les remplacer si nécessaire
    void drawLine(int x0, int y0, int x1, int y1) override {}

    void drawRect(int x, int y, int width, int height, bool fill = false) override {}

    void drawCircle(int x, int y, int radius, bool fill = false) override {}
};
