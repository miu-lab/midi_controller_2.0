#pragma once

#include "BaseNavigationHandler.hpp"
#include "config/SystemConstants.hpp"

/**
 * @brief Handler spécialisé pour les actions spéciales (HOME, BACK)
 * 
 * Gère les actions qui nécessitent une logique particulière
 * et ne suivent pas le routage standard.
 */
class SpecialActionHandler : public BaseNavigationHandler {
public:
    explicit SpecialActionHandler(std::shared_ptr<NavigationStateManager> stateManager)
        : BaseNavigationHandler(stateManager) {}

protected:
    bool canHandleAction(NavigationAction action) const override {
        return action == NavigationAction::HOME || 
               action == NavigationAction::BACK;
    }
    
    bool executeAction(NavigationAction action, int parameter) override {
        if (!stateManager_) {
            return false;
        }
        
        switch (action) {
            case NavigationAction::HOME:
                stateManager_->handleHomeAction();
                return true;
                
            case NavigationAction::BACK:
                stateManager_->handleBackAction();
                return true;
                
            default:
                return false;
        }
    }
};