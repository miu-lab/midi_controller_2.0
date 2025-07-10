#include "AppState.hpp"

const char* AppStateUtils::toString(AppState state) {
    switch (state) {
        case AppState::SPLASH_SCREEN: return "SPLASH_SCREEN";
        case AppState::PARAMETER_FOCUS: return "PARAMETER_FOCUS";
        case AppState::MENU: return "MENU";
        case AppState::PARAMETER_EDIT: return "PARAMETER_EDIT";
        case AppState::MODAL_DIALOG: return "MODAL_DIALOG";
        case AppState::DEBUG_VIEW: return "DEBUG_VIEW";
        case AppState::PROFILE_SELECTION: return "PROFILE_SELECTION";
        default: return "UNKNOWN";
    }
}

bool AppStateUtils::isValid(AppState state) {
    return state >= AppState::SPLASH_SCREEN && state <= AppState::PROFILE_SELECTION;
}

bool AppStateUtils::isValidTransition(AppState from, AppState to) {
    // Toutes les transitions sont valides sauf quelques cas spécifiques
    if (!isValid(from) || !isValid(to)) {
        return false;
    }
    
    // Transitions invalides
    switch (from) {
        case AppState::SPLASH_SCREEN:
            // Depuis splash, on peut aller vers PARAMETER_FOCUS ou MENU
            return to == AppState::PARAMETER_FOCUS || to == AppState::MENU;
            
        case AppState::MODAL_DIALOG:
            // Depuis modal, on ne peut pas aller vers splash
            return to != AppState::SPLASH_SCREEN;
            
        default:
            // Toutes les autres transitions sont valides
            return true;
    }
}

AppState AppStateUtils::getDefaultState() {
    return AppState::PARAMETER_FOCUS;
}