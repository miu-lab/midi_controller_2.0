#pragma once

#include "BaseNavigationHandler.hpp"
#include "config/NavigationConstants.hpp"

/**
 * @brief Handler spécialisé pour les actions contextuelles
 * 
 * Gère les actions qui dépendent du contexte actuel
 * (ITEM_NAVIGATOR, ITEM_VALIDATE, etc.)
 */
class ContextualActionHandler : public BaseNavigationHandler {
public:
    explicit ContextualActionHandler(std::shared_ptr<NavigationStateManager> stateManager)
        : BaseNavigationHandler(stateManager) {}

protected:
    bool canHandleAction(NavigationAction action) const override {
        return action == NavigationAction::ITEM_NAVIGATOR ||
               action == NavigationAction::ITEM_VALIDATE ||
               action == NavigationAction::TOGGLE_VIEW;
    }
    
    bool executeAction(NavigationAction action, int parameter) override {
        switch (action) {
            case NavigationAction::ITEM_NAVIGATOR:
                return handleItemNavigator(parameter);
                
            case NavigationAction::ITEM_VALIDATE:
                return handleItemValidate(parameter);
                
            case NavigationAction::TOGGLE_VIEW:
                return handleToggleView(parameter);
                
            default:
                return false;
        }
    }

private:
    bool handleItemNavigator(int parameter) {
        // Navigation contextuelle - déléguer au StateManager
        if (!stateManager_) {
            return false;
        }
        stateManager_->handleNavigationAction(NavigationAction::ITEM_NAVIGATOR, parameter);
        return true;
    }
    
    bool handleItemValidate(int parameter) {
        // Validation contextuelle - déléguer au StateManager
        if (!stateManager_) {
            return false;
        }
        stateManager_->handleNavigationAction(NavigationAction::ITEM_VALIDATE, parameter);
        return true;
    }
    
    bool handleToggleView(int parameter) {
        // Basculer vers la vue debug
        performStateTransition(AppState::DEBUG_VIEW, static_cast<uint8_t>(parameter), false);
        return true;
    }
};