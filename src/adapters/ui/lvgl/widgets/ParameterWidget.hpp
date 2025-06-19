#pragma once

#include <lvgl.h>
#include <Arduino.h>
#include <functional>

/**
 * @brief Widget LVGL pour affichage de paramètres MIDI
 * 
 * Ce widget remplace ParameterFocusView avec une interface LVGL native.
 * Features:
 * - Arc circulaire pour valeur 0-127
 * - Labels pour CC, canal, nom paramètre et valeur
 * - Animations fluides
 * - Support rotation écran
 * - Callbacks pour intégration
 */
class ParameterWidget {
public:
    /**
     * @brief Configuration du widget
     */
    struct Config {
        // Dimensions et position
        uint16_t width = 240;           ///< Largeur du widget
        uint16_t height = 120;          ///< Hauteur du widget
        
        // Arc configuration
        uint16_t arc_size = 80;         ///< Taille de l'arc (diamètre)
        uint16_t arc_width = 6;         ///< Épaisseur de l'arc
        lv_color_t arc_color = lv_color_hex(0x0080FF);  ///< Couleur arc actif
        lv_color_t arc_bg_color = lv_color_hex(0x404040); ///< Couleur arc background
        
        // Animation
        uint32_t anim_duration = 200;   ///< Durée animation (ms)
        bool enable_animations = true; ///< Activer animations
        
        // Style
        bool show_cc_number = true;     ///< Afficher numéro CC
        bool show_channel = true;       ///< Afficher canal MIDI
        bool show_value = true;         ///< Afficher valeur numérique
    };

    /**
     * @brief Callback appelé quand la valeur change (via interaction utilisateur)
     * @param new_value Nouvelle valeur (0-127)
     */
    using ValueChangedCallback = std::function<void(uint8_t new_value)>;

    /**
     * @brief Constructeur
     * @param parent Objet parent LVGL
     * @param config Configuration du widget (optionnel)
     */
    explicit ParameterWidget(lv_obj_t* parent, const Config& config = getDefaultConfig());

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
     * @brief Définit le callback de changement de valeur
     * @param callback Fonction callback
     */
    void setValueChangedCallback(ValueChangedCallback callback) {
        value_changed_callback_ = callback;
    }

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
     * @brief Obtient l'objet LVGL racine du widget
     * @return Pointeur vers l'objet container
     */
    lv_obj_t* getContainer() const { return container_; }

    /**
     * @brief Configuration par défaut
     */
    static Config getDefaultConfig();

private:
    // Configuration
    Config config_;
    
    // État du paramètre
    uint8_t current_value_;
    uint8_t cc_number_;
    uint8_t channel_;
    String parameter_name_;
    
    // Objets LVGL
    lv_obj_t* container_;       ///< Container principal
    lv_obj_t* arc_;             ///< Arc pour valeur
    lv_obj_t* cc_label_;        ///< Label "CC XX"
    lv_obj_t* channel_label_;   ///< Label "CH XX"
    lv_obj_t* value_label_;     ///< Label valeur numérique
    lv_obj_t* name_label_;      ///< Label nom paramètre
    
    // Animation
    lv_anim_t value_anim_;
    bool is_animating_;
    
    // Callback
    ValueChangedCallback value_changed_callback_;
    
    // === MÉTHODES PRIVÉES ===
    
    /**
     * @brief Crée la structure LVGL du widget
     */
    void createLvglObjects();
    
    /**
     * @brief Configure les styles du widget
     */
    void setupStyles();
    
    /**
     * @brief Met à jour tous les labels
     */
    void updateLabels();
    
    /**
     * @brief Met à jour la position de l'arc selon la valeur
     * @param animate Utiliser animation
     */
    void updateArcValue(bool animate);
    
    /**
     * @brief Calcule la valeur d'arc LVGL (0-1000) depuis valeur MIDI (0-127)
     * @param midi_value Valeur MIDI
     * @return Valeur arc LVGL
     */
    static int16_t midiToArcValue(uint8_t midi_value);
    
    /**
     * @brief Calcule la valeur MIDI (0-127) depuis valeur arc LVGL (0-1000)
     * @param arc_value Valeur arc LVGL
     * @return Valeur MIDI
     */
    static uint8_t arcToMidiValue(int16_t arc_value);
    
    // === CALLBACKS LVGL STATIQUES ===
    
    /**
     * @brief Callback événement arc (rotation utilisateur)
     * @param e Événement LVGL
     */
    static void arc_event_cb(lv_event_t* e);
    
    /**
     * @brief Callback animation terminée
     * @param a Animation
     */
    static void anim_ready_cb(lv_anim_t* a);
    
    /**
     * @brief Exécuteur d'animation pour valeur arc
     * @param obj Objet arc
     * @param value Nouvelle valeur
     */
    static void anim_arc_exec_cb(void* obj, int32_t value);
    
    /**
     * @brief Obtient l'instance du widget depuis l'objet LVGL
     * @param obj Objet LVGL
     * @return Pointeur vers ParameterWidget ou nullptr
     */
    static ParameterWidget* getInstanceFromObj(lv_obj_t* obj);
};