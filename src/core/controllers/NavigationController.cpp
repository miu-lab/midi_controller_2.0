#include "NavigationController.hpp"
#include "config/SystemConstants.hpp"

NavigationController::NavigationController(std::shared_ptr<NavigationStateManager> stateManager,
                                         std::shared_ptr<EventBus> eventBus)
    : stateManager_(stateManager)
    , eventBus_(eventBus)
    , handlerManager_(std::make_unique<NavigationHandlerManager>(stateManager))
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
    
    // Déléguer au système de handlers spécialisés
    if (handlerManager_) {
        handlerManager_->handleAction(action, parameter);
    }
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


bool NavigationController::isActionValidInCurrentContext(NavigationAction action) const {
    if (!stateManager_) {
        return false;
    }
    
    AppState currentState = stateManager_->getCurrentContext().state;
    
    // Validation basique : toutes les actions sauf depuis SPLASH_SCREEN
    // Les actions HOME et BACK sont toujours valides
    return (currentState != AppState::PARAMETER_FOCUS) || 
           (action == NavigationAction::HOME) ||
           (action == NavigationAction::BACK);
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