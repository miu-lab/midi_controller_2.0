#pragma once

#include <memory>
#include <etl/array.h>

#include "core/domain/navigation/NavigationStateManager.hpp"
#include "core/domain/navigation/NavigationEvent.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/core/IEventBus.hpp"

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
    
    // === ÉTAT D'INITIALISATION ===
    bool initialized_;
    
    // === TABLES DE ROUTAGE ===
    
    /**
     * @brief Structure pour le routage d'actions
     */
    struct ActionRoute {
        NavigationAction action;
        AppState targetState;
        bool pushToHistory;      // true = pushState(), false = setState()
        bool requiresParameter;
        
        ActionRoute(NavigationAction act, AppState target, bool push = false, bool reqParam = false)
            : action(act), targetState(target), pushToHistory(push), requiresParameter(reqParam) {}
    };
    
    // Table de routage statique
    static const etl::array<ActionRoute, 11> ACTION_ROUTES;
    
    // === MÉTHODES PRIVÉES ===
    
    /**
     * @brief Exécute une action contextuelle
     */
    void executeContextualAction(NavigationAction action, int parameter);
    
    /**
     * @brief Détermine l'état cible selon l'action et le contexte
     */
    AppState determineTargetState(NavigationAction action, AppState currentState) const;
    
    /**
     * @brief Trouve la route pour une action donnée
     */
    const ActionRoute* findActionRoute(NavigationAction action) const;
    
    /**
     * @brief Vérifie si l'action est valide dans le contexte actuel
     */
    bool isActionValidInCurrentContext(NavigationAction action) const;
    
    /**
     * @brief Traite les actions spéciales (HOME, BACK)
     */
    void handleSpecialAction(NavigationAction action, int parameter);
    
    /**
     * @brief Traite les actions de navigation normale
     */
    void handleNormalAction(NavigationAction action, int parameter);
    
    // === CALLBACKS D'ÉVÉNEMENTS ===
    
    /**
     * @brief Callback générique pour tous les événements de navigation
     */
    void onNavigationEventReceived(const Event& event);
};