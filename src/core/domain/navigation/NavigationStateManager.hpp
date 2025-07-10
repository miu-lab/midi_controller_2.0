#pragma once

#include <etl/stack.h>
#include <memory>

#include "AppState.hpp"
#include "NavigationAction.hpp"
#include "core/domain/interfaces/IViewManager.hpp"

/**
 * @brief Gestionnaire d'état de navigation avec historique
 * 
 * Gère l'état actuel de l'application et maintient un historique
 * pour la navigation contextuelle (bouton "back").
 */
class NavigationStateManager {
public:
    /**
     * @brief Constructeur
     * @param viewManager Gestionnaire de vues pour l'affichage
     */
    explicit NavigationStateManager(IViewManager& viewManager);
    
    /**
     * @brief Destructeur
     */
    ~NavigationStateManager() = default;
    
    // === GESTION D'ÉTAT PRINCIPAL ===
    
    /**
     * @brief Obtient l'état actuel
     */
    AppState getCurrentState() const;
    
    /**
     * @brief Obtient le contexte actuel complet
     */
    const AppStateContext& getCurrentContext() const;
    
    /**
     * @brief Définit un nouvel état
     */
    void setState(AppState newState, uint8_t parameter = 0, uint8_t subState = 0);
    
    /**
     * @brief Définit un nouvel état avec contexte complet
     */
    void setState(const AppStateContext& newContext);
    
    // === HISTORIQUE DE NAVIGATION ===
    
    /**
     * @brief Empile l'état actuel et passe au nouveau
     */
    void pushState(AppState newState, uint8_t parameter = 0, uint8_t subState = 0);
    
    /**
     * @brief Empile l'état actuel et passe au nouveau contexte
     */
    void pushState(const AppStateContext& newContext);
    
    /**
     * @brief Revient à l'état précédent (bouton "back")
     */
    bool popState();
    
    /**
     * @brief Obtient l'état précédent sans le restaurer
     */
    AppState getPreviousState() const;
    
    /**
     * @brief Efface l'historique
     */
    void clearHistory();
    
    // === ACTIONS CONTEXTUELLES ===
    
    /**
     * @brief Gère l'action "retour"
     */
    void handleBackAction();
    
    /**
     * @brief Gère l'action "accueil"
     */
    void handleHomeAction();
    
    /**
     * @brief Gère une action de navigation générique
     */
    void handleNavigationAction(NavigationAction action, int parameter = 0);
    
    // === UTILITAIRES ===
    
    /**
     * @brief Vérifie si on peut revenir en arrière
     */
    bool canGoBack() const;
    
    /**
     * @brief Obtient la taille de l'historique
     */
    size_t getHistorySize() const;
    
    /**
     * @brief Vérifie si l'état est valide
     */
    bool isCurrentStateValid() const;
    
    /**
     * @brief Force un retour à l'état par défaut
     */
    void resetToDefaultState();

private:
    // === DÉPENDANCES ===
    IViewManager& viewManager_;
    
    // === ÉTAT ACTUEL ===
    AppStateContext currentContext_;
    
    // === HISTORIQUE ===
    static constexpr size_t MAX_HISTORY_SIZE = 8;
    etl::stack<AppStateContext, MAX_HISTORY_SIZE> stateHistory_;
    
    // === MÉTHODES PRIVÉES ===
    
    /**
     * @brief Exécute la transition vers un nouvel état
     */
    void executeStateTransition(const AppStateContext& newContext);
    
    /**
     * @brief Met à jour la vue selon l'état
     */
    void updateViewFromState(const AppStateContext& context);
    
    /**
     * @brief Détermine l'état cible selon l'action
     */
    AppState determineTargetState(NavigationAction action) const;
    
    /**
     * @brief Vérifie si l'action est valide dans l'état actuel
     */
    bool isActionValidInCurrentState(NavigationAction action) const;
    
    /**
     * @brief Met à jour le timestamp du contexte actuel
     */
    void updateCurrentTimestamp();
};