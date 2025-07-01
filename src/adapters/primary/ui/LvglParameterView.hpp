#pragma once

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/ui/lvgl/widgets/ParameterWidget.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/UIEvent.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include <memory>
#include <array>
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
     */
    explicit LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge,
                              std::shared_ptr<UnifiedConfiguration> config);

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
    
    // Container grille et widgets
    lv_obj_t* grid_container_;
    std::array<std::unique_ptr<ParameterWidget>, 8> parameter_widgets_;
    
    // Objets LVGL
    lv_obj_t* main_screen_;
    
    // État
    bool initialized_;
    bool active_;
    
    // Event handling
    SubscriptionId event_subscription_id_;
    
    // Configuration des widgets (CC 1-8 mappés aux widgets 0-7)
    // Le mapping est géré dans le .cpp avec un tableau statique
    
    // Méthodes privées
    void setupMainScreen();
    void createGridContainer();
    void createParameterWidgets();
    void cleanupLvglObjects();
    
    // Gestion du mapping CC->Widget depuis la configuration
    void initializeCCMappingFromConfig();
    void initializeWidgetConfigurationsFromConfig();
    
    // Structure pour les infos extraites de la config
    struct MidiControlInfo {
        uint8_t cc_number;
        uint8_t channel;
        String name;
        InputId control_id;
    };
    std::vector<MidiControlInfo> extractMidiControlsFromConfig();
    
    // Event handling
    void subscribeToEvents();
    void unsubscribeFromEvents();
    bool handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event);
};