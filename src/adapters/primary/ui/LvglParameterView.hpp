#pragma once

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/ui/lvgl/widgets/ParameterWidget.hpp"
#include "adapters/ui/lvgl/widgets/ButtonIndicator.hpp"
#include "adapters/primary/ui/parameter/MidiConfigurationParser.hpp"
#include "adapters/primary/ui/parameter/WidgetMappingManager.hpp"
#include "adapters/primary/ui/parameter/ParameterEventHandler.hpp"
#include "adapters/primary/ui/parameter/LvglSceneManager.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/UIEvent.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include <memory>
#include <array>
#include <map>
#include <lvgl.h>

/**
 * @brief Vue LVGL pour affichage de paramètres MIDI avec grille de 8 ParameterWidget
 * 
 * Cette vue utilise 8 ParameterWidget arrangés en grille 4x2 basés sur lv_arc natif
 * pour afficher les paramètres MIDI de manière interactive.
 * 
 * Flux unidirectionnel MIDI→UI : écoute les UIParameterUpdateEvent batchés et met à jour l'affichage.
 * L'UI est en lecture seule et ne génère pas d'événements MIDI.
 */
class LvglParameterView : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param bridge Bridge LVGL vers hardware
     * @param config Configuration unifiée pour accéder aux mappings MIDI
     * @param eventBus Bus d'événements optimisé pour recevoir les événements boutons
     */
    explicit LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge,
                              std::shared_ptr<UnifiedConfiguration> config,
                              std::shared_ptr<EventBus> eventBus);

    /**
     * @brief Destructeur
     */
    ~LvglParameterView();

    // Interface View
    bool init();
    void render();
    void update();
    bool isActive() const;
    void setActive(bool active);

    // Interface paramètre MIDI
    /**
     * @brief Met à jour le paramètre affiché
     * @param cc_number Numéro CC (0-127)
     * @param channel Canal MIDI (1-16)
     * @param value Valeur (0-127)
     * @param parameter_name Nom du paramètre
     * @param animate Utiliser animation
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
     * @brief Obtient l'index du widget pour un CC donné
     * @param cc_number Numéro CC MIDI (0-127)
     * @return Index du widget (0-7) ou -1 si non mappé
     */
    int8_t getWidgetIndexForCC(uint8_t cc_number) const;
    
    /**
     * @brief Obtient le widget pour un CC donné
     * @param cc_number Numéro CC MIDI
     * @return Pointeur vers le widget ou nullptr si non mappé
     */
    ParameterWidget* getWidgetForCC(uint8_t cc_number);

    // === GESTION DES BOUTONS ===
    
    /**
     * @brief Obtient l'index du widget pour un bouton donné (bouton enfant d'encodeur)
     * @param button_id ID du bouton
     * @return Index du widget parent (0-7) ou -1 si non mappé
     */
    int8_t getWidgetIndexForButton(uint16_t button_id) const;
    
    /**
     * @brief Met à jour l'état d'un bouton associé à un encodeur
     * @param button_id ID du bouton
     * @param pressed État du bouton
     * @param animate Utiliser animation
     */
    void setButtonState(uint16_t button_id, bool pressed, bool animate = true);
    
    /**
     * @brief Obtient le widget parent pour un bouton donné
     * @param button_id ID du bouton
     * @return Pointeur vers le widget parent ou nullptr si non mappé
     */
    ParameterWidget* getWidgetForButton(uint16_t button_id);

    // === INTERFACE EVENT LISTENER ===
    
    /**
     * @brief Traite les événements reçus (interface EventListener)
     * @param event Événement à traiter
     * @return true si l'événement a été traité
     */
    bool onEvent(const Event& event) override;

private:
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    std::shared_ptr<UnifiedConfiguration> config_;
    std::shared_ptr<EventBus> eventBus_;
    
    // État
    bool initialized_;
    bool active_;
    
    // Event handling
    SubscriptionId event_subscription_id_;
    
    // Parser pour extraction configuration MIDI (Phase 5.2 refactoring)
    std::unique_ptr<MidiConfigurationParser> configParser_;
    
    // Gestionnaire de mappings CC→Widget et Button→Widget (Phase 5.3 refactoring)
    std::unique_ptr<WidgetMappingManager> mappingManager_;
    
    // Gestionnaire d'événements pour MIDI et boutons (Phase 5.4 refactoring)
    std::unique_ptr<ParameterEventHandler> eventHandler_;
    
    // Gestionnaire de scène LVGL pour les objets et widgets (Phase 5.5 refactoring)
    std::unique_ptr<LvglSceneManager> sceneManager_;
    
    // Méthodes privées - Création du gestionnaire de scène LVGL (Phase 5.5 refactoring)
    void createSceneManager();
    
    // Gestion du mapping CC->Widget depuis la configuration (Phase 5.3 refactoring)
    void initializeMappingsFromConfig();
    
    // Création du gestionnaire d'événements (Phase 5.4 refactoring)
    void createEventHandler();
    
    // Event handling
    void subscribeToEvents();
    void unsubscribeFromEvents();
    bool handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event);
    bool handleButtonEvent(const Event& event);
};