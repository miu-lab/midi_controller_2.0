#pragma once

#include "core/domain/IDisplay.hpp"
#include <string>

/**
 * @brief Implémentation de IDisplay pour les tests
 */
class MockDisplay : public IDisplay {
public:
    void clear() override {
        wasCleared_ = true;
    }
    
    void drawText(int x, int y, const char* text) override {
        lastX_ = x;
        lastY_ = y;
        lastMessage_ = text;
    }
    
    void drawLine(int x0, int y0, int x1, int y1) override {
        // Implémentation vide pour tests
    }
    
    void drawRect(int x, int y, int width, int height, bool fill) override {
        // Implémentation vide pour tests
    }
    
    void drawCircle(int x, int y, int radius, bool fill) override {
        // Implémentation vide pour tests
    }
    
    void update() override {
        updateCalled_ = true;
    }
    
    // Accesseurs pour tests
    std::string getLastMessage() const { return lastMessage_; }
    bool wasCleared() const { return wasCleared_; }
    bool wasUpdateCalled() const { return updateCalled_; }
    int getLastX() const { return lastX_; }
    int getLastY() const { return lastY_; }
    
private:
    std::string lastMessage_ = "";
    bool wasCleared_ = false;
    bool updateCalled_ = false;
    int lastX_ = 0;
    int lastY_ = 0;
};
