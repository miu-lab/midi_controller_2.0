#include "UIController.hpp"

#include <Arduino.h>

UIController::UIController(IViewManager& viewManager, MenuController& menuController)
    : viewManager_(viewManager),
      menuController_(menuController),
      currentState_(UIState::MAIN_SCREEN) {}

void UIController::navigateToMainScreen() {
    // Si nous sommes dans un menu, sortir du menu
    if (menuController_.isInMenu()) {
        menuController_.exitMenu();
    }

    // Demander au ViewManager d'afficher la vue par défaut (home)
    viewManager_.showHome();
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

    // Dans l'architecture simplifiée, pas de vue spécifique pour le moniteur
    // Retourner à la vue par défaut
    viewManager_.showHome();
    currentState_ = UIState::MAIN_SCREEN;
}

void UIController::navigateToDebugScreen() {
    // Si nous sommes dans un menu, sortir du menu
    if (menuController_.isInMenu()) {
        menuController_.exitMenu();
    }

    // Dans l'architecture simplifiée, pas de vue de débogage
    // Retourner à la vue par défaut
    viewManager_.showHome();
    currentState_ = UIState::MAIN_SCREEN;
}

void UIController::showModalDialog(const std::string& message, const std::string& okLabel,
                                   const std::string& cancelLabel) {
    // Dans une implémentation complète, nous passerions les labels au ViewManager
    // Pour cette démonstration, nous les ignorons

    // Demander au ViewManager d'afficher la boîte de dialogue
    viewManager_.showModal(message.c_str());
    currentState_ = UIState::MODAL_DIALOG;
}

void UIController::handleEncoderTurn(int8_t direction) {
    // Traiter la rotation d'un encodeur en fonction de l'état actuel
    switch (currentState_) {
    case UIState::MENU:
        // Dans un menu, sélectionner l'élément suivant ou précédent
        viewManager_.navigateMenu(direction);
        break;

    case UIState::CONTROL_MONITOR:
    case UIState::DEBUG_SCREEN:
        // Ces vues n'existent plus, traiter comme l'écran principal
        // Ne rien faire ou revenir au menu
        break;

    case UIState::MODAL_DIALOG:
        // Dans une boîte de dialogue, pour l'instant ne rien faire
        // Ou basculer entre les boutons si implémenté
        break;

    case UIState::MAIN_SCREEN:
    default:
        // Sur l'écran principal, ne rien faire pour l'instant
        // Les changements d'encodeur devraient déclencher des événements MIDI
        // qui seront gérés par ViewManagerEventListener
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
        viewManager_.hideModal();
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
        viewManager_.hideModal();
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
        viewManager_.hideModal();
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
