#include "core/domain/navigation/NavigationAction.hpp"

const char* NavigationActionUtils::toString(NavigationAction action) {
    switch (action) {
        case NavigationAction::HOME: return "HOME";
        case NavigationAction::BACK: return "BACK";
        case NavigationAction::MENU_ENTER: return "MENU_ENTER";
        case NavigationAction::MENU_EXIT: return "MENU_EXIT";
        case NavigationAction::ITEM_NAVIGATOR: return "ITEM_NAVIGATOR";
        case NavigationAction::ITEM_VALIDATE: return "ITEM_VALIDATE";
        case NavigationAction::ITEM_NEXT: return "ITEM_NEXT";
        case NavigationAction::ITEM_PREVIOUS: return "ITEM_PREVIOUS";
        case NavigationAction::PARAMETER_EDIT: return "PARAMETER_EDIT";
        case NavigationAction::PARAMETER_VALIDATE: return "PARAMETER_VALIDATE";
        case NavigationAction::PARAMETER_CANCEL: return "PARAMETER_CANCEL";
        case NavigationAction::PROFILE_SWITCH: return "PROFILE_SWITCH";
        case NavigationAction::CONTEXT_SWITCH: return "CONTEXT_SWITCH";
        case NavigationAction::QUICK_ACTION_1: return "QUICK_ACTION_1";
        case NavigationAction::QUICK_ACTION_2: return "QUICK_ACTION_2";
        case NavigationAction::TOGGLE_VIEW: return "TOGGLE_VIEW";
        default: return "UNKNOWN";
    }
}

bool NavigationActionUtils::isValid(NavigationAction action) {
    return static_cast<uint8_t>(action) <= static_cast<uint8_t>(NavigationAction::TOGGLE_VIEW);
}