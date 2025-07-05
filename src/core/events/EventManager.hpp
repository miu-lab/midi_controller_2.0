#pragma once

#include <memory>
#include <map>
#include <vector>
#include <functional>
#include "core/domain/events/core/Event.hpp"
#include "core/domain/events/core/IEventBus.hpp"
#include "core/domain/events/EventBatcher.hpp"

/**
 * @brief Gestionnaire d'événements unifié combinant EventBus et EventBatcher
 * 
 * Fournit une API unifiée pour la gestion des événements avec:
 * - Publication et souscription d'événements (EventBus)
 * - Batching et throttling pour les performances (EventBatcher)
 * - Routage typé des événements
 */
class EventManager {
public:
    /**
     * @brief Configuration pour le gestionnaire d'événements
     */
    struct Config {
        unsigned long uiUpdateIntervalMs;     // 60 FPS pour UI
        unsigned long statusUpdateIntervalMs; // 10 FPS pour status
        bool coalesceIdenticalValues;       // Fusionner valeurs identiques
        bool enableBatching;                // Activer le batching
        
        Config() : uiUpdateIntervalMs(16), statusUpdateIntervalMs(100), 
                   coalesceIdenticalValues(true), enableBatching(true) {}
    };

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration pour EventManager
     * @param eventBus Bus d'événements injecté (optionnel)
     */
    explicit EventManager(const Config& config = Config{}, std::shared_ptr<MidiController::Events::IEventBus> eventBus = nullptr);
    
    /**
     * @brief Destructeur
     */
    ~EventManager();

    /**
     * @brief Initialise le gestionnaire d'événements
     * @return true si l'initialisation a réussi
     */
    bool initialize();

    /**
     * @brief Démarre le gestionnaire d'événements
     */
    void start();

    /**
     * @brief Arrête le gestionnaire d'événements
     */
    void stop();

    /**
     * @brief Met à jour le gestionnaire (traite les batchs en attente)
     */
    void update();

    /**
     * @brief Publie un événement
     * @param event Événement à publier
     */
    void publishEvent(const Event& event);

    /**
     * @brief S'abonne à un type d'événement
     * @param eventType Type d'événement
     * @param listener Écouteur d'événements
     * @param priority Priorité de l'écouteur
     * @return ID de l'abonnement
     */
    SubscriptionId subscribe(EventType eventType, 
                           std::shared_ptr<EventListener> listener,
                           EventPriority priority = EventPriority::PRIORITY_NORMAL);

    /**
     * @brief Se désabonne d'un événement
     * @param subscriptionId ID de l'abonnement à annuler
     */
    void unsubscribe(SubscriptionId subscriptionId);

    /**
     * @brief Obtient une référence à l'EventBus sous-jacent
     * @return Référence à IEventBus
     */
    MidiController::Events::IEventBus* getEventBus();

    /**
     * @brief Vérifie si le gestionnaire est démarré
     * @return true si démarré
     */
    bool isStarted() const;

    /**
     * @brief Obtient les statistiques du gestionnaire
     * @return Nombre d'événements traités
     */
    size_t getProcessedEventCount() const;

private:
    Config config_;
    std::unique_ptr<EventBatcher> eventBatcher_;
    std::shared_ptr<MidiController::Events::IEventBus> eventBus_;
    bool initialized_;
    bool started_;
    size_t processedEventCount_;

    /**
     * @brief Configure l'EventBatcher avec la configuration
     */
    void configureEventBatcher();
};