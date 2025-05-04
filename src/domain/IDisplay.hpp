#pragma once

class IDisplay {
public:
    virtual ~IDisplay() = default;
    virtual void clear() = 0;
    virtual void drawText(int x, int y, const char* text) = 0;
    // TODO: Ajouter d'autres primitives graphiques au besoin
};
