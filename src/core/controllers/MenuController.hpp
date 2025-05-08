#pragma once

#include "adapters/primary/ui/ViewManager.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/domain/commands/ui/NavigateMenuCommand.hpp"
#include "core/domain/types.hpp"

/**
 * @brief Contrôleur dédié à la navigation dans les menus
 *
 * Cette classe est responsable de toutes les opérations liées à la
 * navigation dans les menus de l'interface utilisateur.
 */
class MenuController {
public:
    /**
     * @brief Constructeur
     * @param viewManager Gestionnaire de vues
     * @param commandManager Gestionnaire de commandes
     */
    MenuController(ViewManager& viewManager, CommandManager& commandManager);

    /**
     * @brief Entre dans un menu
     */
    void enterMenu();

    /**
     * @brief Sort d'un menu
     */
    void exitMenu();

    /**
     * @brief Sélectionne l'élément de menu suivant
     */
    void selectNextItem();

    /**
     * @brief Sélectionne l'élément de menu précédent
     */
    void selectPreviousItem();

    /**
     * @brief Sélectionne un élément de menu spécifique
     * @param index Index de l'élément à sélectionner
     */
    void selectItem(int index);

    /**
     * @brief Obtient l'élément de menu actuellement sélectionné
     * @return Index de l'élément sélectionné, ou -1 si aucun
     */
    int getCurrentItem() const;

    /**
     * @brief Vérifie si l'utilisateur est actuellement dans un menu
     * @return true si dans un menu, false sinon
     */
    bool isInMenu() const;

    /**
     * @brief Annule la dernière action de navigation
     * @return true si une action a été annulée, false sinon
     */
    bool undoLastNavigation();

    /**
     * @brief Rétablit la dernière action de navigation annulée
     * @return true si une action a été rétablie, false sinon
     */
    bool redoNavigation();

private:
    ViewManager& viewManager_;
    CommandManager& commandManager_;
};
