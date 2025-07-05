#pragma once

#include "ObjectPool.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/UIEvent.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include <memory>

/**
 * @brief Gestionnaire centralisé des pools d'événements
 * 
 * Cette classe gère tous les pools d'objets pour les différents types d'événements,
 * permettant une allocation/libération efficace sans fragmentation mémoire.
 */
class EventPoolManager {
public:
    /**
     * @brief Constructeur par défaut
     */
    EventPoolManager() = default;
    
    /**
     * @brief Destructeur par défaut
     */
    ~EventPoolManager() = default;
    
    // === POOLS D'ÉVÉNEMENTS MIDI ===
    
    /**
     * @brief Acquiert un événement MIDI CC depuis le pool
     * 
     * @param controller Numéro du contrôleur
     * @param channel Canal MIDI
     * @param value Valeur du contrôleur
     * @return Pointeur vers l'événement alloué, ou nullptr si pool plein
     */
    MidiCCEvent* acquireMidiCCEvent(uint8_t controller, uint8_t channel, uint8_t value) {
        return midi_cc_pool_.acquire(controller, channel, value);
    }
    
    /**
     * @brief Acquiert un événement MIDI Note On depuis le pool
     */
    MidiNoteOnEvent* acquireMidiNoteOnEvent(uint8_t note, uint8_t channel, uint8_t velocity) {
        return midi_note_on_pool_.acquire(note, channel, velocity);
    }
    
    /**
     * @brief Acquiert un événement MIDI Note Off depuis le pool
     */
    MidiNoteOffEvent* acquireMidiNoteOffEvent(uint8_t note, uint8_t channel, uint8_t velocity) {
        return midi_note_off_pool_.acquire(note, channel, velocity);
    }
    
    /**
     * @brief Libère un événement MIDI CC
     */
    bool releaseMidiCCEvent(MidiCCEvent* event) {
        return midi_cc_pool_.release(event);
    }
    
    /**
     * @brief Libère un événement MIDI Note On
     */
    bool releaseMidiNoteOnEvent(MidiNoteOnEvent* event) {
        return midi_note_on_pool_.release(event);
    }
    
    /**
     * @brief Libère un événement MIDI Note Off
     */
    bool releaseMidiNoteOffEvent(MidiNoteOffEvent* event) {
        return midi_note_off_pool_.release(event);
    }
    
    // === POOLS D'ÉVÉNEMENTS UI ===
    
    /**
     * @brief Acquiert un événement de mise à jour de paramètre UI
     */
    UIParameterUpdateEvent* acquireUIParameterUpdateEvent(
        uint8_t controller, 
        uint8_t channel, 
        uint8_t value, 
        const String& name) {
        return ui_parameter_pool_.acquire(controller, channel, value, name);
    }
    
    /**
     * @brief Libère un événement de mise à jour de paramètre UI
     */
    bool releaseUIParameterUpdateEvent(UIParameterUpdateEvent* event) {
        return ui_parameter_pool_.release(event);
    }
    
    // === FACTORY METHODS AVEC RAII GUARDS ===
    
    /**
     * @brief Crée un événement MIDI CC avec guard RAII
     */
    Pools::MidiEventGuard<MidiCCEvent> createMidiCCEvent(
        uint8_t controller, uint8_t channel, uint8_t value) {
        auto* event = midi_cc_pool_.acquire(controller, channel, value);
        return Pools::MidiEventGuard<MidiCCEvent>(midi_cc_pool_, event);
    }
    
    /**
     * @brief Crée un événement MIDI Note On avec guard RAII
     */
    Pools::MidiEventGuard<MidiNoteOnEvent> createMidiNoteOnEvent(
        uint8_t note, uint8_t channel, uint8_t velocity) {
        auto* event = midi_note_on_pool_.acquire(note, channel, velocity);
        return Pools::MidiEventGuard<MidiNoteOnEvent>(midi_note_on_pool_, event);
    }
    
    /**
     * @brief Crée un événement UI avec guard RAII
     */
    Pools::UIEventGuard<UIParameterUpdateEvent> createUIParameterUpdateEvent(
        uint8_t controller, uint8_t channel, uint8_t value, const String& name) {
        auto* event = ui_parameter_pool_.acquire(controller, channel, value, name);
        return Pools::UIEventGuard<UIParameterUpdateEvent>(ui_parameter_pool_, event);
    }
    
    // === STATISTIQUES ET MONITORING ===
    
    /**
     * @brief Structure pour les statistiques d'un pool
     */
    struct PoolStats {
        size_t capacity;
        size_t allocated;
        size_t available;
        float usage_ratio;
        bool is_full;
    };
    
    /**
     * @brief Obtient les statistiques du pool MIDI CC
     */
    PoolStats getMidiCCPoolStats() const {
        return {
            midi_cc_pool_.capacity(),
            midi_cc_pool_.allocated_count(),
            midi_cc_pool_.available_count(),
            midi_cc_pool_.usage_ratio(),
            midi_cc_pool_.is_full()
        };
    }
    
