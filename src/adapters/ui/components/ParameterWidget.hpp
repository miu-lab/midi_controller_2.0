#pragma once

#include <lvgl.h>
#include <Arduino.h>
#include <functional>
#include <memory>

// Forward declarations pour éviter les inclusions circulaires
class UITheme;
class ButtonIndicator;

/**
 * @brief Widget LVGL pour affichage de paramètres MIDI basé sur lv_arc natif
 * 
 * Cette nouvelle version utilise le widget lv_arc natif de LVGL comme base
 * et ajoute des labels pour créer un widget de paramètre MIDI complet.
 * 
 * Features:
 * - Arc circulaire natif LVGL pour valeur 0-127
 * - Labels pour CC, canal, nom paramètre et valeur
 * - Configuration simplifiée via le widget arc natif
 * - Callbacks pour intégration avec le système MIDI
 */
class ParameterWidget {
public:

    /**
     * @brief Constructeur avec UITheme (recommandé)
     * @param parent Widget parent LVGL
     * @param theme Thème UI pour le styling
     * @param width Largeur du widget
     * @param height Hauteur du widget
     * @param arc_size Taille de l'arc
     */
    ParameterWidget(lv_obj_t* parent, const UITheme& theme, 
                   uint16_t width = 80, uint16_t height = 120, uint16_t arc_size = 64);

    /**
     * @brief Constructeur legacy sans thème (styles hardcodés)
     * @param parent Widget parent LVGL
     * @param width Largeur du widget
     * @param height Hauteur du widget
     * @param arc_size Taille de l'arc
     */
    ParameterWidget(lv_obj_t* parent, 
                   uint16_t width = 80, uint16_t height = 120, uint16_t arc_size = 64);

    /**
     * @brief Destructeur
     */
    ~ParameterWidget();

    // === INTERFACE PUBLIQUE ===

    /**
     * @brief Met à jour les informations du paramètre
     * @param cc_number Numéro CC (0-127)
     * @param channel Canal MIDI (1-16)
     * @param value Valeur (0-127)
     * @param parameter_name Nom du paramètre
     * @param animate Utiliser animation pour le changement
     */
    void setParameter(uint8_t cc_number, uint8_t channel, uint8_t value, 
                     const String& parameter_name, bool animate = true);

    /**
     * @brief Met à jour uniquement la valeur
     * @param value Nouvelle valeur (0-127)
     * @param animate Utiliser animation
     */
    void setValue(uint8_t value, bool animate = true);

    /**
     * @brief Met à jour le nom du paramètre
     * @param parameter_name Nouveau nom
     */
    void setParameterName(const String& parameter_name);

    /**
     * @brief Obtient la valeur actuelle
     * @return Valeur (0-127)
     */
    uint8_t getValue() const { return current_value_; }


    /**
     * @brief Active/désactive le widget
     * @param visible État de visibilité
     */
    void setVisible(bool visible);

    /**
     * @brief Vérifie si le widget est visible
     * @return true si visible
     */
    bool isVisible() const;

    /**
     * @brief Positionne le widget
     * @param x Position X
     * @param y Position Y
     */
    void setPosition(lv_coord_t x, lv_coord_t y);

    /**
     * @brief Définit la taille du widget
     * @param width Largeur
     * @param height Hauteur
     */
    void setSize(lv_coord_t width, lv_coord_t height);

    /**
     * @brief Centre le widget sur son parent
     */
    void center();
    

    /**
     * @brief Active/désactive l'interaction utilisateur
     * @param enable true pour permettre l'interaction
     */
    void setInteractionEnabled(bool enable);

    /**
     * @brief Obtient l'objet LVGL racine du widget
     * @return Pointeur vers l'objet container
     */
    lv_obj_t* getContainer() const { return container_; }

    /**
     * @brief Obtient l'objet arc LVGL
     * @return Pointeur vers l'arc
     */
    lv_obj_t* getArc() const { return arc_; }
    
    /**
     * @brief Traite les mises à jour différées (appelé périodiquement)
     */
    void processPendingUpdates();

    // === GESTION BUTTON INDICATOR ===

    /**
     * @brief Ajoute un indicateur de bouton au widget
     * @param size Taille de l'indicateur LED (par défaut 12px)
     * @return true si l'indicateur a été créé avec succès
     */
    bool addButtonIndicator(lv_coord_t size = 12);

    /**
     * @brief Supprime l'indicateur de bouton
     */
    void removeButtonIndicator();

    /**
     * @brief Vérifie si le widget a un indicateur de bouton
     * @return true si l'indicateur existe
     */
    bool hasButtonIndicator() const;

    /**
     * @brief Définit l'état de l'indicateur de bouton
     * @param pressed true si le bouton est pressé
     * @param animate Utiliser animation
     */
    void setButtonState(bool pressed, bool animate = true);

    /**
     * @brief Obtient l'indicateur de bouton
     * @return Pointeur vers l'indicateur ou nullptr si inexistant
     */
    ButtonIndicator* getButtonIndicator() const;


private:
    // Configuration interne
    lv_obj_t* parent_;          ///< Objet parent LVGL
    const UITheme* theme_;      ///< Référence au thème UI (optionnel)
    
    // Dimensions du widget
    uint16_t width_;
    uint16_t height_;
    uint16_t arc_size_;
    
    // État du paramètre
    uint8_t current_value_;
    uint8_t cc_number_;
    uint8_t channel_;
    String parameter_name_;
    
    // Objets LVGL
    lv_obj_t* container_;       ///< Container principal
    lv_obj_t* arc_;             ///< Arc natif LVGL
    lv_obj_t* cc_label_;        ///< Label "CC XX" (inutilisé)
    lv_obj_t* channel_label_;   ///< Label "CH XX" (inutilisé)
    lv_obj_t* name_label_;      ///< Label nom paramètre
    
    // Widget optionnel pour indicateur de bouton
    std::unique_ptr<ButtonIndicator> button_indicator_;  ///< Indicateur bouton (optionnel)
    
    // Optimisation performance
    bool pending_value_update_ = false;
    
    
    // === MÉTHODES PRIVÉES ===
    
    /**
     * @brief Crée la structure LVGL du widget
     */
    void createLvglObjects();
    
    /**
     * @brief Met à jour tous les labels
     */
    void updateLabels();
    
    /**
     * @brief Positionne les labels autour de l'arc
     */
    void positionLabels();
    
    /**
     * @brief Configure les styles hardcodés pour le mode legacy
     */
    void setupLegacyStyles();
    
    /**
     * @brief Met à jour la valeur de l'arc
     * @param animate Utiliser animation
     */
    void updateArcValue(bool animate);
    
    /**
     * @brief Positionne l'indicateur de bouton dans le widget
     */
    void positionButtonIndicator();
    
};