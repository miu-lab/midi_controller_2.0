#pragma once

#include <Adafruit_GFX.h>
#include <ILI9341_T4.h>
#include <tgx.h>

// Fontes TGX pour le rendu de texte
#include <font_tgx_Arial.h>
#include <font_tgx_OpenSans.h>

#include <memory>

#include "core/ports/output/DisplayPort.hpp"
#include "core/utils/DisplayProfiler.hpp"

/**
 * @brief Implémentation de DisplayPort pour l'écran TFT ILI9341 avec TGX
 *
 * Cette classe combine ILI9341_T4 (driver SPI optimisé) avec TGX (librairie graphique)
 * pour offrir des performances exceptionnelles sur Teensy 4.x.
 *
 * Architecture:
 * DisplayPort → Ili9341TgxDisplay → TGX (dessin) → ILI9341_T4 (SPI) → Hardware
 */
class Ili9341TgxDisplay : public DisplayPort {
public:
    //@brief Énumération pour les tailles de fonte standard
    enum class FontSize {
        SMALL,   // 8-10px pour infos secondaires
        NORMAL,  // 12-14px pour texte standard
        LARGE,   // 16-20px pour valeurs importantes
        TITLE    // 24px+ pour titres
    };

    /**
     * @brief Configuration matérielle pour ILI9341
     */
    struct Config {
        uint8_t cs_pin;      ///< Chip Select (n'importe quel pin)
        uint8_t dc_pin;      ///< Data/Command (recommandé: pin Hardware CS pour +35% perf)
        uint8_t rst_pin;     ///< Reset (255 = non connecté)
        uint8_t mosi_pin;    ///< SPI MOSI
        uint8_t sck_pin;     ///< SPI Clock
        uint8_t miso_pin;    ///< SPI MISO
        uint32_t spi_speed;  ///< Vitesse SPI en Hz
        uint8_t rotation;    ///< Rotation écran (0-3)
    };

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration des pins et paramètres
     */
    explicit Ili9341TgxDisplay(const Config& config = getDefaultConfig());

    /**
     * @brief Destructeur
     */
    ~Ili9341TgxDisplay() override;

    // === INTERFACE DisplayPort ===

    /**
     * @brief Initialise l'écran et le framebuffer
     * @return true si l'initialisation a réussi
     */
    bool init();

    void clear() override;
    void drawText(int x, int y, const char* text) override;
    void drawLine(int x0, int y0, int x1, int y1) override;
    void drawRect(int x, int y, int width, int height, bool fill = false) override;
    void drawCircle(int x, int y, int radius, bool fill = false) override;
    void update() override;
    void setTextSize(uint8_t size) override;
    void setTextColor(uint16_t color) override;
    void setTextWrap(bool wrap) override;
    void setCursor(int16_t x, int16_t y) override;
    void getTextBounds(const char* text, uint16_t* w, uint16_t* h) override;
    void drawCenteredText(int x, int y, const char* text) override;
    void drawFormattedText(int x, int y, const char* format, ...) override;

    // === GESTION DES FONTES ===
    
    /**
     * @brief Définit la fonte à utiliser
     * @param size Taille de fonte standard
     */
    void setFont(FontSize size);
    
    /**
     * @brief Obtient la fonte TGX courante
     * @return Pointeur vers la fonte TGX
     */
    const ILI9341_t3_font_t* getCurrentFont() const;

    // === IDENTIFICATION ===

    const char* getDisplayType() const override {
        return "ILI9341_TGX";
    }

    bool isSsd1306Display() const override {
        return false;
    }

    // === PERFORMANCE ===

    void getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime,
                             unsigned long& minTime) const override;

    /**
     * @brief Configuration par défaut optimisée pour Teensy 4.1
     */
    static Config getDefaultConfig();

    // === MÉTHODES ÉTENDUES ===

    /**
     * @brief Dessine un cercle avec couleur RGB565
     */
    void drawCircle(int x, int y, int radius, bool fill, uint16_t color);

    /**
     * @brief Définit la rotation de l'écran
     * @param rotation 0-3 pour les différentes orientations
     */
    void setRotation(uint8_t rotation);

    /**
     * @brief Obtient les dimensions actuelles
     */
    void getDimensions(uint16_t& width, uint16_t& height) const;

    /**
     * @brief Dessine une barre de progression (utile pour MIDI)
     * @param x Position X
     * @param y Position Y
     * @param width Largeur totale
     * @param height Hauteur
     * @param value Valeur (0-127 pour MIDI)
     * @param maxValue Valeur maximum (défaut 127)
     */
    void drawProgressBar(int x, int y, int width, int height, int value, int maxValue = 127);

    /**
     * @brief Dessine du texte avec une taille spécifique temporairement
     */
    void drawTextWithSize(int x, int y, const char* text, uint8_t size);

private:
    // Hardware et configuration
    Config config_;
    bool initialized_;

    // ILI9341_T4 driver pour SPI optimisé
    std::unique_ptr<ILI9341_T4::ILI9341Driver> tft_;

    // Buffers selon modèle ILI9341_T4
    uint16_t* internalBuffer_;  // Buffer interne DMAMEM

    // Diff buffers pour performance
    std::unique_ptr<ILI9341_T4::DiffBuffStatic<4096>> diff1_;
    std::unique_ptr<ILI9341_T4::DiffBuffStatic<4096>> diff2_;

    // TGX canvas pour le dessin
    std::unique_ptr<tgx::Image<tgx::RGB565>> canvas_;

    // Dimensions écran ILI9341
    static constexpr int SCREEN_WIDTH = 240;
    static constexpr int SCREEN_HEIGHT = 320;

    // État du texte
    tgx::RGB565 currentTextColor_;
    int currentTextSize_;
    int currentCursorX_, currentCursorY_;
    bool textWrap_;
    
    // Gestion des fontes TGX
    const ILI9341_t3_font_t* currentFont_;
    FontSize currentFontSize_;

    // Performance tracking
    DisplayProfiler profiler_;

    // === COULEURS TGX (RGB565) ===
    // Utilisation directe des couleurs TGX pour éviter les problèmes constexpr

    // === MÉTHODES PRIVÉES ===

    /**
     * @brief Alloue le framebuffer en DMAMEM
     */
    bool allocateFramebuffer();

    /**
     * @brief Libère le framebuffer
     */
    void freeFramebuffer();

    /**
     * @brief Convertit couleur mono (0/1) vers RGB565
     */
    tgx::RGB565 convertMonoColor(uint16_t monoColor) const;

    /**
     * @brief Convertit couleur RGB565 vers format TGX
     */
    tgx::RGB565 convertToTgxColor(uint16_t color) const;

    /**
     * @brief Met à jour les dimensions selon la rotation
     */
    void updateDimensions();

    /**
     * @brief Obtient la fonte TGX selon la taille FontSize
     */
    const ILI9341_t3_font_t* getFontForSize(FontSize size) const;
};
