#include "core/domain/commands/CommandManager.hpp"

void CommandManager::execute(std::unique_ptr<ICommand> command) {
    // Exécuter la commande
    command->execute();

    // Effacer la pile de rétablissement car une nouvelle action a été exécutée
    redoStack.clear();

    // Ajouter à l'historique seulement si la commande est annulable
    if (command->isUndoable()) {
        undoStack.push_back(std::move(command));
    }
}

bool CommandManager::undo() {
    if (undoStack.empty()) {
        return false;
    }

    // Prendre la dernière commande
    std::unique_ptr<ICommand> command = std::move(undoStack.back());
    undoStack.pop_back();

    // Annuler la commande
    bool success = command->undo();

    // Si l'annulation a réussi, ajouter à la pile de rétablissement
    if (success) {
        redoStack.push_back(std::move(command));
    }

    return success;
}

bool CommandManager::redo() {
    if (redoStack.empty()) {
        return false;
    }

    // Prendre la dernière commande annulée
    std::unique_ptr<ICommand> command = std::move(redoStack.back());
    redoStack.pop_back();

    // Ré-exécuter la commande
    command->execute();

    // Ajouter à la pile d'annulation
    undoStack.push_back(std::move(command));

    return true;
}

bool CommandManager::canUndo() const {
    return !undoStack.empty();
}

bool CommandManager::canRedo() const {
    return !redoStack.empty();
}

void CommandManager::clear() {
    undoStack.clear();
    redoStack.clear();
}

const char* CommandManager::getLastCommandDescription() const {
    if (undoStack.empty()) {
        return nullptr;
    }
    return undoStack.back()->getDescription();
}

size_t CommandManager::getHistorySize() const {
    return undoStack.size();
}
