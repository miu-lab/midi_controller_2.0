#pragma once

#include "View.hpp"
#include "adapters/secondary/hardware/display/Ssd1306Display.hpp"
#include <memory>

class PerformanceView : public View {
public:
    // Ajouter un constructeur qui accepte le display
    explicit PerformanceView(std::shared_ptr<DisplayPort> display = nullptr) : View(display) {}
    ~PerformanceView() override = default;
    
    bool init() override { return true; }
    void update() override {}
    bool handleEvent(uint8_t eventType, int32_t data) override { return false; }
    bool isActive() const override { return active_; }
    void setActive(bool active) override { active_ = active; }
    
    void render() override {
        if (!active_ || !display_) return;
        
        // Tenter de convertir en Ssd1306Display
        auto* ssd1306Display = dynamic_cast<Ssd1306Display*>(display_.get());
        if (!ssd1306Display) return;
        
        display_->clear();
        
        display_->drawText(0, 0, "Display Performance:");
        
        char buffer[32];
        
        snprintf(buffer, sizeof(buffer), "Avg: %lu us", ssd1306Display->getAverageUpdateTime());
        display_->drawText(0, 16, buffer);
        
        snprintf(buffer, sizeof(buffer), "Max: %lu us", ssd1306Display->getMaxUpdateTime());
        display_->drawText(0, 24, buffer);
        
        snprintf(buffer, sizeof(buffer), "Min: %lu us", ssd1306Display->getMinUpdateTime());
        display_->drawText(0, 32, buffer);
        
        const unsigned long avgTime = ssd1306Display->getAverageUpdateTime();
        if (avgTime > 0) {
            float fps = 1000000.0f / avgTime;
            snprintf(buffer, sizeof(buffer), "FPS: %.1f", fps);
            display_->drawText(0, 48, buffer);
        }
        
        // Dessiner un graphique simple
        drawPerformanceBar(64, 42, 60, 10, ssd1306Display->getAverageUpdateTime(), 20000);
        
        // Ne pas appeler display_->update() directement 
        // mais laisser le gestionnaire d'affichage s'en charger
    }
    
private:
    void drawPerformanceBar(int x, int y, int width, int height, unsigned long value, unsigned long maxValue) {
        // Dessiner le cadre
        display_->drawRect(x, y, width, height, false);
        
        // Calculer la largeur de la barre (clampée pour ne pas dépasser)
        int barWidth = static_cast<int>((static_cast<float>(value) / maxValue) * (width - 2));
        barWidth = min(barWidth, width - 2);
        
        // Dessiner la barre
        if (barWidth > 0) {
            display_->drawRect(x + 1, y + 1, barWidth, height - 2, true);
        }
    }
};
