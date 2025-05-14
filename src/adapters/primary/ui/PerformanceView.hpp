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
        
        // Afficher les statistiques
        char temp[32];
        FlashStrings::copy(temp, sizeof(temp), PERF_AVG);
        snprintf(buffer, sizeof(buffer), temp, avgTime);
        display_->drawText(0, 16, buffer);
        
        FlashStrings::copy(temp, sizeof(temp), PERF_MAX);
        snprintf(buffer, sizeof(buffer), temp, maxTime);
        display_->drawText(0, 24, buffer);
        
        FlashStrings::copy(temp, sizeof(temp), PERF_MIN);
        snprintf(buffer, sizeof(buffer), temp, minTime);
        display_->drawText(0, 32, buffer);
        if (avgTime > 0) {
            float fps = 1000000.0f / avgTime;
            FlashStrings::copy(temp, sizeof(temp), PERF_FPS);
            snprintf(buffer, sizeof(buffer), temp, fps);
            display_->drawText(0, 48, buffer);
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
