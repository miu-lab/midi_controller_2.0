#pragma once

#include "core/memory/RingBuffer.hpp"
#include "core/domain/types.hpp"
#include "config/PerformanceConfig.hpp"
#include <functional>
#include <array>
#include <atomic>

/**
 * @brief Processeur MIDI optimisé pour latence < 1ms
 * 
 * Cette classe remplace les allocations dynamiques par des structures
 * statiques et des ring buffers pour atteindre des performances temps réel.
 */
class OptimizedMidiProcessor {
public:
    // Types de callbacks optimisés (fonction pointers pour performance)
    using CcCallback = void(*)(MidiChannel, MidiCC, uint8_t, void* userdata);
    using NoteCallback = void(*)(MidiChannel, MidiNote, uint8_t, void* userdata);
    
    /**
     * @brief Structure pour stocker un callback avec contexte
     */
    struct CallbackEntry {
        union {
            CcCallback cc_callback;
            NoteCallback note_callback;
        };
        void* userdata;
        bool active;
        
        CallbackEntry() : cc_callback(nullptr), userdata(nullptr), active(false) {}
    };
    
    /**
     * @brief Statistiques de performance en temps réel
     */
    struct PerformanceStats {
        std::atomic<uint32_t> messages_processed{0};
        std::atomic<uint32_t> messages_dropped{0};
        std::atomic<uint32_t> max_latency_us{0};
        std::atomic<uint32_t> avg_latency_us{0};
        std::atomic<uint32_t> buffer_overruns{0};
        std::atomic<uint32_t> callback_errors{0};
    };
    
    /**
     * @brief Configuration du processeur
     */
    struct Config {
        bool enable_timestamping;
        bool enable_latency_monitoring;
        uint32_t max_latency_threshold_us;
        bool drop_on_overflow;
        
        Config() 
            : enable_timestamping(true)
            , enable_latency_monitoring(true)
            , max_latency_threshold_us(PerformanceConfig::MAX_MIDI_LATENCY_US)
            , drop_on_overflow(true) {}
    };
    
    /**
     * @brief Constructeur avec configuration
     */
    explicit OptimizedMidiProcessor(const Config& config = Config())
        : config_(config)
        , cc_callback_count_(0)
        , note_on_callback_count_(0)
        , note_off_callback_count_(0) {
        
        // Initialiser les tableaux de callbacks
        for (auto& entry : cc_callbacks_) {
            entry = CallbackEntry{};
        }
        for (auto& entry : note_on_callbacks_) {
            entry = CallbackEntry{};
        }
        for (auto& entry : note_off_callbacks_) {
            entry = CallbackEntry{};
        }
    }
    
    /**
     * @brief Destructeur par défaut
     */
    ~OptimizedMidiProcessor() = default;
    
    // === GESTION DES CALLBACKS (API STATIQUE) ===
    
    /**
     * @brief Enregistre un callback pour Control Change
     * 
     * @param callback Fonction callback
     * @param userdata Données utilisateur optionnelles
     * @return Index du callback, ou -1 si échec
     */
    int registerCcCallback(CcCallback callback, void* userdata = nullptr) {
        if (!callback || cc_callback_count_ >= PerformanceConfig::MAX_MIDI_CALLBACKS) {
            return -1;
        }
        
        size_t index = cc_callback_count_++;
        cc_callbacks_[index].cc_callback = callback;
        cc_callbacks_[index].userdata = userdata;
        cc_callbacks_[index].active = true;
        
        return static_cast<int>(index);
    }
    
    /**
     * @brief Enregistre un callback pour Note On
     */
    int registerNoteOnCallback(NoteCallback callback, void* userdata = nullptr) {
        if (!callback || note_on_callback_count_ >= PerformanceConfig::MAX_MIDI_CALLBACKS) {
            return -1;
        }
        
        size_t index = note_on_callback_count_++;
        note_on_callbacks_[index].note_callback = callback;
        note_on_callbacks_[index].userdata = userdata;
        note_on_callbacks_[index].active = true;
        
        return static_cast<int>(index);
    }
    
    /**
     * @brief Enregistre un callback pour Note Off
     */
    int registerNoteOffCallback(NoteCallback callback, void* userdata = nullptr) {
        if (!callback || note_off_callback_count_ >= PerformanceConfig::MAX_MIDI_CALLBACKS) {
            return -1;
        }
        
        size_t index = note_off_callback_count_++;
        note_off_callbacks_[index].note_callback = callback;
        note_off_callbacks_[index].userdata = userdata;
        note_off_callbacks_[index].active = true;
        
        return static_cast<int>(index);
    }
    
