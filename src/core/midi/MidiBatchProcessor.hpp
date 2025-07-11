#pragma once

#include "config/SystemConstants.hpp"
#include "config/ETLConfig.hpp"
#include "core/domain/types.hpp"
#include <array>
#include <atomic>
#include <Arduino.h>

/**
 * @brief Processeur de batch MIDI optimisé avec allocation statique
 * 
 * Cette classe remplace std::map par un tableau indexé pour
 * éliminer complètement les allocations dynamiques dans le batching MIDI.
 */
class MidiBatchProcessor {
public:
    /**
     * @brief Paramètre MIDI en attente de batch
     */
    struct PendingParameter {
        uint8_t controller;
        uint8_t channel;
        uint8_t value;
        uint32_t last_update_ms;
        bool needs_ui_update;
        bool needs_status_update;
        bool active;  // Slot occupé ou libre
        
        PendingParameter() 
            : controller(0), channel(0), value(0)
            , last_update_ms(0), needs_ui_update(false)
            , needs_status_update(false), active(false) {}
    };
    
    /**
     * @brief Configuration du batch processor
     */
    struct Config {
        uint32_t ui_update_interval_ms;
        uint32_t status_update_interval_ms;
        bool coalesce_identical_values;
        bool enable_ui_batching;
        bool enable_status_batching;
        
        Config() 
            : ui_update_interval_ms(SystemConstants::Performance::DISPLAY_REFRESH_PERIOD_MS)
            , status_update_interval_ms(100)
            , coalesce_identical_values(true)
            , enable_ui_batching(true)
            , enable_status_batching(true) {}
    };
    
    /**
     * @brief Callback pour événements UI batchés
     */
    using UIBatchCallback = void(*)(uint8_t controller, uint8_t channel, uint8_t value, void* userdata);
    
    /**
     * @brief Callback pour événements status batchés
     */
    using StatusBatchCallback = void(*)(const PendingParameter* params, size_t count, void* userdata);
    
    /**
     * @brief Constructeur avec configuration
     */
    explicit MidiBatchProcessor(const Config& config = Config())
        : config_(config)
        , last_ui_batch_ms_(0)
        , last_status_batch_ms_(0)
        , active_parameter_count_(0)
        , ui_callback_(nullptr)
        , status_callback_(nullptr)
        , ui_userdata_(nullptr)
        , status_userdata_(nullptr) {
        
        // Initialiser le tableau de paramètres
        for (auto& param : parameters_) {
            param = PendingParameter{};
        }
    }
    
    /**
     * @brief Destructeur par défaut
     */
    ~MidiBatchProcessor() = default;
    
    // === CONFIGURATION DES CALLBACKS ===
    
    /**
     * @brief Configure le callback pour les événements UI
     */
    void setUICallback(UIBatchCallback callback, void* userdata = nullptr) {
        ui_callback_ = callback;
        ui_userdata_ = userdata;
    }
    
    /**
     * @brief Configure le callback pour les événements status
     */
    void setStatusCallback(StatusBatchCallback callback, void* userdata = nullptr) {
        status_callback_ = callback;
        status_userdata_ = userdata;
    }
    
    // === TRAITEMENT DES PARAMÈTRES ===
    
    /**
     * @brief Ajoute ou met à jour un paramètre MIDI pour batching
     * 
     * @param controller Numéro du contrôleur CC
     * @param channel Canal MIDI
     * @param value Nouvelle valeur
     * @return true si ajouté/mis à jour, false si erreur
     */
    bool addParameter(uint8_t controller, uint8_t channel, uint8_t value) {
        uint32_t now = millis();
        
        // Chercher un paramètre existant
        int index = findParameter(controller, channel);
        
        if (index >= 0) {
            // Paramètre existant - mise à jour
            PendingParameter& param = parameters_[index];
            
            // Vérifier coalescing des valeurs identiques
            if (config_.coalesce_identical_values && param.value == value) {
                return true; // Valeur identique, pas de mise à jour nécessaire
            }
            
            param.value = value;
            param.last_update_ms = now;
            
            if (config_.enable_ui_batching) {
                param.needs_ui_update = true;
            }
            if (config_.enable_status_batching) {
                param.needs_status_update = true;
            }
            
            return true;
        } else {
            // Nouveau paramètre - chercher un slot libre
            index = findFreeSlot();
            if (index < 0) {
                return false; // Pas de slot libre
            }
            
            PendingParameter& param = parameters_[index];
            param.controller = controller;
            param.channel = channel;
            param.value = value;
            param.last_update_ms = now;
            param.needs_ui_update = config_.enable_ui_batching;
            param.needs_status_update = config_.enable_status_batching;
            param.active = true;
            
            active_parameter_count_.fetch_add(1, std::memory_order_relaxed);
            return true;
        }
    }
    
    /**
     * @brief Traite les batchs en attente
     * 
     * Cette méthode doit être appelée régulièrement pour envoyer
     * les événements batchés selon les intervalles configurés.
     */
    void processPendingBatches() {
        uint32_t now = millis();
        
        // Traiter le batch UI si nécessaire
        if (config_.enable_ui_batching && 
            (now - last_ui_batch_ms_) >= config_.ui_update_interval_ms) {
            flushUIBatch();
            last_ui_batch_ms_ = now;
        }
        
        // Traiter le batch status si nécessaire
        if (config_.enable_status_batching && 
            (now - last_status_batch_ms_) >= config_.status_update_interval_ms) {
            flushStatusBatch();
            last_status_batch_ms_ = now;
        }
    }
    
