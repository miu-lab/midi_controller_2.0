#pragma once

#include "IEventBus.hpp"
#include <atomic>
#include <algorithm>
#include <memory>
#include <mutex>
#include <Arduino.h>
#include "config/PerformanceConfig.hpp"
#include "config/ETLConfig.hpp"
#include "../MidiEvents.hpp"
#include "../UIEvent.hpp"

/**
 * @brief Implémentation du bus d'événements unifié
 * 
 * Combine EventBus et OptimizedEventBus en une seule classe moderne
 * qui implémente l'interface IEventBus pour l'injection de dépendance.
 */
class EventBus : public MidiController::Events::IEventBus, public EventListener {
public:
    /**
     * @brief Configuration unifiée pour EventBus (inclut batching)
     */
    struct Config {
        // Configuration du batching
        unsigned long ui_update_interval_ms;     ///< 60 FPS pour UI
        unsigned long status_update_interval_ms; ///< 10 FPS pour status
        bool coalesce_identical_values;          ///< Fusionner valeurs identiques
        bool enable_batching;                    ///< Activer le batching
        
        Config() 
            : ui_update_interval_ms(PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS)
            , status_update_interval_ms(100)
            , coalesce_identical_values(true)
            , enable_batching(true) {}
    };
    
    /**
     * @brief Constructeur avec configuration
     * @param config Configuration pour EventBus
     */
    explicit EventBus(const Config& config = Config()) 
        : config_(config)
        , nextId_(1)
        , initialized_(false)
        , started_(false)
        , processedEventCount_(0) {
        // ETL containers ont une taille fixe, pas besoin de reserve()
        
        // Initialiser les compteurs
        for (auto& counter : eventCounters_) {
            counter.store(0, std::memory_order_relaxed);
        }
    }
    
    /**
     * @brief Destructeur
     */
    ~EventBus() override = default;
    
    /**
     * @brief S'abonne au bus d'événements avec priorité
     * @param listener Écouteur d'événements
     * @param priority Niveau de priorité
     * @return Identifiant d'abonnement, 0 si échec
     */
    SubscriptionId subscribe(EventListener* listener, EventPriority priority = EventPriority::PRIORITY_NORMAL) override {
        if (!listener) {
            return 0;
        }
        
        Subscription sub;
        sub.listener = listener;
        sub.id = nextId_++;
        sub.priority = priority;
        sub.active = true;
        
        subscriptions_.push_back(sub);
        
        // Trier par priorité pour optimiser la publication
        sortByPriority();
        
        return sub.id;
    }
    
    /**
     * @brief API de compatibilité avec l'ancien EventBus
     * @param listener Écouteur d'événements
     * @param priority Priorité numérique (0-255)
     * @return Identifiant d'abonnement
     */
    SubscriptionId subscribe(EventListener* listener, uint8_t priority) override {
        EventPriority eventPriority = EventPriority::PRIORITY_NORMAL;
        if (priority >= 200) {
            eventPriority = EventPriority::PRIORITY_HIGH;
        } else if (priority < 50) {
            eventPriority = EventPriority::PRIORITY_LOW;
        }
        
        return subscribe(listener, eventPriority);
    }
    
    /**
     * @brief Méthodes utilitaires pour un usage simplifié
     */
    SubscriptionId subscribeHigh(EventListener* listener) override {
        return subscribe(listener, EventPriority::PRIORITY_HIGH);
    }
    
    SubscriptionId subscribeNormal(EventListener* listener) override {
        return subscribe(listener, EventPriority::PRIORITY_NORMAL);
    }
    
    SubscriptionId subscribeLow(EventListener* listener) override {
        return subscribe(listener, EventPriority::PRIORITY_LOW);
    }
    
