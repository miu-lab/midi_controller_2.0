#pragma once

#include "OptimizedMidiProcessor.hpp"
#include "MidiBatchProcessor.hpp"
#include "core/memory/RingBuffer.hpp"
#include "core/memory/EventPoolManager.hpp"
#include "config/PerformanceConfig.hpp"
#include <memory>

/**
 * @brief Gestionnaire MIDI haute performance pour latence < 1ms
 * 
 * Cette classe unifie tous les composants MIDI optimisés :
 * - OptimizedMidiProcessor pour callbacks sans allocation
 * - MidiBatchProcessor pour batching avec tableau statique
 * - RingBuffer pour queuing des messages
 * - Integration avec EventPoolManager pour événements
 */
class HighPerformanceMidiManager {
public:
    /**
     * @brief Configuration globale du gestionnaire MIDI
     */
    struct Config {
        OptimizedMidiProcessor::Config processor_config;
        MidiBatchProcessor::Config batch_config;
        bool enable_event_integration;
        bool enable_performance_monitoring;
        uint32_t monitoring_interval_ms;
        
        Config() 
            : enable_event_integration(true)
            , enable_performance_monitoring(true)
            , monitoring_interval_ms(1000) {}
    };
    
    /**
     * @brief Statistiques globales de performance MIDI
     */
    struct GlobalStats {
        OptimizedMidiProcessor::PerformanceStats processor_stats;
        MidiBatchProcessor::Stats batch_stats;
        OptimizedMidiProcessor::BufferStatus buffer_status;
        
        // Métriques agrégées
        uint32_t total_messages_per_second;
        uint32_t total_latency_us;
        float system_load_ratio;
        bool is_realtime_capable;
    };
    
    /**
     * @brief Constructeur avec configuration et pool manager
     */
    explicit HighPerformanceMidiManager(
        const Config& config = Config(),
        std::shared_ptr<EventPoolManager> pool_manager = nullptr)
        : config_(config)
        , processor_(config.processor_config)
        , batch_processor_(config.batch_config)
        , pool_manager_(pool_manager)
        , last_monitoring_ms_(0)
        , messages_last_second_(0) {
        
        // Configurer les callbacks du batch processor
        setupBatchCallbacks();
        
        // Enregistrer les callbacks MIDI de base
        setupMidiCallbacks();
    }
    
    /**
     * @brief Destructeur par défaut
     */
    ~HighPerformanceMidiManager() = default;
    
    // === API PUBLIQUE SIMPLIFIÉE ===
    
    /**
     * @brief Traite un message MIDI entrant (appelé depuis ISR ou loop principal)
     * 
     * @param status Status byte MIDI
     * @param data1 Premier data byte
     * @param data2 Deuxième data byte
     * @return true si traité avec succès
     */
    bool processMidiMessage(uint8_t status, uint8_t data1, uint8_t data2) {
        // Ajouter au buffer de traitement rapide
        return processor_.enqueueMidiFast(status, data1, data2);
    }
    
    /**
     * @brief Méthode principale à appeler dans la boucle principale
     * 
     * Cette méthode doit être appelée très régulièrement (idéalement à chaque cycle)
     * pour maintenir les performances temps réel.
     */
    void update() {
        // Traiter les messages MIDI entrants
        processor_.processIncomingMessages();
        
        // Traiter les batchs en attente
        batch_processor_.processPendingBatches();
        
        // Monitoring de performance
        if (config_.enable_performance_monitoring) {
            updatePerformanceMonitoring();
        }
    }
    
    /**
     * @brief Enregistre un callback pour Control Change
     * 
     * @param callback Fonction callback
     * @param userdata Données utilisateur optionnelles
     * @return ID du callback, ou -1 si échec
     */
    int onControlChange(OptimizedMidiProcessor::CcCallback callback, void* userdata = nullptr) {
        return processor_.registerCcCallback(callback, userdata);
    }
    
    /**
     * @brief Enregistre un callback pour Note On
     */
    int onNoteOn(OptimizedMidiProcessor::NoteCallback callback, void* userdata = nullptr) {
        return processor_.registerNoteOnCallback(callback, userdata);
    }
    