    /**
     * @brief Force l'envoi immédiat de tous les batchs
     */
    void flushAllBatches() {
        if (config_.enable_ui_batching) {
            flushUIBatch();
        }
        if (config_.enable_status_batching) {
            flushStatusBatch();
        }
    }
    
    // === STATISTIQUES ET MONITORING ===
    
    /**
     * @brief Statistiques du batch processor
     */
    struct Stats {
        size_t active_parameters;
        size_t total_capacity;
        float usage_ratio;
        uint32_t ui_batches_sent;
        uint32_t status_batches_sent;
        uint32_t parameters_coalesced;
    };
    
    /**
     * @brief Obtient les statistiques actuelles
     */
    Stats getStats() const {
        size_t active = active_parameter_count_.load(std::memory_order_relaxed);
        return {
            active,
            SystemConstants::Performance::MAX_MIDI_PENDING_PARAMS,
            static_cast<float>(active) / static_cast<float>(SystemConstants::Performance::MAX_MIDI_PENDING_PARAMS),
            ui_batches_sent_.load(std::memory_order_relaxed),
            status_batches_sent_.load(std::memory_order_relaxed),
            parameters_coalesced_.load(std::memory_order_relaxed)
        };
    }
    
    /**
     * @brief Réinitialise les statistiques
     */
    void resetStats() {
        ui_batches_sent_.store(0, std::memory_order_relaxed);
        status_batches_sent_.store(0, std::memory_order_relaxed);
        parameters_coalesced_.store(0, std::memory_order_relaxed);
    }
    
    /**
     * @brief Vide tous les paramètres en attente
     */
    void clear() {
        for (auto& param : parameters_) {
            param.active = false;
        }
        active_parameter_count_.store(0, std::memory_order_relaxed);
    }
    
    /**
     * @brief Vérifie si le batch processor approche de la saturation
     */
    bool isNearCapacity() const {
        return getStats().usage_ratio > 0.8f;
    }

private:
    // Empêcher la copie
    MidiBatchProcessor(const MidiBatchProcessor&) = delete;
    MidiBatchProcessor& operator=(const MidiBatchProcessor&) = delete;
    
    /**
     * @brief Cherche un paramètre existant
     * 
     * @param controller Numéro du contrôleur
     * @param channel Canal MIDI
     * @return Index du paramètre, ou -1 si non trouvé
     */
    int findParameter(uint8_t controller, uint8_t channel) const {
        for (size_t i = 0; i < SystemConstants::Performance::MAX_MIDI_PENDING_PARAMS; ++i) {
            const auto& param = parameters_[i];
            if (param.active && param.controller == controller && param.channel == channel) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    /**
     * @brief Cherche un slot libre dans le tableau
     * 
     * @return Index du slot libre, ou -1 si aucun disponible
     */
    int findFreeSlot() const {
        for (size_t i = 0; i < SystemConstants::Performance::MAX_MIDI_PENDING_PARAMS; ++i) {
            if (!parameters_[i].active) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    /**
     * @brief Envoie les événements UI batchés
     */
    void flushUIBatch() {
        if (!ui_callback_) {
            return;
        }
        
        size_t sent_count = 0;
        
        for (auto& param : parameters_) {
            if (param.active && param.needs_ui_update) {
                ui_callback_(param.controller, param.channel, param.value, ui_userdata_);
                param.needs_ui_update = false;
                sent_count++;
            }
        }
        
        if (sent_count > 0) {
            ui_batches_sent_.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    /**
     * @brief Envoie les événements status batchés
     */
    void flushStatusBatch() {
        if (!status_callback_) {
            return;
        }
        
        // Collecter tous les paramètres qui ont besoin d'une mise à jour status
        static std::array<PendingParameter, SystemConstants::Performance::MAX_MIDI_PENDING_PARAMS> status_params;
        size_t count = 0;
        
        for (auto& param : parameters_) {
            if (param.active && param.needs_status_update && 
                count < SystemConstants::Performance::MAX_MIDI_PENDING_PARAMS) {
                status_params[count] = param;
                param.needs_status_update = false;
                count++;
            }
        }
        
        if (count > 0) {
            status_callback_(status_params.data(), count, status_userdata_);
            status_batches_sent_.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    // === DONNÉES MEMBRES ===
    
    Config config_;
    
    // Tableau statique pour les paramètres (remplace std::map)
    std::array<PendingParameter, SystemConstants::Performance::MAX_MIDI_PENDING_PARAMS> parameters_;
    
    // Timers pour les batchs
    uint32_t last_ui_batch_ms_;
    uint32_t last_status_batch_ms_;
    
    // Compteur atomique des paramètres actifs
    std::atomic<size_t> active_parameter_count_;
    
    // Callbacks
    UIBatchCallback ui_callback_;
    StatusBatchCallback status_callback_;
    void* ui_userdata_;
    void* status_userdata_;
    
    // Statistiques
    mutable std::atomic<uint32_t> ui_batches_sent_{0};
    mutable std::atomic<uint32_t> status_batches_sent_{0};
    mutable std::atomic<uint32_t> parameters_coalesced_{0};
};