    /**
     * @brief Obtient les statistiques du pool MIDI Note On
     */
    PoolStats getMidiNoteOnPoolStats() const {
        return {
            midi_note_on_pool_.capacity(),
            midi_note_on_pool_.allocated_count(),
            midi_note_on_pool_.available_count(),
            midi_note_on_pool_.usage_ratio(),
            midi_note_on_pool_.is_full()
        };
    }
    
    /**
     * @brief Obtient les statistiques du pool UI
     */
    PoolStats getUIParameterPoolStats() const {
        return {
            ui_parameter_pool_.capacity(),
            ui_parameter_pool_.allocated_count(),
            ui_parameter_pool_.available_count(),
            ui_parameter_pool_.usage_ratio(),
            ui_parameter_pool_.is_full()
        };
    }
    
    /**
     * @brief Obtient les statistiques globales de tous les pools
     */
    struct GlobalStats {
        size_t total_capacity;
        size_t total_allocated;
        size_t total_available;
        float global_usage_ratio;
        bool any_pool_full;
    };
    
    /**
     * @brief Calcule les statistiques globales
     */
    GlobalStats getGlobalStats() const {
        size_t total_cap = midi_cc_pool_.capacity() + 
                          midi_note_on_pool_.capacity() + 
                          midi_note_off_pool_.capacity() + 
                          ui_parameter_pool_.capacity();
        
        size_t total_alloc = midi_cc_pool_.allocated_count() + 
                            midi_note_on_pool_.allocated_count() + 
                            midi_note_off_pool_.allocated_count() + 
                            ui_parameter_pool_.allocated_count();
        
        bool any_full = midi_cc_pool_.is_full() || 
                       midi_note_on_pool_.is_full() || 
                       midi_note_off_pool_.is_full() || 
                       ui_parameter_pool_.is_full();
        
        return {
            total_cap,
            total_alloc,
            total_cap - total_alloc,
            static_cast<float>(total_alloc) / static_cast<float>(total_cap),
            any_full
        };
    }
    
    /**
     * @brief Vérifie si un pool approche de la saturation (>90% utilisé)
     */
    bool hasHighMemoryPressure() const {
        return midi_cc_pool_.usage_ratio() > 0.9f ||
               midi_note_on_pool_.usage_ratio() > 0.9f ||
               midi_note_off_pool_.usage_ratio() > 0.9f ||
               ui_parameter_pool_.usage_ratio() > 0.9f;
    }

private:
    // Empêcher la copie
    EventPoolManager(const EventPoolManager&) = delete;
    EventPoolManager& operator=(const EventPoolManager&) = delete;
    
    // === POOLS SPÉCIALISÉS ===
    
    // Pools MIDI (haute fréquence)
    Pools::MidiEventPool<MidiCCEvent> midi_cc_pool_;
    Pools::MidiEventPool<MidiNoteOnEvent> midi_note_on_pool_;
    Pools::MidiEventPool<MidiNoteOffEvent> midi_note_off_pool_;
    
    // Pool UI (fréquence modérée)
    Pools::UIEventPool<UIParameterUpdateEvent> ui_parameter_pool_;
};

/**
 * @brief Factory globale pour créer des événements avec pools
 * 
 * Cette classe provide un accès statique au EventPoolManager
 * pour une utilisation simplifiée dans tout le système.
 */
class EventFactory {
public:
    /**
     * @brief Initialise la factory avec un gestionnaire de pools
     */
    static void initialize(std::shared_ptr<EventPoolManager> pool_manager) {
        pool_manager_ = pool_manager;
    }
    
    /**
     * @brief Obtient le gestionnaire de pools
     */
    static std::shared_ptr<EventPoolManager> getPoolManager() {
        return pool_manager_;
    }
    
    /**
     * @brief Crée un événement MIDI CC avec pool
     */
    static MidiCCEvent* createMidiCCEvent(uint8_t controller, uint8_t channel, uint8_t value) {
        if (pool_manager_) {
            return pool_manager_->acquireMidiCCEvent(controller, channel, value);
        }
        return new MidiCCEvent(controller, channel, value); // Fallback sur allocation classique
    }
    
    /**
     * @brief Libère un événement MIDI CC
     */
    static void releaseMidiCCEvent(MidiCCEvent* event) {
        if (pool_manager_ && !pool_manager_->releaseMidiCCEvent(event)) {
            delete event; // Fallback pour événements non-poolés
        }
    }
    
    /**
     * @brief Crée un événement UI avec pool
     */
    static UIParameterUpdateEvent* createUIParameterUpdateEvent(
        uint8_t controller, uint8_t channel, uint8_t value, const String& name) {
        if (pool_manager_) {
            return pool_manager_->acquireUIParameterUpdateEvent(controller, channel, value, name);
        }
        return new UIParameterUpdateEvent(controller, channel, value, name);
    }
    
    /**
     * @brief Libère un événement UI
     */
    static void releaseUIParameterUpdateEvent(UIParameterUpdateEvent* event) {
        if (pool_manager_ && !pool_manager_->releaseUIParameterUpdateEvent(event)) {
            delete event;
        }
    }

private:
    static std::shared_ptr<EventPoolManager> pool_manager_;
};