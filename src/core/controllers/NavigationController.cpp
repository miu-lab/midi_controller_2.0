#include "NavigationController.hpp"

#include <functional>

// Table de routage statique
const etl::array<NavigationController::ActionRoute, 10> NavigationController::ACTION_ROUTES = {{
    // Actions de navigation générale
    {NavigationAction::MENU_ENTER, AppState::MENU, true, false},
    {NavigationAction::MENU_EXIT, AppState::PARAMETER_FOCUS, false, false},
    
    // Actions de paramètres
    {NavigationAction::PARAMETER_EDIT, AppState::PARAMETER_EDIT, true, true},
    {NavigationAction::PARAMETER_VALIDATE, AppState::PARAMETER_FOCUS, false, false},
    {NavigationAction::PARAMETER_CANCEL, AppState::PARAMETER_FOCUS, false, false},
    
    // Actions contextuelles (pas de routage direct)
    // ITEM_NAVIGATOR supprimé - traité contextuellement
    // ITEM_VALIDATE supprimé - traité contextuellement
    {NavigationAction::ITEM_NEXT, AppState::MENU, false, false},
    {NavigationAction::ITEM_PREVIOUS, AppState::MENU, false, false},
    
    // Actions spéciales (traitées séparément)
    {NavigationAction::HOME, AppState::PARAMETER_FOCUS, false, false},
    {NavigationAction::BACK, AppState::PARAMETER_FOCUS, false, false},
    
    // Actions avancées
    {NavigationAction::TOGGLE_VIEW, AppState::DEBUG_VIEW, false, false}
}};

NavigationController::NavigationController(std::shared_ptr<NavigationStateManager> stateManager,
                                         std::shared_ptr<EventBus> eventBus)
    : stateManager_(stateManager)
    , eventBus_(eventBus)
    , initialized_(false) {
}

NavigationController::~NavigationController() {
    // Les abonnements aux événements seront automatiquement nettoyés
}

void NavigationController::initialize() {
    if (initialized_) {
        return;
    }
    
    subscribeToEvents();
    initialized_ = true;
}

void NavigationController::subscribeToEvents() {
    if (!eventBus_) {
        return;
    }
    
    // S'abonner aux événements de navigation en tant qu'EventListener
    eventBus_->subscribe(this);
}

void NavigationController::handleNavigationEvent(const NavigationEvent& event) {
    if (!stateManager_) {
        return;
    }
    
    routeNavigationAction(event.getAction(), event.getParameter());
}

void NavigationController::handleStateChangeEvent(const StateChangeEvent& event) {
    if (!stateManager_) {
        return;
    }
    
    // Si parameter=1, cela indique qu'on veut pousser l'état actuel dans l'historique
    // (pour la navigation vers sous-page)
    if (event.getParameter() == 1 && event.getNewState() == AppState::MENU) {
        // Pousser l'état MENU avec subState=1 pour créer un contexte différent du courant
        stateManager_->pushState(event.getNewState(), 0, 1);
    } else {
        forceStateChange(event.getNewState(), event.getParameter(), event.getSubState());
    }
}

void NavigationController::handleBackRequestedEvent(const BackRequestedEvent& event) {
    if (!stateManager_) {
        return;
    }
    
    stateManager_->handleBackAction();
}

void NavigationController::handleHomeRequestedEvent(const HomeRequestedEvent& event) {
    if (!stateManager_) {
        return;
    }
    
    stateManager_->handleHomeAction();
}

void NavigationController::routeNavigationAction(NavigationAction action, int parameter) {
    if (!isActionValidInCurrentContext(action)) {
        return;
    }
    
    // Traiter les actions spéciales en premier
    if (action == NavigationAction::HOME || action == NavigationAction::BACK) {
        handleSpecialAction(action, parameter);
        return;
    }
    
    // Traiter les actions normales
    handleNormalAction(action, parameter);
}

void NavigationController::forceStateChange(AppState newState, uint8_t parameter, uint8_t subState) {
    if (!stateManager_) {
        return;
    }
    
    stateManager_->setState(newState, parameter, subState);
}

