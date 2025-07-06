#include "core/domain/commands/ui/NavigateMenuCommand.hpp"

#include <cstdio>  // Pour snprintf

#include "core/domain/interfaces/IViewManager.hpp"

NavigateMenuCommand::NavigateMenuCommand(IViewManager& viewManager, Action action, int itemIndex)
    : viewManager_(viewManager),
      action_(action),
      itemIndex_(itemIndex),
      previousIndex_(-1),
      previousState_(false) {}

void NavigateMenuCommand::execute() {
    if (!hasExecuted_) {
        // Dans l'architecture simplifiée, pas besoin de sauvegarder l'état
        // previousIndex_ = 0; // Plus nécessaire 
        // previousState_ = false; // Plus nécessaire
        hasExecuted_ = true;
    }

    // Exécuter l'action en fonction du type
    switch (action_) {
    case Action::ENTER:
        viewManager_.showMenu();
        break;

    case Action::EXIT:
        viewManager_.showHome();
        break;

    case Action::NEXT_ITEM:
        viewManager_.navigateMenu(1);
        break;

    case Action::PREVIOUS_ITEM:
        viewManager_.navigateMenu(-1);
        break;

    case Action::HOME:
        viewManager_.selectMenuItem();
        break;
    }
}

bool NavigateMenuCommand::undo() {
    if (!hasExecuted_) {
        return false;
    }

    // Dans l'architecture simplifiée, juste revenir à la vue par défaut
    viewManager_.showHome();
    
    return true;
}

bool NavigateMenuCommand::isUndoable() const {
    // La plupart des actions de navigation sont annulables
    return hasExecuted_;
}

const char* NavigateMenuCommand::getDescription() const {
    static char buffer[64];

    switch (action_) {
    case Action::ENTER:
        return "Enter Menu";

    case Action::EXIT:
        return "Exit Menu";

    case Action::NEXT_ITEM:
        return "Next Menu Item";

    case Action::PREVIOUS_ITEM:
        return "Previous Menu Item";

    case Action::HOME:
        snprintf(buffer, sizeof(buffer), "Go to Menu Item %d", itemIndex_);
        return buffer;

    default:
        return "Menu Navigation";
    }
}
