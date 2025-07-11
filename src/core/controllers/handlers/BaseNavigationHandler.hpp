#pragma once

#include <memory>
#include "core/domain/navigation/NavigationStateManager.hpp"
#include "core/domain/navigation/NavigationAction.hpp"
#include "core/domain/navigation/AppState.hpp"

/**
 * @brief Classe de base pour tous les handlers de navigation
 * 
 * Applique le Template Method Pattern pour factoriser la logique commune
 * de validation et de transition d'état.
 */
class BaseNavigationHandler {
public:
    /**
     * @brief Constructeur
     * @param stateManager Gestionnaire d'état partagé
     */
    explicit BaseNavigationHandler(std::shared_ptr<NavigationStateManager> stateManager)
        : stateManager_(stateManager) {}
    
    /**
     * @brief Destructeur virtuel
     */
    virtual ~BaseNavigationHandler() = default;
    
    /**
     * @brief Méthode principale - Template Method Pattern
     * @param action Action de navigation
     * @param parameter Paramètre optionnel
     * @return true si l'action a été traitée avec succès
     */
    bool handleAction(NavigationAction action, int parameter = 0) {
        // Validation commune
        if (!isValidContext()) {
            return false;
        }
        
        if (!canHandleAction(action)) {
            return false;
        }
        
        // Exécution spécifique au handler
        return executeAction(action, parameter);
    }

protected:
    /**
     * @brief Vérifie si le contexte est valide (Template Method)
     */
    virtual bool isValidContext() const {
        return stateManager_ != nullptr;
    }
    
    /**
     * @brief Vérifie si ce handler peut traiter l'action (à implémenter)
     */
    virtual bool canHandleAction(NavigationAction action) const = 0;
    
    /**
     * @brief Exécute l'action spécifique (à implémenter)
     */
    virtual bool executeAction(NavigationAction action, int parameter) = 0;
    
    /**
     * @brief Helpers communes pour les sous-classes
     */
    void performStateTransition(AppState targetState, uint8_t parameter = 0, bool pushToHistory = false) {
        if (!stateManager_) return;
        
        if (pushToHistory) {
            stateManager_->pushState(targetState, parameter);
        } else {
            stateManager_->setState(targetState, parameter);
        }
    }
    
    AppState getCurrentState() const {
        return stateManager_ ? stateManager_->getCurrentState() : AppState::SPLASH_SCREEN;
    }
    
    bool canGoBack() const {
        return stateManager_ ? stateManager_->canGoBack() : false;
    }
    
    AppState getPreviousState() const {
        return stateManager_ ? stateManager_->getPreviousState() : AppState::SPLASH_SCREEN;
    }

protected:
    std::shared_ptr<NavigationStateManager> stateManager_;
};