#pragma once

#include "MidiConfigurationParser.hpp"
#include "WidgetMappingManager.hpp"
#include "ParameterEventHandler.hpp"
#include "LvglSceneManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/UIEvent.hpp"
#include <memory>

/**
 * @brief Contrôleur principal pour orchestrer tous les composants de la vue des paramètres
 * 
 * Ce contrôleur centralise la coordination entre:
 * - MidiConfigurationParser : parsing de la configuration
 * - WidgetMappingManager : gestion des mappings CC→Widget et Button→Widget
 * - ParameterEventHandler : gestion des événements MIDI et boutons
 * - LvglSceneManager : gestion de la scène et des objets LVGL
 * 
 * Il implémente le pattern Controller pour respecter la séparation des responsabilités
 * et permettre à LvglParameterView de devenir un simple wrapper.
 */
class ParameterViewController : public EventListener {
public:
    /**
     * @brief Configuration du contrôleur
     */
    struct ControllerConfig {
        uint8_t maxWidgets = 8;           // Nombre maximum de widgets
        uint16_t screenWidth = 320;       // Largeur écran ILI9341
        uint16_t screenHeight = 240;      // Hauteur écran ILI9341
        uint8_t gridCols = 4;             // Colonnes de la grille
        uint8_t gridRows = 2;             // Lignes de la grille
        uint16_t widgetWidth = 80;        // Largeur d'un widget
        uint16_t widgetHeight = 120;      // Hauteur d'un widget
        uint16_t arcRadius = 70;          // Rayon de l'arc des widgets
        uint8_t buttonIndicatorSize = 12; // Taille des indicateurs de bouton
        uint32_t backgroundColor = 0x000000; // Couleur de fond
        bool enableLogging = false;       // Activation des logs
        bool enableAnimation = true;      // Activation des animations
    };

    /**
     * @brief Constructeur
     * @param config Configuration du contrôleur
     * @param bridge Bridge LVGL vers hardware
     * @param unifiedConfig Configuration unifiée pour mappings MIDI
     * @param eventBus Bus d'événements pour abonnement
     */
    explicit ParameterViewController(const ControllerConfig& config,
                                   std::shared_ptr<Ili9341LvglBridge> bridge,
                                   std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                                   std::shared_ptr<EventBus> eventBus);

    /**
     * @brief Destructeur
     */
    ~ParameterViewController();

    /**
     * @brief Initialise le contrôleur et tous ses composants
     * @return true si l'initialisation réussit
     */
    bool initialize();

    /**
     * @brief Met à jour le contrôleur (appelé régulièrement)
     */
    void update();

    /**
     * @brief Active ou désactive le contrôleur
     * @param active État d'activation
     */
    void setActive(bool active);

    /**
     * @brief Vérifie si le contrôleur est actif
     * @return true si actif
     */
    bool isActive() const;

    /**
     * @brief Obtient l'écran principal pour affichage
     * @return Pointeur vers l'écran LVGL ou nullptr
     */
    lv_obj_t* getMainScreen() const;

    /**
     * @brief Met à jour un paramètre MIDI
     * @param cc_number Numéro CC (0-127)
     * @param channel Canal MIDI (1-16)
     * @param value Valeur (0-127)
     * @param parameter_name Nom du paramètre
     * @param animate Utiliser animation
     */
    void setParameter(uint8_t cc_number, uint8_t channel, uint8_t value,
                     const String& parameter_name, bool animate = true);

    /**
     * @brief Met à jour l'état d'un bouton
     * @param button_id ID du bouton
     * @param pressed État du bouton
     * @param animate Utiliser animation
     */
    void setButtonState(uint16_t button_id, bool pressed, bool animate = true);

    /**
     * @brief Obtient le widget pour un CC donné
     * @param cc_number Numéro CC MIDI
     * @return Pointeur vers le widget ou nullptr
     */
    ParameterWidget* getWidgetForCC(uint8_t cc_number);

    /**
     * @brief Obtient le widget pour un bouton donné
     * @param button_id ID du bouton
     * @return Pointeur vers le widget ou nullptr
     */
    ParameterWidget* getWidgetForButton(uint16_t button_id);

    /**
     * @brief Interface EventListener - traite les événements reçus
     * @param event Événement à traiter
     * @return true si l'événement a été traité
     */
    bool onEvent(const Event& event) override;

    /**
     * @brief Finalise le positionnement des éléments LVGL
     */
    void finalizePositioning();

    /**
     * @brief Contrôle la visibilité des widgets
     * @param visible État de visibilité
     */
    void setWidgetsVisible(bool visible);

    /**
     * @brief Vérifie si le contrôleur est initialisé
     * @return true si initialisé
     */
    bool isInitialized() const;

private:
    // Configuration
    ControllerConfig config_;
    
    // Dépendances externes
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    std::shared_ptr<UnifiedConfiguration> unifiedConfig_;
    std::shared_ptr<EventBus> eventBus_;
    
    // État
    bool initialized_;
    bool active_;
    SubscriptionId event_subscription_id_;
    
    // Composants orchestrés (Phase 5.2-5.5)
    std::unique_ptr<MidiConfigurationParser> configParser_;
    std::unique_ptr<WidgetMappingManager> mappingManager_;
    std::unique_ptr<ParameterEventHandler> eventHandler_;
    std::unique_ptr<LvglSceneManager> sceneManager_;
    
    // Méthodes privées d'initialisation
    bool initializeConfigParser();
    bool initializeMappingManager();
    bool initializeSceneManager();
    bool initializeEventHandler();
    bool initializeMappingsFromConfig();
    
    // Gestion des événements
    void subscribeToEvents();
    void unsubscribeFromEvents();
    bool handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event);
    bool handleButtonEvent(const Event& event);
    
    // Utilitaires
    void logInfo(const String& message) const;
    void logDebug(const String& message) const;
    void logError(const String& message) const;
};