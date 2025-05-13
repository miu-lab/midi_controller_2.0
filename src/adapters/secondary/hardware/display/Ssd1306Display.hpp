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
     * @brief Met à jour l'affichage
     */
    void update() override;

    /**
     * @brief Définit la taille du texte
     * @param size Taille du texte (1 = standard, 2 = double, etc.)
     */
    void setTextSize(uint8_t size);

    /**
     * @brief Définit la couleur du texte (pour les affichages monochromes, 1 = blanc, 0 = noir)
     * @param color Couleur (1 = blanc, 0 = noir)
     */
    void setTextColor(uint16_t color);

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
};
