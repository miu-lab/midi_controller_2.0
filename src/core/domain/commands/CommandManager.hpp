#pragma once

#include <memory>
#include <vector>

#include "core/domain/commands/Command.hpp"

/**
 * @brief Gestionnaire de commandes avec historique et annulation
 *
 * Cette classe gère l'exécution et l'historique des commandes pour
 * permettre l'annulation (undo) et le rétablissement (redo).
 */
class CommandManager {
public:
    /**
     * @brief Exécute une commande et l'ajoute à l'historique
     * @param command Commande à exécuter
     */
    void execute(std::unique_ptr<ICommand> command);

    /**
     * @brief Annule la dernière commande exécutée
     * @return true si une commande a été annulée, false sinon
     */
    bool undo();

    /**
     * @brief Rétablit la dernière commande annulée
     * @return true si une commande a été rétablie, false sinon
     */
    bool redo();

    /**
     * @brief Vérifie si une annulation est possible
     * @return true s'il y a des commandes à annuler, false sinon
     */
    bool canUndo() const;

    /**
     * @brief Vérifie si un rétablissement est possible
     * @return true s'il y a des commandes à rétablir, false sinon
     */
    bool canRedo() const;

    /**
     * @brief Efface tout l'historique des commandes
     */
    void clear();

    /**
     * @brief Obtient la description de la dernière commande
     * @return Description de la dernière commande, ou nullptr si aucune
     */
    const char* getLastCommandDescription() const;

    /**
     * @brief Obtient le nombre de commandes dans l'historique
     * @return Nombre de commandes
     */
    size_t getHistorySize() const;

private:
    std::vector<std::unique_ptr<ICommand>> undoStack;  // Commandes qui peuvent être annulées
    std::vector<std::unique_ptr<ICommand>> redoStack;  // Commandes qui peuvent être rétablies
};
