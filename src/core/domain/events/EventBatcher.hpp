#pragma once

#include "core/domain/events/core/Event.hpp"
#include "core/domain/events/core/IEventBus.hpp"
#include "config/PerformanceConfig.hpp"
#include "MidiEvents.hpp"
#include "UIEvent.hpp"
#include <map>
#include <memory>
#include <Arduino.h>

/**
 * @brief Système de batching d'événements pour optimiser les performances UI
 * 
 * Convertit les événements MIDI haute fréquence en événements UI
 * optimisés avec throttling intelligent.
 */
class EventBatcher : public EventListener, public std::enable_shared_from_this<EventBatcher> {
public:
    /**
     * @brief Configuration du batching
     */
    struct BatchConfig {
        unsigned long ui_update_interval_ms;  ///< 60 FPS pour UI
        unsigned long status_update_interval_ms;  ///< 10 FPS pour status
        bool coalesce_identical_values;  ///< Fusionner valeurs identiques
        
        // Constructeur avec valeurs par défaut
        BatchConfig() : ui_update_interval_ms(PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS), status_update_interval_ms(100), coalesce_identical_values(true) {}
    };

    /**
     * @brief Constructeur
     * @param config Configuration du batching
     * @param eventBus Bus d'événements pour s'abonner/publier
     */
    explicit EventBatcher(const BatchConfig& config = BatchConfig(), std::shared_ptr<MidiController::Events::IEventBus> eventBus = nullptr);

    /**
     * @brief Démarrer le batching (s'abonner aux événements MIDI)
     */
    void start();

    /**
     * @brief Arrêter le batching
     */
    void stop();

    /**
     * @brief Traiter les batchs en attente (appelé périodiquement)
     */
    void processPendingBatches();

    // Interface EventListener
    bool onEvent(const Event& event) override;

private:
    /**
     * @brief État d'un paramètre en attente de batch
     */
    struct PendingParameter {
        uint8_t controller;
        uint8_t channel;
        uint8_t value;
        String name;
        unsigned long last_update_ms;
        bool needs_ui_update = false;
    };

    BatchConfig config_;
    bool started_ = false;
    SubscriptionId subscription_id_ = 0;
    std::shared_ptr<MidiController::Events::IEventBus> eventBus_;
    
    // État des paramètres en attente
    std::map<uint16_t, PendingParameter> pending_parameters_;  // Key: (channel << 8) | controller
    
    // Timers
    unsigned long last_ui_batch_ms_ = 0;
    unsigned long last_status_batch_ms_ = 0;

    /**
     * @brief Traite un événement MIDI CC
     */
    void handleMidiCCEvent(const MidiCCEvent& midi_event);

    /**
     * @brief Génère une clé unique pour un paramètre
     */
    uint16_t getParameterKey(uint8_t controller, uint8_t channel) const {
        return (static_cast<uint16_t>(channel) << 8) | controller;
    }

    /**
     * @brief Envoie les événements UI batchés
     */
    void flushUIBatch();

    /**
     * @brief Envoie les événements de status batchés
     */
    void flushStatusBatch();
};