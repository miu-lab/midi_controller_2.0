#pragma once

#include "core/ports/output/DisplayPort.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include <memory>

/**
 * @brief Adaptateur pour connecter la nouvelle architecture LVGL à l'interface DisplayPort
 * 
 * Cette classe adapte l'interface DisplayPort existante pour utiliser la nouvelle
 * architecture modulaire Ili9341Driver + Ili9341LvglBridge.
 */
class LvglDisplayPortAdapter : public DisplayPort {
public:
    /**
     * @brief Constructeur
     * @param bridge Le bridge LVGL vers le hardware
     */
    explicit LvglDisplayPortAdapter(std::shared_ptr<Ili9341LvglBridge> bridge);
    
    // Implémentation de l'interface DisplayPort
    virtual ~LvglDisplayPortAdapter() = default;
    
    virtual const char* getDisplayType() const override;
    virtual void getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime, unsigned long& minTime) const override;
    
    virtual void clear() override;
    virtual void drawText(int x, int y, const char* text) override;
    virtual void drawLine(int x0, int y0, int x1, int y1) override;
    virtual void drawRect(int x, int y, int width, int height, bool fill = false) override;
    virtual void drawCircle(int x, int y, int radius, bool fill = false) override;
    virtual void update() override;
    
    virtual void setTextSize(uint8_t size) override;
    virtual void setTextColor(uint16_t color) override;
    virtual void setTextWrap(bool wrap) override;
    virtual void setCursor(int16_t x, int16_t y) override;
    virtual void getTextBounds(const char* text, uint16_t* w, uint16_t* h) override;
    virtual void drawCenteredText(int x, int y, const char* text) override;
    virtual void drawFormattedText(int x, int y, const char* format, ...) override;

private:
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    
    // État interne pour compatibilité avec l'ancienne API
    uint16_t currentTextColor_;
    uint8_t currentTextSize_;
    bool textWrap_;
    int16_t cursorX_, cursorY_;
    
    // Styles LVGL réutilisables pour éviter les fuites mémoire
    lv_style_t textStyle_;
    lv_style_t lineStyle_;
    lv_style_t rectStyle_;
    
    // Compteur d'objets pour éviter les fuites mémoire
    uint16_t objectCount_;
};