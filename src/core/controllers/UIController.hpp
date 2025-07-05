#pragma once

#include <functional>

#include "adapters/ui/views/ViewManager.hpp"
#include "MenuController.hpp"
#include "core/domain/types.hpp"

/**
 * @brief Contrôleur dédié à la gestion de l'interface utilisateur
 *
 * Ce contrôleur est responsable de la coordination des interactions utilisateur avec l'interface.
 * Il gère la navigation entre les différentes vues et les interactions avec ces vues.
 */
class UIController {
public:
    /**
     * @brief Constructeur
     * @param viewManager Gestionnaire de vues
     * @param menuController Contrôleur de menu
     */
    UIController(ViewManager& viewManager, MenuController& menuController);

    /**
     * @brief États possibles de l'interface utilisateur
     */
    enum class UIState {
        MAIN_SCREEN,      // Écran principal
        MENU,             // Menu
        CONTROL_MONITOR,  // Moniteur de contrôles
        DEBUG_SCREEN,     // Écran de débogage
        MODAL_DIALOG      // Boîte de dialogue modale
    };

    /**
     * @brief Navigation vers l'écran principal
     */
    void navigateToMainScreen();

    /**
     * @brief Navigation vers le menu
     */
    void navigateToMenu();

    /**
     * @brief Navigation vers le moniteur de contrôles
     */
    void navigateToControlMonitor();

    /**
     * @brief Navigation vers l'écran de débogage
     */
    void navigateToDebugScreen();

    /**
     * @brief Affiche une boîte de dialogue modale
     * @param message Message à afficher
     * @param okLabel Label du bouton OK (optionnel)
     * @param cancelLabel Label du bouton Annuler (optionnel)
     */
    void showModalDialog(const String& message, const String& okLabel = "OK",
                         const String& cancelLabel = "Annuler");

    /**
     * @brief Gestion de la rotation d'un encodeur de navigation
     * @param direction Direction de rotation (positive ou négative)
     */
    void handleEncoderTurn(int8_t direction);

    /**
     * @brief Gestion du clic sur un encodeur de navigation
     */
    void handleEncoderClick();

    /**
     * @brief Gestion de l'appui sur le bouton retour
     */
    void handleBackButton();

    /**
     * @brief Gestion de l'appui sur le bouton OK
     */
    void handleOkButton();

    /**
     * @brief Obtient l'état actuel de l'interface utilisateur
     * @return État actuel de l'UI
     */
    UIState getCurrentState() const;

private:
    ViewManager& viewManager_;
    MenuController& menuController_;
    UIState currentState_;
};
