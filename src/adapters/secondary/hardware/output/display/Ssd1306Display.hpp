#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <memory>

#include "core/ports/output/DisplayPort.hpp"
#include "core/utils/DisplayProfiler.hpp"

/**
 * @brief Implémentation de DisplayPort pour l'écran OLED SSD1306
 * 
 * Cette classe adapte l'interface DisplayPort pour fonctionner avec
 * la bibliothèque Adafruit_SSD1306, permettant un affichage sur un
 * écran OLED SSD1306 via le bus I2C.
 */
class Ssd1306Display : public DisplayPort {
public:
    /**
     * @brief Vérifie si l'affichage est de type SSD1306
     * @return Toujours true pour cette classe
     */
    bool isSsd1306Display() const override { return true; }
    
    /**
     * @brief Obtient le type d'affichage
     * @return "SSD1306"
     */
    const char* getDisplayType() const override { return "SSD1306"; }
    
    /**
     * @brief Obtient les statistiques de performance de l'affichage
     * @param avgTime Temps moyen en microsecondes (sortie)
     * @param maxTime Temps maximum en microsecondes (sortie)
     * @param minTime Temps minimum en microsecondes (sortie)
     */
    void getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime, unsigned long& minTime) const override {
        avgTime = getAverageUpdateTime();
        maxTime = getMaxUpdateTime();
        minTime = getMinUpdateTime();
    }
    
    /**
     * @brief Dessine un arc de cercle 
     * @param x Position horizontale du centre
     * @param y Position verticale du centre
     * @param radius Rayon
     * @param startAngle Angle de départ en degrés (0-360)
     * @param endAngle Angle d'arrivée en degrés (0-360)
     * @param color Couleur (1 = blanc, 0 = noir)
     * @param thickness Épaisseur de l'arc en pixels
     */
    void drawArc(int x, int y, int radius, int startAngle, int endAngle, uint16_t color, uint8_t thickness = 1);
    
    /**
     * @brief Efface une zone circulaire (pour éviter le scintillement)
     * @param x Position horizontale du centre
     * @param y Position verticale du centre
     * @param innerRadius Rayon intérieur de la zone à effacer
     * @param outerRadius Rayon extérieur de la zone à effacer
     */
    void clearArcArea(int x, int y, int innerRadius, int outerRadius);
    
    /**
     * @brief Constructeur
     * @param width Largeur en pixels de l'écran (typiquement 128)
     * @param height Hauteur en pixels de l'écran (typiquement 64 ou 32)
     * @param i2cAddress Adresse I2C de l'écran (typiquement 0x3C ou 0x3D)
     * @param wireInstance Instance de Wire à utiliser (par défaut Wire2 pour Teensy 4.1)
     */
    Ssd1306Display(uint16_t width = 128, uint16_t height = 64, uint8_t i2cAddress = 0x3C,
                   TwoWire& wireInstance = Wire2);

    /**
     * @brief Destructeur
     */
    ~Ssd1306Display() override = default;

    /**
     * @brief Initialise l'écran
     * @param resetPin Broche de reset (-1 si non utilisée)
     * @return true si l'initialisation a réussi, false sinon
     */
    bool init(int8_t resetPin = -1);

    /**
     * @brief Efface l'écran
     */
    void clear() override;

    /**
     * @brief Affiche du texte à une position donnée
     * @param x Position horizontale
     * @param y Position verticale
     * @param text Texte à afficher
     */
    void drawText(int x, int y, const char* text) override;

    /**
     * @brief Dessine une ligne
     * @param x0 Position horizontale de départ
     * @param y0 Position verticale de départ
     * @param x1 Position horizontale d'arrivée
     * @param y1 Position verticale d'arrivée
     */
    void drawLine(int x0, int y0, int x1, int y1) override;

    /**
     * @brief Dessine un rectangle
     * @param x Position horizontale
     * @param y Position verticale
     * @param width Largeur
     * @param height Hauteur
     * @param fill Si true, le rectangle est rempli
     */
    void drawRect(int x, int y, int width, int height, bool fill = false) override;

    /**
     * @brief Dessine un cercle
     * @param x Position horizontale du centre
     * @param y Position verticale du centre
     * @param radius Rayon
     * @param fill Si true, le cercle est rempli
     */
    void drawCircle(int x, int y, int radius, bool fill = false) override;
    
    /**
     * @brief Dessine un cercle avec une couleur spécifique
     * @param x Position horizontale du centre
     * @param y Position verticale du centre
     * @param radius Rayon
     * @param fill Si true, le cercle est rempli
     * @param color Couleur (1 = blanc, 0 = noir)
     */
    void drawCircle(int x, int y, int radius, bool fill, uint16_t color);

    /**
     * @brief Met à jour l'affichage
     */
    void update() override;

    /**
     * @brief Définit la taille du texte
     * @param size Taille du texte (1 = standard, 2 = double, etc.)
     */
    void setTextSize(uint8_t size) override;

    /**
     * @brief Définit la couleur du texte (pour les affichages monochromes, 1 = blanc, 0 = noir)
     * @param color Couleur (1 = blanc, 0 = noir)
     */
    void setTextColor(uint16_t color) override;
    
    /**
     * @brief Définit si le texte doit automatiquement passer à la ligne
     * @param wrap true pour activer le retour à la ligne
     */
    void setTextWrap(bool wrap) override;
    
    /**
     * @brief Positionne le curseur pour le texte
     * @param x Position horizontale
     * @param y Position verticale
     */
    void setCursor(int16_t x, int16_t y) override;
    
    /**
     * @brief Calcule les dimensions d'un texte
     * @param text Texte à mesurer
     * @param w Largeur (sortie)
     * @param h Hauteur (sortie)
     */
    void getTextBounds(const char* text, uint16_t* w, uint16_t* h) override;
    
    /**
     * @brief Affiche du texte centré horizontalement
     * @param x Position horizontale du centre
     * @param y Position verticale
     * @param text Texte à afficher
     */
    void drawCenteredText(int x, int y, const char* text) override;
    
    /**
     * @brief Affiche un texte formaté (similaire à printf)
     * @param x Position horizontale
     * @param y Position verticale
     * @param format Format (comme printf)
     * @param ... Arguments variables pour le format
     */
    void drawFormattedText(int x, int y, const char* format, ...) override;

    /**
     * @brief Obtient le temps moyen de mise à jour de l'écran en microsecondes
     * @return Temps moyen en microsecondes
     */
    unsigned long getAverageUpdateTime() const;
    
    /**
     * @brief Obtient le temps maximum de mise à jour de l'écran en microsecondes
     * @return Temps maximum en microsecondes
     */
    unsigned long getMaxUpdateTime() const;
    
    /**
     * @brief Obtient le temps minimum de mise à jour de l'écran en microsecondes
     * @return Temps minimum en microsecondes
     */
    unsigned long getMinUpdateTime() const;
    
    /**
     * @brief Réinitialise les compteurs de performance
     */
    void resetPerformanceCounters();

private:
    Adafruit_SSD1306 display_;
    uint16_t width_;
    uint16_t height_;
    uint8_t i2cAddress_;
    TwoWire& wireInstance_;
    bool initialized_;
    DisplayProfiler profiler_;
    bool isDirty_ = false;  // Flag indiquant si l'affichage a été modifié
    
    // Méthodes d'aide pour l'algorithme d'arc déterministe
    void drawBresenhamArc(int cx, int cy, int radius, int startAngle, int endAngle, uint16_t color);
    void drawArcPixelIfInRange(int cx, int cy, int dx, int dy, int startAngle, int endAngle, uint16_t color);
    int fastAtan2Degrees(int y, int x);
    int betterAtan2Degrees(int y, int x);
    int normalizeAngle(int angle);
    bool isAngleInRange(int angle, int startAngle, int endAngle);
    bool isAngleInRangeWithTolerance(int angle, int startAngle, int endAngle);
};