    /**
     * @brief Se désabonne du bus d'événements
     * @param id Identifiant d'abonnement
     * @return true si désabonnement réussi, false sinon
     */
    bool unsubscribe(SubscriptionId id) override {
        auto it = std::find_if(subscriptions_.begin(), subscriptions_.end(),
                              [id](const Subscription& sub) { return sub.id == id; });
        
        if (it != subscriptions_.end()) {
            subscriptions_.erase(it);
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief Met en pause un abonnement
     * @param id Identifiant d'abonnement
     * @return true si mise en pause réussie, false sinon
     */
    bool pause(SubscriptionId id) override {
        return setSubscriptionActive(id, false);
    }
    
    /**
     * @brief Reprend un abonnement mis en pause
     * @param id Identifiant d'abonnement
     * @return true si reprise réussie, false sinon
     */
    bool resume(SubscriptionId id) override {
        return setSubscriptionActive(id, true);
    }
    
    /**
     * @brief Publie un événement à tous les abonnés actifs
     * @param event Événement à publier
     * @return true si au moins un abonné a traité l'événement, false sinon
     */
    bool publish(Event& event) override {
        bool handled = false;
        
        // Traiter tous les abonnements (déjà triés par priorité)
        for (auto& subscription : subscriptions_) {
            if (subscription.active && subscription.listener) {
                if (subscription.listener->onEvent(event)) {
                    handled = true;
                    event.setHandled();
                }
                
                // Arrêter la propagation si demandé
                if (!event.shouldPropagate()) {
                    break;
                }
            }
        }
        
        // Incrémenter les compteurs de performance pour les événements haute priorité
        if (event.getType() >= EventTypes::HighPriorityEncoderChanged && 
            event.getType() <= EventTypes::HighPriorityButtonPress) {
            uint8_t index = event.getType() - EventTypes::HighPriorityEncoderChanged;
            if (index < eventCounters_.size()) {
                eventCounters_[index].fetch_add(1, std::memory_order_relaxed);
            }
        }
        
        return handled;
    }
    
    /**
     * @brief Surcharge pour pointeur
     */
    bool publish(Event* event) override {
        if (event) {
            return publish(*event);
        }
        return false;
    }
    
    /**
     * @brief Supprime tous les abonnements
     */
    void clear() override {
        subscriptions_.clear();
    }
    
    /**
     * @brief Vérifie si un abonnement existe
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement existe, false sinon
     */
    bool exists(SubscriptionId id) const override {
        return std::any_of(subscriptions_.begin(), subscriptions_.end(),
                          [id](const Subscription& sub) { return sub.id == id; });
    }
    
    /**
     * @brief Vérifie si un abonnement est actif
     * @param id Identifiant d'abonnement
     * @return true si l'abonnement est actif, false sinon
     */
    bool isActive(SubscriptionId id) const override {
        auto it = std::find_if(subscriptions_.begin(), subscriptions_.end(),
                              [id](const Subscription& sub) { return sub.id == id; });
        
        return (it != subscriptions_.end()) ? it->active : false;
    }
    
    /**
     * @brief Obtient le nombre d'abonnements
     * @return Nombre d'abonnements
     */
    int getCount() const override {
        return subscriptions_.size();
    }
    
    /**
     * @brief Obtient la capacité actuelle du vecteur
     * @return Capacité du vecteur
     */
    int getCapacity() const override {
        return subscriptions_.capacity();
    }
    
    /**
     * @brief Obtient les statistiques de traitement d'événements
     * @param eventType Type d'événement haute priorité
     * @return Nombre d'événements traités de ce type
     */
    uint32_t getEventProcessingCount(EventType eventType) const override {
        if (eventType >= EventTypes::HighPriorityEncoderChanged && 
            eventType <= EventTypes::HighPriorityButtonPress) {
            uint8_t index = eventType - EventTypes::HighPriorityEncoderChanged;
            if (index < eventCounters_.size()) {
                return eventCounters_[index].load(std::memory_order_relaxed);
            }
        }
        return 0;
    }
    
    /**
     * @brief Réinitialise les compteurs de traitement d'événements
     */
    void resetEventProcessingCounters() override {
        for (auto& counter : eventCounters_) {
            counter.store(0, std::memory_order_relaxed);
        }
    }
    
    // === Nouvelles méthodes de gestion du cycle de vie (consolidées depuis EventManager) ===
    
    /**
     * @brief Initialise le bus d'événements
     * @return true si l'initialisation a réussi
     */
    bool initialize() override {
        if (initialized_) {
            return true;
        }
        
        // Initialiser le batching si activé
        if (config_.enable_batching) {
            initializeBatching();
        }
        
        initialized_ = true;
        return true;
    }
    
    /**
     * @brief Démarre le bus d'événements (active le batching si configuré)
     */
    void start() override {
        if (!initialized_) {
            initialize();
        }
        
        if (started_) {
            return;
        }
        
        // Démarrer le batching si activé
        if (config_.enable_batching) {
            startBatching();
        }
        
        started_ = true;
    }
    
    /**
     * @brief Arrête le bus d'événements
     */
    void stop() override {
        if (!started_) {
            return;
        }
        
        // Arrêter le batching
        if (config_.enable_batching) {
            stopBatching();
        }
        
        started_ = false;
    }
    
    /**
     * @brief Met à jour le bus (traite les batchs en attente)
     */
    void update() override {
        if (!started_) {
            return;
        }
        
        // Traiter les batchs en attente
        if (config_.enable_batching) {
            processPendingBatches();
        }
        
        processedEventCount_++;
    }
    
    /**
     * @brief Vérifie si le bus est démarré
     * @return true si démarré
     */
    bool isStarted() const override {
        return started_;
    }
    
    /**
     * @brief Obtient les statistiques du gestionnaire
     * @return Nombre d'événements traités
     */
    size_t getProcessedEventCount() const override {
        return processedEventCount_;
    }
    
    // === Interface EventListener (pour le batching interne) ===
    
    /**
     * @brief Traite les événements pour le batching (interface EventListener)
     * @param event Événement reçu
     * @return true si l'événement a été traité
     */
    bool onEvent(const Event& event) override {
        if (!started_ || !config_.enable_batching) {
            return false;
        }
        
        // Traiter seulement les événements MIDI CC pour le batching
        if (event.getType() == EventTypes::MidiControlChange) {
            const auto& midi_event = static_cast<const MidiCCEvent&>(event);
            handleMidiCCEventForBatching(midi_event);
            return true; // Événement consommé pour le batching
        }
        
        return false;
    }

private:
    /**
     * @brief Structure compacte pour stocker les informations d'abonnement
     */
    struct Subscription {
        EventListener* listener;
        SubscriptionId id;
        EventPriority priority;
        bool active;
    };
    
    
    // Empêcher la copie
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    /**
     * @brief Trie les abonnements par priorité
     */
    void sortByPriority() {
        std::sort(subscriptions_.begin(), subscriptions_.end(),
                 [](const Subscription& a, const Subscription& b) {
                     return static_cast<uint8_t>(a.priority) < static_cast<uint8_t>(b.priority);
                 });
    }
    
    /**
     * @brief Active/désactive un abonnement
     */
    bool setSubscriptionActive(SubscriptionId id, bool active) {
        auto it = std::find_if(subscriptions_.begin(), subscriptions_.end(),
                              [id](const Subscription& sub) { return sub.id == id; });
        
        if (it != subscriptions_.end()) {
            it->active = active;
            return true;
        }
        
        return false;
    }
    
    // === Variables de base ===
    Config config_;
    ETLConfig::EventSubscriptionVector<Subscription> subscriptions_;
    SubscriptionId nextId_;
    
    // === Variables de cycle de vie ===
    bool initialized_;
    bool started_;
    size_t processedEventCount_;
    
    // Compteurs atomiques pour le suivi des événements traités (diagnostics)
    std::array<std::atomic<uint32_t>, 3> eventCounters_; // Un compteur par type d'événement haute priorité
    
    // === Variables de batching (intégrées depuis EventBatcher) ===
    struct PendingParameter {
        uint8_t controller;
        uint8_t channel;
        uint8_t value;
        String name;
        unsigned long last_update_ms;
        bool needs_ui_update = false;
    };
    
    ETLConfig::MidiPendingMap<uint16_t, PendingParameter> pending_parameters_; // Key: (channel << 8) | controller
    unsigned long last_ui_batch_ms_ = 0;
    unsigned long last_status_batch_ms_ = 0;
    SubscriptionId batching_subscription_id_ = 0;
    
    // === Méthodes de batching (intégrées depuis EventBatcher) ===
    
    /**
     * @brief Initialise le système de batching
     */
    void initializeBatching() {
        // Rien de spécifique à initialiser pour le moment
    }
    
    /**
     * @brief Démarre le batching (s'abonne aux événements MIDI)
     */
    void startBatching() {
        if (batching_subscription_id_ == 0) {
            // S'abonner aux événements avec haute priorité pour traiter avant l'UI
            batching_subscription_id_ = subscribe(this, EventPriority::PRIORITY_HIGH);
        }
    }
    
    /**
     * @brief Arrête le batching
     */
    void stopBatching() {
        if (batching_subscription_id_ > 0) {
            unsubscribe(batching_subscription_id_);
            batching_subscription_id_ = 0;
        }
        
        // Vider les batchs en attente
        flushUIBatch();
    }
    
    /**
     * @brief Traite les batchs en attente (appelé périodiquement)
     */
    void processPendingBatches() {
        unsigned long now = millis();
        
        // Traiter le batch UI si nécessaire
        if ((now - last_ui_batch_ms_) >= config_.ui_update_interval_ms) {
            flushUIBatch();
            last_ui_batch_ms_ = now;
        }
        
        // Traiter le batch status si nécessaire
        if ((now - last_status_batch_ms_) >= config_.status_update_interval_ms) {
            flushStatusBatch();
            last_status_batch_ms_ = now;
        }
    }
    
    /**
     * @brief Traite un événement MIDI CC pour le batching
     */
    void handleMidiCCEventForBatching(const MidiCCEvent& midi_event) {
        uint16_t key = getParameterKey(midi_event.controller, midi_event.channel);
        unsigned long now = millis();
        
        // Chercher ou créer l'entrée
        auto it = pending_parameters_.find(key);
        if (it == pending_parameters_.end()) {
            // Nouveau paramètre
            PendingParameter param;
            param.controller = midi_event.controller;
            param.channel = midi_event.channel;
            param.value = midi_event.value;
            param.name = "CC" + String(midi_event.controller);
            param.last_update_ms = now;
            param.needs_ui_update = true;
            
            pending_parameters_[key] = param;
            
        } else {
            // Paramètre existant - mise à jour
            PendingParameter& param = it->second;
            
            // Vérifier si la valeur a changé
            if (config_.coalesce_identical_values && param.value == midi_event.value) {
                return; // Valeur identique, ignorer
            }
            
            param.value = midi_event.value;
            param.last_update_ms = now;
            param.needs_ui_update = true;
        }
    }
    
    /**
     * @brief Génère une clé unique pour un paramètre
     */
    uint16_t getParameterKey(uint8_t controller, uint8_t channel) const {
        return (static_cast<uint16_t>(channel) << 8) | controller;
    }
    
    /**
     * @brief Envoie les événements UI batchés
     */
    void flushUIBatch() {
        for (auto& [key, param] : pending_parameters_) {
            if (param.needs_ui_update) {
                // Créer et envoyer l'événement UI optimisé
                UIParameterUpdateEvent ui_event(
                    param.controller,
                    param.channel,
                    param.value,
                    param.name
                );
                
                // Publier directement via la méthode de base (pas de récursion)
                publishDirect(ui_event);
                param.needs_ui_update = false;
            }
        }
    }
    
    /**
     * @brief Envoie les événements de status batchés
     */
    void flushStatusBatch() {
        // Pour l'instant, pas d'événements de status à traiter
        // Peut être étendu plus tard pour des mises à jour de status général
    }
    
    /**
     * @brief Publication directe sans batching (pour éviter la récursion)
     */
    bool publishDirect(Event& event) {
        bool handled = false;
        
        // Traiter tous les abonnements (déjà triés par priorité)
        // Mais exclure l'abonnement du batching pour éviter la récursion
        for (auto& subscription : subscriptions_) {
            if (subscription.active && subscription.listener && 
                subscription.id != batching_subscription_id_) {
                if (subscription.listener->onEvent(event)) {
                    handled = true;
                    event.setHandled();
                }
                
                // Arrêter la propagation si demandé
                if (!event.shouldPropagate()) {
                    break;
                }
            }
        }
        
        return handled;
    }
};