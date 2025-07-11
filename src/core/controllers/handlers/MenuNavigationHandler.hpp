#pragma once

#include "BaseNavigationHandler.hpp"
#include "config/NavigationConstants.hpp"

/**
 * @brief Handler spécialisé pour la navigation dans les menus
 * 
 * Gère les actions d'entrée/sortie de menu et la navigation
 * entre les éléments de menu.
 */
class MenuNavigationHandler : public BaseNavigationHandler {
public:
    explicit MenuNavigationHandler(std::shared_ptr<NavigationStateManager> stateManager)
        : BaseNavigationHandler(stateManager) {}

protected:
    bool canHandleAction(NavigationAction action) const override {
        return action == NavigationAction::MENU_ENTER ||
               action == NavigationAction::MENU_EXIT ||
               action == NavigationAction::ITEM_NEXT ||
               action == NavigationAction::ITEM_PREVIOUS;
    }
    
    bool executeAction(NavigationAction action, int parameter) override {
        switch (action) {
            case NavigationAction::MENU_ENTER:
                return handleMenuEnter(parameter);
                
            case NavigationAction::MENU_EXIT:
                return handleMenuExit(parameter);
                
            case NavigationAction::ITEM_NEXT:
            case NavigationAction::ITEM_PREVIOUS:
                return handleItemNavigation(action, parameter);
                
            default:
                return false;
        }
    }

private:
    bool handleMenuEnter(int parameter) {
        performStateTransition(AppState::MENU, static_cast<uint8_t>(parameter), true);
        return true;
    }
    
    bool handleMenuExit(int parameter) {
        // Sortir du menu = retour à l'état précédent ou PARAMETER_FOCUS
        AppState targetState = canGoBack() ? getPreviousState() : NavigationConstants::DefaultStates::FALLBACK_STATE;
        performStateTransition(targetState, static_cast<uint8_t>(parameter), false);
        return true;
    }
    
    bool handleItemNavigation(NavigationAction action, int parameter) {
        // Ces actions restent dans l'état MENU mais sont contextuelles
        // Pas de changement d'état, juste propagation à l'UI
        return true;
    }
};