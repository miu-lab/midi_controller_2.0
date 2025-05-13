#include "UIController.hpp"

#include <Arduino.h>

UIController::UIController(ViewManager& viewManager, MenuController& menuController)
    : viewManager_(viewManager),
      menuController_(menuController),
      currentState_(UIState::MAIN_SCREEN) {}

void UIController::navigateToMainScreen() {
    // Si nous sommes dans un menu, sortir du menu
    if (menuController_.isInMenu()) {
        menuController_.exitMenu();
    }

    // Demander au ViewManager d'afficher l'écran principal
    viewManager_.showMainScreen();
    currentState_ = UIState::MAIN_SCREEN;
}

void UIController::navigateToMenu() {
    // Entrer dans le menu via le MenuController
    menuController_.enterMenu();
    currentState_ = UIState::MENU;
}

void UIController::navigateToControlMonitor() {
    // Si nous sommes dans un menu, sortir du menu
    if (menuController_.isInMenu()) {
        menuController_.exitMenu();
    }

    // Demander au ViewManager d'afficher le moniteur de contrôles
    viewManager_.showControlMonitor();
    currentState_ = UIState::CONTROL_MONITOR;
}

void UIController::navigateToDebugScreen() {
    // Si nous sommes dans un menu, sortir du menu
    if (menuController_.isInMenu()) {
        menuController_.exitMenu();
    }

    // Demander au ViewManager d'afficher l'écran de débogage
    viewManager_.showDebugScreen();
    currentState_ = UIState::DEBUG_SCREEN;
}

void UIController::showModalDialog(const String& message, const String& okLabel,
                                   const String& cancelLabel) {
    // Dans une implémentation complète, nous passerions les labels au ViewManager
    // Pour cette démonstration, nous les ignorons

    // Demander au ViewManager d'afficher la boîte de dialogue
    viewManager_.showModalDialog(message);
    currentState_ = UIState::MODAL_DIALOG;
}

void UIController::handleEncoderTurn(int8_t direction) {
    // Traiter la rotation d'un encodeur en fonction de l'état actuel
    switch (currentState_) {
    case UIState::MENU:
        // Dans un menu, sélectionner l'élément suivant ou précédent
        if (direction > 0) {
            menuController_.selectNextItem();
        } else {
            menuController_.selectPreviousItem();
        }
        break;

    case UIState::CONTROL_MONITOR:
        // Dans le moniteur de contrôles, navigation spécifique
        // Par exemple, faire défiler la liste des contrôles
        viewManager_.scrollControlMonitorByDelta(direction);
        break;

    case UIState::DEBUG_SCREEN:
        // Dans l'écran de débogage, faire défiler les logs
        viewManager_.scrollDebugLogByDelta(direction);
        break;

    case UIState::MODAL_DIALOG:
        // Dans une boîte de dialogue, basculer entre OK et Annuler
        viewManager_.toggleModalDialogButton();
        break;

    case UIState::MAIN_SCREEN:
    default:
        // Sur l'écran principal, comportement spécifique
        // Par exemple, faire défiler les informations
        viewManager_.scrollMainScreenByDelta(direction);
        break;
    }
}

void UIController::handleEncoderClick() {
    // Traiter le clic sur un encodeur en fonction de l'état actuel
    switch (currentState_) {
    case UIState::MAIN_SCREEN: {
        // Sur l'écran principal, aller au menu
        navigateToMenu();
        break;
    }

    case UIState::MENU: {
        // Dans un menu, sélectionner l'élément actuel
        int index = menuController_.getCurrentItem();
        if (index >= 0) {
            // Traiter la sélection en fonction de l'élément sélectionné
            // C'est une simplification, le code réel dépendrait des éléments de menu
            switch (index) {
            case 0:  // Premier élément (par exemple "Contrôle")
                navigateToControlMonitor();
                break;
            case 1:  // Deuxième élément (par exemple "Debug")
                navigateToDebugScreen();
                break;
            default:
                // Par défaut, rester dans le menu
                break;
            }
        }
        break;
    }

    case UIState::CONTROL_MONITOR:
    case UIState::DEBUG_SCREEN: {
        // Sur ces écrans, revenir au menu
        navigateToMenu();
        break;
    }

    case UIState::MODAL_DIALOG: {
        // Dans une boîte de dialogue, confirmer la sélection actuelle

        // Dans une implémentation complète, nous vérifierions quelle option est sélectionnée
        // et nous appellerions les callbacks appropriés
        // Pour cette démonstration, nous nous contentons de fermer la boîte de dialogue

        // Fermer la boîte de dialogue et revenir à l'écran précédent
        viewManager_.hideModalDialog();
        // Revenir à l'état précédent, supposons MAIN_SCREEN pour simplifier
        currentState_ = UIState::MAIN_SCREEN;
        break;
    }
    }
}

void UIController::handleBackButton() {
    // Traiter l'appui sur le bouton retour en fonction de l'état actuel
    switch (currentState_) {
    case UIState::MENU: {
        // Dans un menu, revenir à l'écran principal
        navigateToMainScreen();
        break;
    }

    case UIState::CONTROL_MONITOR:
    case UIState::DEBUG_SCREEN: {
        // Sur ces écrans, revenir à l'écran principal
        navigateToMainScreen();
        break;
    }

    case UIState::MODAL_DIALOG: {
        // Dans une boîte de dialogue, annuler

        // Dans une implémentation complète, nous appellerions un callback ici
        // Pour cette démonstration, nous nous contentons de fermer la boîte de dialogue

        // Fermer la boîte de dialogue et revenir à l'écran précédent
        viewManager_.hideModalDialog();
        // Revenir à l'état précédent, supposons MAIN_SCREEN pour simplifier
        currentState_ = UIState::MAIN_SCREEN;
        break;
    }

    case UIState::MAIN_SCREEN:
    default: {
        // Sur l'écran principal, peut-être afficher un message d'aide
        // ou ne rien faire
        break;
    }
    }
}

void UIController::handleOkButton() {
    // Traiter l'appui sur le bouton OK en fonction de l'état actuel
    switch (currentState_) {
    case UIState::MAIN_SCREEN: {
        // Sur l'écran principal, aller au menu
        navigateToMenu();
        break;
    }

    case UIState::MENU: {
        // Dans un menu, même comportement que handleEncoderClick
        handleEncoderClick();
        break;
    }

    case UIState::MODAL_DIALOG: {
        // Dans une boîte de dialogue, confirmer

        // Dans une implémentation complète, nous appellerions un callback ici
        // Pour cette démonstration, nous nous contentons de fermer la boîte de dialogue

        // Fermer la boîte de dialogue et revenir à l'écran précédent
        viewManager_.hideModalDialog();
        // Revenir à l'état précédent, supposons MAIN_SCREEN pour simplifier
        currentState_ = UIState::MAIN_SCREEN;
        break;
    }

    case UIState::CONTROL_MONITOR:
    case UIState::DEBUG_SCREEN:
    default: {
        // Sur ces écrans, aucune action spécifique pour OK
        break;
    }
    }
}

UIController::UIState UIController::getCurrentState() const {
    return currentState_;
}