    /**
     * @brief Désactive un callback CC
     */
    bool unregisterCcCallback(int index) {
        if (index < 0 || index >= static_cast<int>(cc_callback_count_)) {
            return false;
        }
        
        cc_callbacks_[index].active = false;
        return true;
    }
    
    /**
     * @brief Désactive un callback Note On
     */
    bool unregisterNoteOnCallback(int index) {
        if (index < 0 || index >= static_cast<int>(note_on_callback_count_)) {
            return false;
        }
        
        note_on_callbacks_[index].active = false;
        return true;
    }
    
    /**
     * @brief Désactive un callback Note Off
     */
    bool unregisterNoteOffCallback(int index) {
        if (index < 0 || index >= static_cast<int>(note_off_callback_count_)) {
            return false;
        }
        
        note_off_callbacks_[index].active = false;
        return true;
    }
    
    // === TRAITEMENT DES MESSAGES ===
    
    /**
     * @brief Ajoute un message MIDI au buffer d'entrée
     * 
     * @param message Message MIDI à traiter
     * @return true si ajouté avec succès, false si buffer plein
     */
    bool enqueueMidiMessage(const MidiBuffers::MidiMessage& message) {
        if (!incoming_buffer_.write(message)) {
            stats_.buffer_overruns.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        return true;
    }
    
    /**
     * @brief Traite tous les messages en attente
     * 
     * Cette méthode doit être appelée régulièrement (idéalement à chaque cycle)
     * pour maintenir une latence minimale.
     * 
     * @return Nombre de messages traités
     */
    uint32_t processIncomingMessages() {
        uint32_t processed_count = 0;
        MidiBuffers::MidiMessage message;
        
        // Traiter tous les messages disponibles
        while (incoming_buffer_.read(message)) {
            uint32_t start_time = 0;
            
            // Mesurer la latence si activé
            if (config_.enable_latency_monitoring) {
                start_time = micros();
            }
            
            // Dispatcher le message selon son type
            processMessage(message);
            processed_count++;
            
            // Calculer et enregistrer la latence
            if (config_.enable_latency_monitoring && start_time > 0) {
                uint32_t latency = micros() - start_time;
                updateLatencyStats(latency);
            }
            
            stats_.messages_processed.fetch_add(1, std::memory_order_relaxed);
            
            // Éviter de monopoliser le CPU
            if (processed_count >= 32) {
                break;
            }
        }
        
        return processed_count;
    }
    
    /**
     * @brief Méthode d'entrée rapide pour messages MIDI depuis ISR
     * 
     * Version optimisée pour appel depuis interruption
     */
    bool enqueueMidiFast(uint8_t status, uint8_t data1, uint8_t data2) {
        MidiBuffers::MidiMessage msg(status, data1, data2, 
                                   config_.enable_timestamping ? micros() : 0);
        return incoming_buffer_.write(msg);
    }
    
    // === STATISTIQUES ET MONITORING ===
    
    /**
     * @brief Obtient les statistiques de performance
     */
    PerformanceStats getStats() const {
        return stats_;
    }
    
    /**
     * @brief Réinitialise les statistiques
     */
    void resetStats() {
        stats_.messages_processed.store(0, std::memory_order_relaxed);
        stats_.messages_dropped.store(0, std::memory_order_relaxed);
        stats_.max_latency_us.store(0, std::memory_order_relaxed);
        stats_.avg_latency_us.store(0, std::memory_order_relaxed);
        stats_.buffer_overruns.store(0, std::memory_order_relaxed);
        stats_.callback_errors.store(0, std::memory_order_relaxed);
    }
    
    /**
     * @brief Obtient l'état des buffers
     */
    struct BufferStatus {
        size_t incoming_size;
        size_t incoming_capacity;
        float incoming_usage;
        bool incoming_full;
    };
    
    BufferStatus getBufferStatus() const {
        return {
            incoming_buffer_.size(),
            incoming_buffer_.capacity(),
            incoming_buffer_.usage_ratio(),
            incoming_buffer_.is_full()
        };
    }
    
    /**
     * @brief Vérifie si le système est en surcharge
     */
    bool isOverloaded() const {
        return incoming_buffer_.usage_ratio() > 0.8f ||
               stats_.max_latency_us.load(std::memory_order_relaxed) > config_.max_latency_threshold_us;
    }

private:
    // Empêcher la copie
    OptimizedMidiProcessor(const OptimizedMidiProcessor&) = delete;
    OptimizedMidiProcessor& operator=(const OptimizedMidiProcessor&) = delete;
    
    /**
     * @brief Traite un message MIDI individuel
     */
    void processMessage(const MidiBuffers::MidiMessage& message) {
        uint8_t status = message.status;
        uint8_t type = status & 0xF0;
        uint8_t channel = status & 0x0F;
        
        try {
            switch (type) {
                case 0xB0: // Control Change
                    dispatchCcCallbacks(channel, message.data1, message.data2);
                    break;
                    
                case 0x90: // Note On
                    if (message.data2 == 0) {
                        // Note On avec vélocité 0 = Note Off
                        dispatchNoteOffCallbacks(channel, message.data1, 0);
                    } else {
                        dispatchNoteOnCallbacks(channel, message.data1, message.data2);
                    }
                    break;
                    
                case 0x80: // Note Off
                    dispatchNoteOffCallbacks(channel, message.data1, message.data2);
                    break;
                    
                default:
                    // Type de message non supporté, ignorer silencieusement
                    break;
            }
        } catch (...) {
            // Capturer toute exception pour éviter de planter le système
            stats_.callback_errors.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    /**
     * @brief Dispatch les callbacks CC
     */
    void dispatchCcCallbacks(MidiChannel channel, MidiCC cc, uint8_t value) {
        for (size_t i = 0; i < cc_callback_count_; ++i) {
            if (cc_callbacks_[i].active && cc_callbacks_[i].cc_callback) {
                cc_callbacks_[i].cc_callback(channel, cc, value, cc_callbacks_[i].userdata);
            }
        }
    }
    
    /**
     * @brief Dispatch les callbacks Note On
     */
    void dispatchNoteOnCallbacks(MidiChannel channel, MidiNote note, uint8_t velocity) {
        for (size_t i = 0; i < note_on_callback_count_; ++i) {
            if (note_on_callbacks_[i].active && note_on_callbacks_[i].note_callback) {
                note_on_callbacks_[i].note_callback(channel, note, velocity, note_on_callbacks_[i].userdata);
            }
        }
    }
    
    /**
     * @brief Dispatch les callbacks Note Off
     */
    void dispatchNoteOffCallbacks(MidiChannel channel, MidiNote note, uint8_t velocity) {
        for (size_t i = 0; i < note_off_callback_count_; ++i) {
            if (note_off_callbacks_[i].active && note_off_callbacks_[i].note_callback) {
                note_off_callbacks_[i].note_callback(channel, note, velocity, note_off_callbacks_[i].userdata);
            }
        }
    }
    
    /**
     * @brief Met à jour les statistiques de latence
     */
    void updateLatencyStats(uint32_t latency_us) {
        // Mettre à jour la latence maximale
        uint32_t current_max = stats_.max_latency_us.load(std::memory_order_relaxed);
        while (latency_us > current_max && 
               !stats_.max_latency_us.compare_exchange_weak(current_max, latency_us, 
                                                           std::memory_order_relaxed)) {
            // Retry si la valeur a changé entre temps
        }
        
        // Calcul simple de la moyenne mobile (approximation)
        uint32_t current_avg = stats_.avg_latency_us.load(std::memory_order_relaxed);
        uint32_t new_avg = (current_avg * 7 + latency_us) / 8; // Moyenne mobile avec facteur 0.875
        stats_.avg_latency_us.store(new_avg, std::memory_order_relaxed);
    }
    
    // === DONNÉES MEMBRES ===
    
    Config config_;
    
    // Buffers ring pour messages entrants
    MidiBuffers::IncomingMidiBuffer incoming_buffer_;
    
    // Callbacks statiques (pas d'allocation dynamique)
    std::array<CallbackEntry, PerformanceConfig::MAX_MIDI_CALLBACKS> cc_callbacks_;
    std::array<CallbackEntry, PerformanceConfig::MAX_MIDI_CALLBACKS> note_on_callbacks_;
    std::array<CallbackEntry, PerformanceConfig::MAX_MIDI_CALLBACKS> note_off_callbacks_;
    
    // Compteurs de callbacks actifs
    std::atomic<size_t> cc_callback_count_;
    std::atomic<size_t> note_on_callback_count_;
    std::atomic<size_t> note_off_callback_count_;
    
    // Statistiques de performance
    mutable PerformanceStats stats_;
};