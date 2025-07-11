#pragma once

#include "BaseNavigationHandler.hpp"
#include "config/NavigationConstants.hpp"

/**
 * @brief Handler spécialisé pour les actions sur les paramètres
 * 
 * Gère l'édition, validation et annulation des paramètres
 * avec la logique de transition d'état appropriée.
 */
class ParameterActionHandler : public BaseNavigationHandler {
public:
    explicit ParameterActionHandler(std::shared_ptr<NavigationStateManager> stateManager)
        : BaseNavigationHandler(stateManager) {}

protected:
    bool canHandleAction(NavigationAction action) const override {
        return action == NavigationAction::PARAMETER_EDIT ||
               action == NavigationAction::PARAMETER_VALIDATE ||
               action == NavigationAction::PARAMETER_CANCEL;
    }
    
    bool executeAction(NavigationAction action, int parameter) override {
        switch (action) {
            case NavigationAction::PARAMETER_EDIT:
                return handleParameterEdit(parameter);
                
            case NavigationAction::PARAMETER_VALIDATE:
                return handleParameterValidate(parameter);
                
            case NavigationAction::PARAMETER_CANCEL:
                return handleParameterCancel(parameter);
                
            default:
                return false;
        }
    }

private:
    bool handleParameterEdit(int parameter) {
        // Entrer en mode édition - pousser dans l'historique
        performStateTransition(AppState::PARAMETER_EDIT, static_cast<uint8_t>(parameter), true);
        return true;
    }
    
    bool handleParameterValidate(int parameter) {
        // Validation = retour à l'état précédent ou PARAMETER_FOCUS
        AppState targetState = determineExitState();
        performStateTransition(targetState, static_cast<uint8_t>(parameter), false);
        return true;
    }
    
    bool handleParameterCancel(int parameter) {
        // Annulation = même logique que validation
        AppState targetState = determineExitState();
        performStateTransition(targetState, static_cast<uint8_t>(parameter), false);
        return true;
    }
    
    AppState determineExitState() const {
        // Si on est en PARAMETER_EDIT et qu'on peut revenir en arrière
        if (getCurrentState() == AppState::PARAMETER_EDIT && canGoBack()) {
            return getPreviousState();
        }
        return NavigationConstants::DefaultStates::FALLBACK_STATE;
    }
};