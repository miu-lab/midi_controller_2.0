#pragma once

#include <memory>

#include "core/domain/navigation/NavigationStateManager.hpp"
#include "core/domain/navigation/NavigationEvent.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/core/IEventBus.hpp"
#include "NavigationHandlerManager.hpp"

/**
 * @brief Contrôleur central pour la navigation
 * 
 * Ce contrôleur gère tous les événements de navigation et orchestre
 * les changements d'état de l'application via NavigationStateManager.
 */
class NavigationController : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param stateManager Gestionnaire d'état de navigation
     * @param eventBus Bus d'événements pour les abonnements
     */
    NavigationController(std::shared_ptr<NavigationStateManager> stateManager,
                        std::shared_ptr<EventBus> eventBus);
    
    /**
     * @brief Destructeur
     */
    ~NavigationController();
    
    // === INITIALISATION ===
    
    /**
     * @brief Initialise le contrôleur
     */
    void initialize();
    
    /**
     * @brief S'abonne aux événements de navigation
     */
    void subscribeToEvents();
    
    // === GESTION DES ÉVÉNEMENTS ===
    
    /**
     * @brief Traite un événement de navigation générique
     */
    void handleNavigationEvent(const NavigationEvent& event);
    
    /**
     * @brief Traite un événement de changement d'état
     */
    void handleStateChangeEvent(const StateChangeEvent& event);
    
    /**
     * @brief Traite un événement "retour"
     */
    void handleBackRequestedEvent(const BackRequestedEvent& event);
    
    /**
     * @brief Traite un événement "accueil"
     */
    void handleHomeRequestedEvent(const HomeRequestedEvent& event);
    
    // === ROUTAGE PUBLIC ===
    
    /**
     * @brief Route une action de navigation
     * @param action Action à exécuter
     * @param parameter Paramètre optionnel
     */
    void routeNavigationAction(NavigationAction action, int parameter = 0);
    
    /**
     * @brief Force un changement d'état
     * @param newState Nouvel état
     * @param parameter Paramètre optionnel
     * @param subState Sous-état optionnel
     */
    void forceStateChange(AppState newState, uint8_t parameter = 0, uint8_t subState = 0);
    
    // === ÉTAT ET DIAGNOSTICS ===
    
    /**
     * @brief Obtient l'état actuel
     */
    AppState getCurrentState() const;
    
    /**
     * @brief Vérifie si la navigation peut revenir en arrière
     */
    bool canGoBack() const;
    
    /**
     * @brief Obtient la taille de l'historique de navigation
     */
    size_t getNavigationHistorySize() const;
    
    // === INTERFACE EVENTLISTENER ===
    
    /**
     * @brief Traite un événement reçu
     */
    bool onEvent(const Event& event) override;

private:
    // === DÉPENDANCES ===
    std::shared_ptr<NavigationStateManager> stateManager_;
    std::shared_ptr<EventBus> eventBus_;
    
    // === SYSTÈME DE HANDLERS ===
    std::unique_ptr<NavigationHandlerManager> handlerManager_;
    
    // === ÉTAT D'INITIALISATION ===
    bool initialized_;
    
    // === MÉTHODES PRIVÉES ===
    
    /**
     * @brief Vérifie si l'action est valide dans le contexte actuel
     */
    bool isActionValidInCurrentContext(NavigationAction action) const;
};