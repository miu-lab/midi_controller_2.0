#include "MenuController.hpp"

MenuController::MenuController(IViewManager& viewManager, CommandManager& commandManager)
    : viewManager_(viewManager), commandManager_(commandManager) {}

void MenuController::enterMenu() {
    auto command =
        std::make_unique<NavigateMenuCommand>(viewManager_, NavigateMenuCommand::Action::ENTER);
    commandManager_.execute(std::move(command));
}

void MenuController::exitMenu() {
    auto command =
        std::make_unique<NavigateMenuCommand>(viewManager_, NavigateMenuCommand::Action::EXIT);
    commandManager_.execute(std::move(command));
}

void MenuController::selectNextItem() {
    auto command =
        std::make_unique<NavigateMenuCommand>(viewManager_, NavigateMenuCommand::Action::NEXT_ITEM);
    commandManager_.execute(std::move(command));
}

void MenuController::selectPreviousItem() {
    auto command =
        std::make_unique<NavigateMenuCommand>(viewManager_,
                                              NavigateMenuCommand::Action::PREVIOUS_ITEM);
    commandManager_.execute(std::move(command));
}

void MenuController::selectItem(int index) {
    auto command = std::make_unique<NavigateMenuCommand>(viewManager_,
                                                         NavigateMenuCommand::Action::HOME,
                                                         index);
    commandManager_.execute(std::move(command));
}

int MenuController::getCurrentItem() const {
    // Retourner un index fictif pour l'instant
    return 0;
}

bool MenuController::isInMenu() const {
    // Retourner false pour l'instant - à implémenter si nécessaire
    return false;
}

bool MenuController::undoLastNavigation() {
    return commandManager_.undo();
}

bool MenuController::redoNavigation() {
    return commandManager_.redo();
}
