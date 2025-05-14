#pragma once

/**
 * @brief Port de sortie pour l'affichage
 *
 * Cette interface définit le contrat pour l'interaction avec l'affichage.
 */
class DisplayPort {
public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~DisplayPort() = default;
    
    /**
     * @brief Vérifie si l'affichage est de type SSD1306
     * @return true si c'est un SSD1306, false sinon
     */
    virtual bool isSsd1306Display() const { return false; }
    
    /**
     * @brief Obtient le type d'affichage
     * @return Chaîne de caractères représentant le type
     */
    virtual const char* getDisplayType() const { return "BaseDisplay"; }
    
    /**
     * @brief Obtient les statistiques de performance de l'affichage
     * @param avgTime Temps moyen en microsecondes (sortie)
     * @param maxTime Temps maximum en microsecondes (sortie)
     * @param minTime Temps minimum en microsecondes (sortie)
     */
    virtual void getPerformanceStats(unsigned long& avgTime, unsigned long& maxTime, unsigned long& minTime) const {
        avgTime = 0;
        maxTime = 0;
        minTime = 0;
    }

    /**
     * @brief Efface l'écran
     */
    virtual void clear() = 0;

    /**
     * @brief Affiche du texte à une position donnée
     * @param x Position horizontale
     * @param y Position verticale
     * @param text Texte à afficher
     */
    virtual void drawText(int x, int y, const char* text) = 0;

    /**
     * @brief Dessine une ligne
     * @param x0 Position horizontale de départ
     * @param y0 Position verticale de départ
     * @param x1 Position horizontale d'arrivée
     * @param y1 Position verticale d'arrivée
     */
    virtual void drawLine(int x0, int y0, int x1, int y1) = 0;

    /**
     * @brief Dessine un rectangle
     * @param x Position horizontale
     * @param y Position verticale
     * @param width Largeur
     * @param height Hauteur
     * @param fill Si true, le rectangle est rempli
     */
    virtual void drawRect(int x, int y, int width, int height, bool fill = false) = 0;

    /**
     * @brief Dessine un cercle
     * @param x Position horizontale du centre
     * @param y Position verticale du centre
     * @param radius Rayon
     * @param fill Si true, le cercle est rempli
     */
    virtual void drawCircle(int x, int y, int radius, bool fill = false) = 0;

    /**
    * @brief Met à jour l'affichage
    *
    * Certains affichages nécessitent un appel explicite pour mettre à jour
    * le contenu après des modifications.
    */
virtual void update() = 0;

/**
 * @brief Définit la couleur du texte (pour les affichages monochromes, 1 = blanc, 0 = noir)
 * @param color Couleur (1 = blanc, 0 = noir)
 */
virtual void setTextColor(uint16_t color) = 0;
};
