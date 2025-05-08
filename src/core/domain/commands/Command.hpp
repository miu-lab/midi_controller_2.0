#pragma once

/**
 * @brief Interface de base pour toutes les commandes (Command Pattern)
 *
 * Cette interface définit le contrat commun pour toutes les commandes
 * dans l'application, selon le pattern Command.
 */
class ICommand {
public:
    /**
     * @brief Exécute la commande
     */
    virtual void execute() = 0;

    /**
     * @brief Défait la commande (si possible)
     * @return true si la commande a pu être annulée, false sinon
     */
    virtual bool undo() = 0;

    /**
     * @brief Vérifie si la commande peut être annulée
     * @return true si la commande est annulable, false sinon
     */
    virtual bool isUndoable() const = 0;

    /**
     * @brief Obtient la description de la commande
     * @return Description textuelle de la commande
     */
    virtual const char* getDescription() const = 0;

    /**
     * @brief Destructeur virtuel
     */
    virtual ~ICommand() = default;
};