AppState NavigationController::getCurrentState() const {
    if (!stateManager_) {
        return AppState::SPLASH_SCREEN;
    }
    
    return stateManager_->getCurrentState();
}

bool NavigationController::canGoBack() const {
    if (!stateManager_) {
        return false;
    }
    
    return stateManager_->canGoBack();
}

size_t NavigationController::getNavigationHistorySize() const {
    if (!stateManager_) {
        return 0;
    }
    
    return stateManager_->getHistorySize();
}

void NavigationController::executeContextualAction(NavigationAction action, int parameter) {
    stateManager_->handleNavigationAction(action, parameter);
}

AppState NavigationController::determineTargetState(NavigationAction action, AppState currentState) const {
    // Trouver la route pour cette action
    const ActionRoute* route = findActionRoute(action);
    if (!route) {
        return currentState;
    }
    
    // Certaines actions ont un état cible contextuel
    switch (action) {
        case NavigationAction::BACK:
            return stateManager_->getPreviousState();
            
        case NavigationAction::MENU_EXIT:
            // Sortir du menu = retour à l'état précédent ou PARAMETER_FOCUS
            if (stateManager_->canGoBack()) {
                return stateManager_->getPreviousState();
            }
            return AppState::PARAMETER_FOCUS;
            
        case NavigationAction::PARAMETER_VALIDATE:
        case NavigationAction::PARAMETER_CANCEL:
            // Validation/annulation de paramètre = retour à l'état précédent
            if (currentState == AppState::PARAMETER_EDIT && stateManager_->canGoBack()) {
                return stateManager_->getPreviousState();
            }
            return AppState::PARAMETER_FOCUS;
            
        default:
            return route->targetState;
    }
}

const NavigationController::ActionRoute* NavigationController::findActionRoute(NavigationAction action) const {
    for (const auto& route : ACTION_ROUTES) {
        if (route.action == action) {
            return &route;
        }
    }
    return nullptr;
}

bool NavigationController::isActionValidInCurrentContext(NavigationAction action) const {
    if (!stateManager_) {
        return false;
    }
    
    AppState currentState = stateManager_->getCurrentContext().state;
    
    // Validation basique : toutes les actions sauf depuis SPLASH_SCREEN
    // Les actions HOME et BACK sont toujours valides
    return (currentState != AppState::SPLASH_SCREEN) || 
           (action == NavigationAction::HOME) ||
           (action == NavigationAction::BACK);
}

void NavigationController::handleSpecialAction(NavigationAction action, int parameter) {
    if (!stateManager_) {
        return;
    }
    
    switch (action) {
        case NavigationAction::HOME:
            stateManager_->handleHomeAction();
            break;
            
        case NavigationAction::BACK:
            stateManager_->handleBackAction();
            break;
            
        default:
            // Action spéciale inconnue
            break;
    }
}

void NavigationController::handleNormalAction(NavigationAction action, int parameter) {
    const ActionRoute* route = findActionRoute(action);
    if (!route) {
        // Action non trouvée, traiter contextuellement
        executeContextualAction(action, parameter);
        return;
    }
    
    // Déterminer l'état cible
    AppState targetState = determineTargetState(action, getCurrentState());
    
    // Exécuter la transition
    if (route->pushToHistory) {
        stateManager_->pushState(targetState, static_cast<uint8_t>(parameter));
    } else {
        stateManager_->setState(targetState, static_cast<uint8_t>(parameter));
    }
}

bool NavigationController::onEvent(const Event& event) {
    switch (event.getType()) {
        case NavigationEventTypes::NAVIGATION_REQUESTED:
            handleNavigationEvent(static_cast<const NavigationEvent&>(event));
            return true;
            
        case NavigationEventTypes::STATE_CHANGE_REQUESTED:
            handleStateChangeEvent(static_cast<const StateChangeEvent&>(event));
            return true;
            
        case NavigationEventTypes::BACK_REQUESTED:
            handleBackRequestedEvent(static_cast<const BackRequestedEvent&>(event));
            return true;
            
        case NavigationEventTypes::HOME_REQUESTED:
            handleHomeRequestedEvent(static_cast<const HomeRequestedEvent&>(event));
            return true;
            
        default:
            // Événement non géré
            return false;
    }
}