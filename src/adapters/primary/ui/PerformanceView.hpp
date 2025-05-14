#pragma once

#include "View.hpp"
#include "adapters/secondary/hardware/display/Ssd1306Display.hpp"
#include "core/utils/AppStrings.hpp"
#include "core/utils/FlashStrings.hpp"
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
        
        // Utiliser les méthodes virtuelles au lieu de dynamic_cast
        if (!display_->isSsd1306Display()) return;
        
        display_->clear();
        
        // Utiliser des chaînes en mémoire Flash
        char buffer[32]; // Buffer temporaire pour les chaînes
        
        // Afficher le titre
        FlashStrings::copy(buffer, sizeof(buffer), PERF_TITLE);
        display_->drawText(0, 0, buffer);
        
        // Récupérer les statistiques via la méthode virtuelle
        unsigned long avgTime = 0, maxTime = 0, minTime = 0;
        display_->getPerformanceStats(avgTime, maxTime, minTime);
        
        // Afficher les statistiques avec les nouveaux formats
        char format[32];
        
        // Afficher Avg
        FlashStrings::copy(buffer, sizeof(buffer), FMT_AVG);
        display_->drawText(0, 16, buffer);
        FlashStrings::copy(format, sizeof(format), FMT_MICROSECONDS);
        snprintf(buffer, sizeof(buffer), format, avgTime);
        display_->drawText(32, 16, buffer);
        
        // Afficher Max
        FlashStrings::copy(buffer, sizeof(buffer), FMT_MAX);
        display_->drawText(0, 24, buffer);
        snprintf(buffer, sizeof(buffer), format, maxTime);
        display_->drawText(32, 24, buffer);
        
        // Afficher Min
        FlashStrings::copy(buffer, sizeof(buffer), FMT_MIN);
        display_->drawText(0, 32, buffer);
        snprintf(buffer, sizeof(buffer), format, minTime);
        display_->drawText(32, 32, buffer);
        if (avgTime > 0) {
            float fps = 1000000.0f / avgTime;
            FlashStrings::copy(buffer, sizeof(buffer), PERF_FPS);
            snprintf(format, sizeof(format), buffer, fps);
            display_->drawText(0, 48, format);
        }
        
        // Dessiner un graphique simple
        drawPerformanceBar(64, 42, 60, 10, avgTime, 20000);
        
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