    /**
     * @brief Enregistre un callback pour Note Off
     */
    int onNoteOff(OptimizedMidiProcessor::NoteCallback callback, void* userdata = nullptr) {
        return processor_.registerNoteOffCallback(callback, userdata);
    }
    
    /**
     * @brief Désactive un callback Control Change
     */
    bool removeControlChangeCallback(int callback_id) {
        return processor_.unregisterCcCallback(callback_id);
    }
    
    /**
     * @brief Désactive un callback Note On
     */
    bool removeNoteOnCallback(int callback_id) {
        return processor_.unregisterNoteOnCallback(callback_id);
    }
    
    /**
     * @brief Désactive un callback Note Off
     */
    bool removeNoteOffCallback(int callback_id) {
        return processor_.unregisterNoteOffCallback(callback_id);
    }
    
    // === GESTION DES ÉVÉNEMENTS UI ===
    
    /**
     * @brief Configure un callback pour recevoir les événements UI batchés
     */
    void setUIEventCallback(MidiBatchProcessor::UIBatchCallback callback, void* userdata = nullptr) {
        batch_processor_.setUICallback(callback, userdata);
    }
    
    /**
     * @brief Configure un callback pour recevoir les événements status batchés
     */
    void setStatusEventCallback(MidiBatchProcessor::StatusBatchCallback callback, void* userdata = nullptr) {
        batch_processor_.setStatusCallback(callback, userdata);
    }
    
    // === STATISTIQUES ET MONITORING ===
    
    /**
     * @brief Obtient les statistiques globales de performance
     */
    GlobalStats getGlobalStats() const {
        auto processor_stats = processor_.getStats();
        auto batch_stats = batch_processor_.getStats();
        auto buffer_status = processor_.getBufferStatus();
        
        GlobalStats global_stats;
        // Copie manuelle pour éviter les problèmes avec std::atomic
        global_stats.processor_stats = processor_stats;
        global_stats.batch_stats = batch_stats;
        global_stats.buffer_status = buffer_status;
        
        // Calculer les métriques agrégées
        global_stats.total_messages_per_second = messages_last_second_;
        global_stats.total_latency_us = processor_stats.avg_latency_us;
        global_stats.system_load_ratio = calculateSystemLoad();
        global_stats.is_realtime_capable = isRealtimeCapable();
        
        return global_stats;
    }
    
    /**
     * @brief Vérifie si le système maintient les performances temps réel
     */
    bool isRealtimeCapable() const {
        auto stats = processor_.getStats();
        auto buffer_status = processor_.getBufferStatus();
        
        return stats.max_latency_us < PerformanceConfig::MAX_MIDI_LATENCY_US &&
               buffer_status.incoming_usage < 0.8f &&
               !processor_.isOverloaded();
    }
    
    /**
     * @brief Réinitialise toutes les statistiques
     */
    void resetAllStats() {
        processor_.resetStats();
        batch_processor_.resetStats();
        messages_last_second_ = 0;
    }
    
    /**
     * @brief Force l'envoi de tous les batchs en attente
     */
    void flushAllBatches() {
        batch_processor_.flushAllBatches();
    }
    
    /**
     * @brief Obtient des informations de diagnostic détaillées
     */
    String getDiagnosticInfo() const {
        auto global_stats = getGlobalStats();
        
        String info;
        info += "=== MIDI Performance Diagnostics ===\n";
        info += "Messages/sec: " + String(global_stats.total_messages_per_second) + "\n";
        info += "Avg Latency: " + String(global_stats.total_latency_us) + "μs\n";
        info += "Max Latency: " + String(global_stats.processor_stats.max_latency_us) + "μs\n";
        info += "Buffer Usage: " + String(global_stats.buffer_status.incoming_usage * 100.0f, 1) + "%\n";
        info += "Batch Usage: " + String(global_stats.batch_stats.usage_ratio * 100.0f, 1) + "%\n";
        info += "System Load: " + String(global_stats.system_load_ratio * 100.0f, 1) + "%\n";
        info += "Realtime: " + String(global_stats.is_realtime_capable ? "YES" : "NO") + "\n";
        info += "Buffer Overruns: " + String(global_stats.processor_stats.buffer_overruns) + "\n";
        info += "Callback Errors: " + String(global_stats.processor_stats.callback_errors) + "\n";
        
        return info;
    }

private:
    // Empêcher la copie
    HighPerformanceMidiManager(const HighPerformanceMidiManager&) = delete;
    HighPerformanceMidiManager& operator=(const HighPerformanceMidiManager&) = delete;
    
