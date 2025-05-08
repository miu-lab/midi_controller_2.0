#include "core/domain/commands/ui/NavigateMenuCommand.hpp"

#include <cstdio>  // Pour snprintf

#include "adapters/primary/ui/ViewManager.hpp"

NavigateMenuCommand::NavigateMenuCommand(ViewManager& viewManager, Action action, int itemIndex)
    : viewManager_(viewManager),
      action_(action),
      itemIndex_(itemIndex),
      previousIndex_(-1),
      previousState_(false) {}

void NavigateMenuCommand::execute() {
    if (!hasExecuted_) {
        // Sauvegarder l'état actuel pour l'annulation
        previousIndex_ = viewManager_.getCurrentMenuIndex();
        previousState_ = viewManager_.isInMenu();
        hasExecuted_ = true;
    }

    // Exécuter l'action en fonction du type
    switch (action_) {
    case Action::ENTER_MENU:
        viewManager_.enterMenu();
        break;

    case Action::EXIT_MENU:
        viewManager_.exitMenu();
        break;

    case Action::NEXT_ITEM:
        viewManager_.selectNextMenuItem();
        break;

    case Action::PREVIOUS_ITEM:
        viewManager_.selectPreviousMenuItem();
        break;

    case Action::GO_TO_ITEM:
        if (itemIndex_ >= 0) {
            viewManager_.selectMenuItem(itemIndex_);
        }
        break;
    }
}

bool NavigateMenuCommand::undo() {
    if (!hasExecuted_) {
        return false;
    }

    // Restaurer l'état précédent
    if (previousState_ != viewManager_.isInMenu()) {
        if (previousState_) {
            viewManager_.enterMenu();
        } else {
            viewManager_.exitMenu();
        }
    }

    if (previousIndex_ >= 0) {
        viewManager_.selectMenuItem(previousIndex_);
    }

    return true;
}

bool NavigateMenuCommand::isUndoable() const {
    // La plupart des actions de navigation sont annulables
    return hasExecuted_;
}

const char* NavigateMenuCommand::getDescription() const {
    static char buffer[64];

    switch (action_) {
    case Action::ENTER_MENU:
        return "Enter Menu";

    case Action::EXIT_MENU:
        return "Exit Menu";

    case Action::NEXT_ITEM:
        return "Next Menu Item";

    case Action::PREVIOUS_ITEM:
        return "Previous Menu Item";

    case Action::GO_TO_ITEM:
        snprintf(buffer, sizeof(buffer), "Go to Menu Item %d", itemIndex_);
        return buffer;

    default:
        return "Menu Navigation";
    }
}
