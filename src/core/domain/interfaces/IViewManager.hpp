#pragma once

/**
 * @brief Interface pour la gestion des vues
 * 
 * Cette interface définit les opérations pour naviguer entre les vues
 * et gérer l'affichage du menu dans l'interface utilisateur.
 */
class IViewManager {
public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~IViewManager() = default;

    /**
     * @brief Affiche le menu principal
     */
    virtual void showMenu() = 0;

    /**
     * @brief Affiche la vue d'accueil
     */
    virtual void showHome() = 0;

    /**
     * @brief Navigue dans le menu
     * @param direction Direction de navigation (positif = suivant, négatif = précédent)
     */
    virtual void navigateMenu(int direction) = 0;

    /**
     * @brief Sélectionne l'élément de menu actuel
     */
    virtual void selectMenuItem() = 0;

    /**
     * @brief Retourne à la page racine du menu
     */
    virtual void goBackToMenuRoot() = 0;

    /**
     * @brief Remonte d'un niveau dans la hiérarchie de menu
     */
    virtual void goBackOneLevel() = 0;

    /**
     * @brief Affiche un modal avec un message
     * @param message Message à afficher
     */
    virtual void showModal(const char* message) = 0;

    /**
     * @brief Cache le modal actuel
     */
    virtual void hideModal() = 0;

    /**
     * @brief Met à jour le gestionnaire de vues
     */
    virtual void update() = 0;

    /**
     * @brief Initialise le gestionnaire de vues
     * @return true si l'initialisation réussit
     */
    virtual bool init() = 0;
};