    /**
     * @brief Configure les callbacks du batch processor
     */
    void setupBatchCallbacks() {
        // Callback pour événements UI
        auto ui_callback = [](uint8_t controller, uint8_t channel, uint8_t value, void* userdata) {
            auto* self = static_cast<HighPerformanceMidiManager*>(userdata);
            self->handleUIBatchEvent(controller, channel, value);
        };
        
        batch_processor_.setUICallback(ui_callback, this);
        
        // Callback pour événements status
        auto status_callback = [](const MidiBatchProcessor::PendingParameter* params, size_t count, void* userdata) {
            auto* self = static_cast<HighPerformanceMidiManager*>(userdata);
            self->handleStatusBatchEvent(params, count);
        };
        
        batch_processor_.setStatusCallback(status_callback, this);
    }
    
    /**
     * @brief Configure les callbacks MIDI de base pour le batching
     */
    void setupMidiCallbacks() {
        // Callback pour Control Change - ajouter au batch processor
        auto cc_callback = [](MidiChannel channel, MidiCC cc, uint8_t value, void* userdata) {
            auto* self = static_cast<HighPerformanceMidiManager*>(userdata);
            self->batch_processor_.addParameter(cc, channel, value);
        };
        
        processor_.registerCcCallback(cc_callback, this);
    }
    
    /**
     * @brief Gère les événements UI batchés
     */
    void handleUIBatchEvent(uint8_t controller, uint8_t channel, uint8_t value) {
        if (config_.enable_event_integration && pool_manager_) {
            // Créer un événement UI via le pool manager
            auto* ui_event = pool_manager_->acquireUIParameterUpdateEvent(
                controller, channel, value, "CC" + String(controller)
            );
            
            if (ui_event) {
                // TODO: Publier l'événement via EventBus
                // Ceci nécessiterait une référence à EventBus
                
                // Pour l'instant, libérer l'événement
                pool_manager_->releaseUIParameterUpdateEvent(ui_event);
            }
        }
    }
    
    /**
     * @brief Gère les événements status batchés
     */
    void handleStatusBatchEvent(const MidiBatchProcessor::PendingParameter* params, size_t count) {
        // Traitement des événements status (logging, monitoring, etc.)
        // Implémentation selon les besoins spécifiques
    }
    
    /**
     * @brief Met à jour le monitoring de performance
     */
    void updatePerformanceMonitoring() {
        uint32_t now = millis();
        
        if ((now - last_monitoring_ms_) >= config_.monitoring_interval_ms) {
            // Calculer les messages par seconde
            auto current_messages = processor_.getStats().messages_processed;
            messages_last_second_ = current_messages - last_message_count_;
            last_message_count_ = current_messages;
            last_monitoring_ms_ = now;
        }
    }
    
    /**
     * @brief Calcule la charge système
     */
    float calculateSystemLoad() const {
        auto buffer_status = processor_.getBufferStatus();
        auto batch_stats = batch_processor_.getStats();
        
        // Combinaison de la charge buffer et batch
        float buffer_load = buffer_status.incoming_usage;
        float batch_load = batch_stats.usage_ratio;
        
        return (buffer_load + batch_load) / 2.0f;
    }
    
    // === DONNÉES MEMBRES ===
    
    Config config_;
    
    // Composants principaux
    OptimizedMidiProcessor processor_;
    MidiBatchProcessor batch_processor_;
    std::shared_ptr<EventPoolManager> pool_manager_;
    
    // Monitoring
    uint32_t last_monitoring_ms_;
    uint32_t messages_last_second_;
    uint32_t last_message_count_ = 0;
};