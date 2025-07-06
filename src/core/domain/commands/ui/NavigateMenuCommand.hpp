#pragma once

#include <memory>

#include "core/domain/commands/Command.hpp"

// Forward declaration pour éviter les dépendances circulaires
class IViewManager;

/**
 * @brief Commande pour naviguer dans un menu
 */
class NavigateMenuCommand : public ICommand {
public:
    /**
     * @brief Types d'action de navigation
     */
    enum class Action {
        ENTER,          ///< Entrer dans un menu ou sélectionner une option
        EXIT,           ///< Sortir d'un menu
        NEXT_ITEM,      ///< Sélectionner l'élément suivant
        PREVIOUS_ITEM,  ///< Sélectionner l'élément précédent
        HOME            ///< Aller directement à un élément spécifique
    };

    /**
     * @brief Constructeur
     * @param viewManager Gestionnaire de vues
     * @param action Action à effectuer
     * @param itemIndex Index de l'élément (pour GO_TO_ITEM)
     */
    NavigateMenuCommand(IViewManager& viewManager, Action action, int itemIndex = -1);

    /**
     * @brief Exécute la commande : effectue l'action de navigation
     */
    void execute() override;

    /**
     * @brief Annule la commande (si possible)
     * @return true si la commande a été annulée, false sinon
     */
    bool undo() override;

    /**
     * @brief Vérifie si la commande peut être annulée
     * @return true si la commande est annulable, false sinon
     */
    bool isUndoable() const override;

    /**
     * @brief Obtient la description de la commande
     * @return Description textuelle de la commande
     */
    const char* getDescription() const override;

private:
    IViewManager& viewManager_;
    Action action_;
    int itemIndex_;
    int previousIndex_;
    bool previousState_;  // État précédent pour l'annulation
    bool hasExecuted_ = false;
};
