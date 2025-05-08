#pragma once

#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Alias pour maintenir la compatibilité
 * @deprecated Utiliser DisplayPort à la place
 */
class IDisplay : public DisplayPort {
public:
    // Méthodes virtuelles pour maintenir la compatibilité avec l'ancienne interface
    virtual void drawLine(int x0, int y0, int x1, int y1) override {}

    virtual void drawRect(int x, int y, int width, int height, bool fill = false) override {}

    virtual void drawCircle(int x, int y, int radius, bool fill = false) override {}

    virtual void update() override {}
};
