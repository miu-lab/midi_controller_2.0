#pragma once

#include <lvgl.h>
#include <Arduino.h>

// Forward declaration pour éviter les inclusions circulaires
class UITheme;

/**
 * @brief Widget LVGL pour indication d'état de bouton basé sur lv_led natif
 * 
 * Ce widget utilise le widget lv_led natif de LVGL pour afficher l'état
 * d'un bouton associé à un encodeur ou indépendant.
 * 
 * Features:
 * - LED native LVGL avec états visuels distincts
 * - Support thème UITheme optionnel avec fallback hardcodé
 * - États: OFF, PRESSED, ACTIVE, ERROR
 * - Taille configurable et positionnement flexible
 * - Optimisé pour intégration dans ParameterWidget
 */
class ButtonIndicator {
public:
    /**
     * @brief États possibles de l'indicateur
     */
    enum class State {
        OFF,        ///< Bouton non pressé, LED éteinte (gris foncé)
        PRESSED,    ///< Bouton pressé, LED allumée (vert vif)
        ACTIVE,     ///< Bouton maintenu actif (vert moyen)
        ERROR       ///< État d'erreur (rouge)
    };

    /**
     * @brief Constructeur avec UITheme (recommandé)
     * @param parent Widget parent LVGL
     * @param theme Thème UI pour le styling
     * @param size Taille de la LED (par défaut 12px)
     */
    ButtonIndicator(lv_obj_t* parent, const UITheme& theme, lv_coord_t size = 12);

    /**
     * @brief Constructeur legacy sans thème (styles hardcodés)
     * @param parent Widget parent LVGL
     * @param size Taille de la LED (par défaut 12px)
     */
    ButtonIndicator(lv_obj_t* parent, lv_coord_t size = 12);

    /**
     * @brief Destructeur
     */
    ~ButtonIndicator();

    // === INTERFACE PUBLIQUE ===

    /**
     * @brief Définit l'état de l'indicateur
     * @param state Nouvel état
     * @param animate Utiliser animation pour le changement
     */
    void setState(State state, bool animate = true);

    /**
     * @brief Obtient l'état actuel
     * @return État actuel
     */
    State getState() const { return current_state_; }

    /**
     * @brief Active/désactive l'indicateur
     * @param visible État de visibilité
     */
    void setVisible(bool visible);

    /**
     * @brief Vérifie si l'indicateur est visible
     * @return true si visible
     */
    bool isVisible() const;

    /**
     * @brief Positionne l'indicateur
     * @param x Position X
     * @param y Position Y
     */
    void setPosition(lv_coord_t x, lv_coord_t y);

    /**
     * @brief Définit la taille de l'indicateur
     * @param size Nouvelle taille
     */
    void setSize(lv_coord_t size);

    /**
     * @brief Centre l'indicateur sur son parent
     */
    void center();

    /**
     * @brief Aligne l'indicateur par rapport à un autre objet
     * @param align_to Objet de référence pour l'alignement
     * @param align Type d'alignement LVGL
     * @param x_offset Décalage X
     * @param y_offset Décalage Y
     */
    void alignTo(lv_obj_t* align_to, lv_align_t align, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);

    /**
     * @brief Obtient l'objet LVGL de la LED
     * @return Pointeur vers l'objet LED
     */
    lv_obj_t* getLed() const { return led_; }

    /**
     * @brief Obtient l'objet container (pour compatibility avec d'autres widgets)
     * @return Pointeur vers l'objet LED (même que getLed())
     */
    lv_obj_t* getContainer() const { return led_; }

    /**
     * @brief Obtient l'objet LED LVGL
     * @return Pointeur vers l'objet LED
     */
    lv_obj_t* getLedObject() const { return led_; }

    // === MÉTHODES STATIQUES UTILITAIRES ===

    /**
     * @brief Convertit un état en couleur par défaut
     * @param state État à convertir
     * @return Couleur LVGL correspondante
     */
    static lv_color_t getDefaultColorForState(State state);

    /**
     * @brief Convertit un état en opacité par défaut
     * @param state État à convertir
     * @return Opacité LVGL correspondante
     */
    static lv_opa_t getDefaultOpacityForState(State state);

private:
    // Configuration interne
    lv_obj_t* parent_;          ///< Objet parent LVGL
    const UITheme* theme_;      ///< Référence au thème UI (optionnel)
    
    // Dimensions
    lv_coord_t size_;           ///< Taille de la LED
    
    // État
    State current_state_;       ///< État actuel de l'indicateur
    
    // Objets LVGL
    lv_obj_t* led_;             ///< LED native LVGL
    
    // === MÉTHODES PRIVÉES ===
    
    /**
     * @brief Crée la LED LVGL
     */
    void createLvglObjects();
    
    /**
     * @brief Configure les styles hardcodés pour le mode legacy
     */
    void setupLegacyStyles();
    
    /**
     * @brief Met à jour l'apparence visuelle selon l'état
     * @param animate Utiliser animation
     */
    void updateVisualState(bool animate);
    
    /**
     * @brief Applique une couleur et opacité à la LED
     * @param color Couleur à appliquer
     * @param opacity Opacité à appliquer
     * @param animate Utiliser animation
     */
    void applyColorAndOpacity(lv_color_t color, lv_opa_t opacity, bool animate);
};