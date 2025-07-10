#include "NavigationStateManager.hpp"

#include <Arduino.h>
#include "core/domain/interfaces/IViewManager.hpp"

NavigationStateManager::NavigationStateManager(IViewManager& viewManager)
    : viewManager_(viewManager)
    , currentContext_(AppState::SPLASH_SCREEN) {
    updateCurrentTimestamp();
}

AppState NavigationStateManager::getCurrentState() const {
    return currentContext_.state;
}

const AppStateContext& NavigationStateManager::getCurrentContext() const {
    return currentContext_;
}

void NavigationStateManager::setState(AppState newState, uint8_t parameter, uint8_t subState) {
    AppStateContext newContext(newState, parameter, subState);
    setState(newContext);
}

void NavigationStateManager::setState(const AppStateContext& newContext) {
    if (!AppStateUtils::isValid(newContext.state)) {
        resetToDefaultState();
        return;
    }
    
    if (!AppStateUtils::isValidTransition(currentContext_.state, newContext.state)) {
        return;
    }
    
    executeStateTransition(newContext);
}

void NavigationStateManager::pushState(AppState newState, uint8_t parameter, uint8_t subState) {
    AppStateContext newContext(newState, parameter, subState);
    pushState(newContext);
}

void NavigationStateManager::pushState(const AppStateContext& newContext) {
    if (!AppStateUtils::isValid(newContext.state)) {
        return;
    }
    
    if (!AppStateUtils::isValidTransition(currentContext_.state, newContext.state)) {
        return;
    }
    
    // Empiler l'état actuel seulement s'il est différent du nouveau
    if (currentContext_ != newContext) {
        if (!stateHistory_.full()) {
            stateHistory_.push(currentContext_);
        } else {
            // Historique plein, on supprime le plus ancien
            etl::stack<AppStateContext, MAX_HISTORY_SIZE> tempStack;
            
            // Garder les 7 plus récents
            for (size_t i = 0; i < MAX_HISTORY_SIZE - 1 && !stateHistory_.empty(); ++i) {
                tempStack.push(stateHistory_.top());
                stateHistory_.pop();
            }
            
            // Vider le stack original
            while (!stateHistory_.empty()) {
                stateHistory_.pop();
            }
            
            // Remettre les éléments dans l'ordre inverse
            while (!tempStack.empty()) {
                stateHistory_.push(tempStack.top());
                tempStack.pop();
            }
            
            // Ajouter l'état actuel
            stateHistory_.push(currentContext_);
        }
    }
    
    executeStateTransition(newContext);
}

bool NavigationStateManager::popState() {
    if (stateHistory_.empty()) {
        return false;
    }
    
    AppStateContext previousContext = stateHistory_.top();
    stateHistory_.pop();
    
    executeStateTransition(previousContext);
    return true;
}

AppState NavigationStateManager::getPreviousState() const {
    if (stateHistory_.empty()) {
        return AppStateUtils::getDefaultState();
    }
    
    return stateHistory_.top().state;
}

void NavigationStateManager::clearHistory() {
    while (!stateHistory_.empty()) {
        stateHistory_.pop();
    }
}

void NavigationStateManager::handleBackAction() {
    if (!popState()) {
        // Pas d'historique, retour à l'état par défaut
        setState(AppStateUtils::getDefaultState());
    }
}

void NavigationStateManager::handleHomeAction() {
    // Toggle entre MENU et PARAMETER_FOCUS
    switch (currentContext_.state) {
        case AppState::MENU:
            // Si on est dans le menu, retourner à l'accueil
            setState(AppState::PARAMETER_FOCUS);
            break;
            
        case AppState::PARAMETER_FOCUS:
            // Si on est à l'accueil, aller au menu
            setState(AppState::MENU);
            break;
            
        default:
            // Depuis tout autre état, aller au menu
            setState(AppState::MENU);
            break;
    }
}

