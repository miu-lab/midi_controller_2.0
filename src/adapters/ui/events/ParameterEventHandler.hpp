#pragma once

#include <array>
#include <functional>
#include <memory>

#include "ParameterWidgetMappingManager.hpp"
#include "adapters/ui/components/ParameterWidget.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/UIEvent.hpp"
#include "core/domain/events/core/EventBus.hpp"

/**
 * @brief Gestionnaire d'événements pour les paramètres MIDI et boutons
 * 
 * Cette classe centralise la logique de traitement des événements MIDI (UIParameterUpdateEvent)
 * et des événements de boutons (HighPriorityButtonPressEvent) pour simplifier LvglParameterView.
 * Respecte le principe de responsabilité unique.
 */
class ParameterEventHandler : public EventListener {
public:
    /**
     * @brief Configuration pour le gestionnaire d'événements
     */
    struct EventConfig {
        bool enableMidiEvents;        // Traiter les événements MIDI
        bool enableButtonEvents;      // Traiter les événements de boutons
        bool enableLogging;           // Activer le logging de debug
        bool enableAnimation;         // Utiliser les animations par défaut
        
        EventConfig() : enableMidiEvents(true), enableButtonEvents(true), 
                       enableLogging(false), enableAnimation(true) {}
    };

    /**
     * @brief Callback pour accéder aux widgets de paramètres
     * @param index Index du widget (0-7)
     * @return Pointeur vers le widget ou nullptr si invalide
     */
    using WidgetAccessor = std::function<ParameterWidget*(uint8_t index)>;

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration du gestionnaire
     * @param widgetAccessor Fonction pour accéder aux widgets
     * @param mappingManager Gestionnaire de mappings pour résoudre CC→Widget et Button→Widget
     */
    explicit ParameterEventHandler(const EventConfig& config, WidgetAccessor widgetAccessor,
                                   std::shared_ptr<ParameterWidgetMappingManager> mappingManager);

    /**
     * @brief Destructeur par défaut
     */
    ~ParameterEventHandler() = default;

    /**
     * @brief Interface EventListener - traite les événements reçus
     * @param event Événement à traiter
     * @return true si l'événement a été traité
     */
    bool onEvent(const Event& event) override;

    /**
     * @brief Active ou désactive le gestionnaire
     * @param active État d'activation
     */
    void setActive(bool active);

    /**
     * @brief Vérifie si le gestionnaire est actif
     * @return true si actif
     */
    bool isActive() const;

    /**
     * @brief Met à jour la configuration du gestionnaire
     * @param config Nouvelle configuration
     */
    void updateConfig(const EventConfig& config);

    /**
     * @brief Obtient les statistiques de traitement d'événements
     */
    struct EventStats {
        uint32_t midiEventsProcessed;
        uint32_t buttonEventsProcessed;
        uint32_t eventsIgnored;
        uint32_t totalEvents;
    };
    EventStats getStats() const;

    /**
     * @brief Remet à zéro les statistiques
     */
    void resetStats();

private:
    EventConfig config_;
    WidgetAccessor widgetAccessor_;
    std::shared_ptr<ParameterWidgetMappingManager> mappingManager_;
    bool active_;
    
    // Statistiques
    mutable EventStats stats_;

    /**
     * @brief Traite un événement de mise à jour de paramètre MIDI
     * @param event Événement UIParameterUpdateEvent
     * @return true si traité avec succès
     */
    bool handleUIParameterUpdateEvent(const UIParameterUpdateEvent& event);

    /**
     * @brief Traite un événement de bouton haute priorité
     * @param event Événement HighPriorityButtonPressEvent
     * @return true si traité avec succès
     */
    bool handleButtonEvent(const Event& event);

    /**
     * @brief Obtient un widget par index avec validation
     * @param index Index du widget (0-7)
     * @return Pointeur vers le widget ou nullptr si invalide
     */
    ParameterWidget* getWidget(uint8_t index) const;

    /**
     * @brief Obtient un widget pour un CC donné
     * @param cc_number Numéro CC MIDI
     * @return Pointeur vers le widget ou nullptr si non mappé
     */
    ParameterWidget* getWidgetForCC(uint8_t cc_number) const;

    /**
     * @brief Obtient un widget pour un bouton donné
     * @param button_id ID du bouton
     * @return Pointeur vers le widget ou nullptr si non mappé
     */
    ParameterWidget* getWidgetForButton(uint16_t button_id) const;

    /**
     * @brief Log une information si le logging est activé
     * @param message Message à logger
     */
    void logInfo(const String& message) const;

    /**
     * @brief Log un debug si le logging est activé
     * @param message Message de debug à logger
     */
    void logDebug(const String& message) const;

    /**
     * @brief Log une erreur si le logging est activé
     * @param message Message d'erreur à logger
     */
    void logError(const String& message) const;
};