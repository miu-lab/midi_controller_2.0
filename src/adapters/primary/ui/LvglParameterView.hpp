#pragma once

#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/ui/lvgl/widgets/ParameterWidget.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/UIEvent.hpp"
#include <memory>
#include <lvgl.h>

/**
 * @brief Vue LVGL pour affichage de paramètres MIDI avec ParameterWidget
 * 
 * Cette vue utilise le nouveau ParameterWidget basé sur lv_arc natif
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
     */
    explicit LvglParameterView(std::shared_ptr<Ili9341LvglBridge> bridge);

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

    // === INTERFACE EVENT LISTENER ===
    
    /**
     * @brief Traite les événements reçus (interface EventListener)
     * @param event Événement à traiter
     * @return true si l'événement a été traité
     */
    bool onEvent(const Event& event) override;

private:
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    
    // Widget principal
    std::unique_ptr<ParameterWidget> parameter_widget_;
    
    // Objets LVGL
    lv_obj_t* main_screen_;
    
    // État
    bool initialized_;
    bool active_;
    
    // Event handling
    SubscriptionId event_subscription_id_;
    
    // Dernier paramètre MIDI reçu
    uint8_t last_cc_number_;
    uint8_t last_channel_;
    uint8_t last_value_;
    String last_parameter_name_;
    
    // Méthodes privées
    void setupMainScreen();
    void createParameterWidget();
    void cleanupLvglObjects();
    
    // Event handling
    void subscribeToEvents();
    void unsubscribeFromEvents();
    bool handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event);
};