void NavigationStateManager::handleNavigationAction(NavigationAction action, int parameter) {
    if (!isActionValidInCurrentState(action)) {
        return;
    }
    
    switch (action) {
        case NavigationAction::HOME:
            handleHomeAction();
            break;
            
        case NavigationAction::BACK:
            handleBackAction();
            break;
            
        case NavigationAction::MENU_ENTER:
            pushState(AppState::MENU);
            break;
            
        case NavigationAction::MENU_EXIT:
            handleBackAction();
            break;
            
        case NavigationAction::ITEM_NAVIGATOR:
            // Navigation dans l'état actuel (pas de changement d'état)
            if (currentContext_.state == AppState::MENU) {
                viewManager_.navigateMenu(parameter);
            }
            break;
            
        case NavigationAction::ITEM_VALIDATE:
            // Validation selon l'état actuel
            if (currentContext_.state == AppState::MENU) {
                // Validation d'un élément de menu - rester dans le menu
                setState(AppState::MENU, static_cast<uint8_t>(parameter));
            }
            break;
            
        case NavigationAction::PARAMETER_EDIT:
            pushState(AppState::PARAMETER_EDIT, static_cast<uint8_t>(parameter));
            break;
            
        case NavigationAction::PARAMETER_VALIDATE:
            if (currentContext_.state == AppState::PARAMETER_EDIT) {
                handleBackAction();
            }
            break;
            
        case NavigationAction::PARAMETER_CANCEL:
            if (currentContext_.state == AppState::PARAMETER_EDIT) {
                handleBackAction();
            }
            break;
            
        default:
            // Action non gérée
            break;
    }
}

bool NavigationStateManager::canGoBack() const {
    return !stateHistory_.empty();
}

size_t NavigationStateManager::getHistorySize() const {
    return stateHistory_.size();
}

bool NavigationStateManager::isCurrentStateValid() const {
    return AppStateUtils::isValid(currentContext_.state);
}

void NavigationStateManager::resetToDefaultState() {
    clearHistory();
    AppStateContext defaultContext(AppStateUtils::getDefaultState());
    executeStateTransition(defaultContext);
}

void NavigationStateManager::executeStateTransition(const AppStateContext& newContext) {
    currentContext_ = newContext;
    updateCurrentTimestamp();
    updateViewFromState(currentContext_);
}

void NavigationStateManager::updateViewFromState(const AppStateContext& context) {
    switch (context.state) {
        case AppState::SPLASH_SCREEN:
            // Splash géré automatiquement par DefaultViewManager
            break;
            
        case AppState::PARAMETER_FOCUS:
            viewManager_.showHome();
            break;
            
        case AppState::MENU:
            viewManager_.showMenu();
            break;
            
        case AppState::PARAMETER_EDIT:
            // Édition de paramètre - reste sur la vue actuelle
            break;
            
        case AppState::MODAL_DIALOG:
            // Modal géré séparément
            break;
            
        case AppState::DEBUG_VIEW:
            // Vue de debug - pas encore implémentée
            break;
            
        case AppState::PROFILE_SELECTION:
            // Sélection de profil - pas encore implémentée
            break;
    }
}

AppState NavigationStateManager::determineTargetState(NavigationAction action) const {
    switch (action) {
        case NavigationAction::HOME:
            return AppState::PARAMETER_FOCUS;
            
        case NavigationAction::MENU_ENTER:
            return AppState::MENU;
            
        case NavigationAction::PARAMETER_EDIT:
            return AppState::PARAMETER_EDIT;
            
        case NavigationAction::BACK:
            return getPreviousState();
            
        default:
            return currentContext_.state;
    }
}

bool NavigationStateManager::isActionValidInCurrentState(NavigationAction action) const {
    switch (currentContext_.state) {
        case AppState::SPLASH_SCREEN:
            // Depuis splash, seules certaines actions sont valides
            return action == NavigationAction::HOME;
            
        case AppState::PARAMETER_FOCUS:
            // Depuis parameter focus, toutes les actions sont valides
            return true;
            
        case AppState::MENU:
            // Depuis menu, toutes les actions sauf parameter_edit sont valides
            return action != NavigationAction::PARAMETER_VALIDATE && 
                   action != NavigationAction::PARAMETER_CANCEL;
            
        case AppState::PARAMETER_EDIT:
            // Depuis parameter edit, seules certaines actions sont valides
            return action == NavigationAction::PARAMETER_VALIDATE || 
                   action == NavigationAction::PARAMETER_CANCEL ||
                   action == NavigationAction::BACK ||
                   action == NavigationAction::HOME;
            
        case AppState::MODAL_DIALOG:
            // Depuis modal, seules back et home sont valides
            return action == NavigationAction::BACK || 
                   action == NavigationAction::HOME;
            
        default:
            return true;
    }
}

void NavigationStateManager::updateCurrentTimestamp() {
    currentContext_.timestamp = millis();
}