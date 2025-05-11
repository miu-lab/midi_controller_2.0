#include "core/controllers/InputController.hpp"

#include <Arduino.h>

#include "core/controllers/UIController.hpp"

InputController::InputController(NavigationConfigService& navigationConfig)
    : navigationConfig_(navigationConfig), uiController_(nullptr) {
    // Initialiser les callbacks avec des fonctions vides pour éviter les nullptr
    auto emptyEncoderCallback = [](EncoderId, int32_t, int8_t) {};
    auto emptyEncoderButtonCallback = [](EncoderId, bool) {};
    auto emptyButtonCallback = [](ButtonId, bool) {};

    navigationEncoderCallback_ = emptyEncoderCallback;
    midiEncoderCallback_ = emptyEncoderCallback;
    navigationEncoderButtonCallback_ = emptyEncoderButtonCallback;
    midiEncoderButtonCallback_ = emptyEncoderButtonCallback;
    navigationButtonCallback_ = emptyButtonCallback;
    midiButtonCallback_ = emptyButtonCallback;
}

void InputController::setUIController(std::shared_ptr<UIController> uiController) {
    uiController_ = uiController;

    // Si l'UIController est défini, nous pouvons configurer les callbacks par défaut
    // pour utiliser l'UIController au lieu de l'EventBus
    if (uiController_) {
        navigationEncoderCallback_ = [this](EncoderId id, int32_t absPos, int8_t relChange) {
            handleNavigationEncoderTurn(id, absPos, relChange);
        };

        navigationEncoderButtonCallback_ = [this](EncoderId id, bool pressed) {
            handleNavigationEncoderButton(id, pressed);
        };

        navigationButtonCallback_ = [this](ButtonId id, bool pressed) {
            handleNavigationButton(id, pressed);
        };
    }
}

void InputController::setUIController(UIController* uiController) {
    // Méthode dépréciée - convertir le pointeur brut en shared_ptr avec un deleter spécial
    // qui ne détruit pas l'objet (puisque nous ne sommes pas propriétaires)
    if (uiController) {
        setUIController(std::shared_ptr<UIController>(uiController, [](UIController*) {}));
    } else {
        uiController_ = nullptr;
    }
}

void InputController::processEncoderTurn(EncoderId id, int32_t absolutePosition,
                                         int8_t relativeChange) {
    // Déterminer si cet encodeur est dédié à la navigation
    if (navigationConfig_.isNavigationControl(id)) {
        navigationEncoderCallback_(id, absolutePosition, relativeChange);
    } else {
        midiEncoderCallback_(id, absolutePosition, relativeChange);
    }
}

void InputController::processEncoderButton(EncoderId id, bool pressed) {
    // Déterminer si ce bouton d'encodeur est dédié à la navigation
    if (navigationConfig_.isNavigationControl(id)) {
        navigationEncoderButtonCallback_(id, pressed);
    } else {
        midiEncoderButtonCallback_(id, pressed);
    }
}

void InputController::processButtonPress(ButtonId id, bool pressed) {
    // Déterminer si ce bouton est dédié à la navigation
    if (navigationConfig_.isNavigationControl(id)) {
        navigationButtonCallback_(id, pressed);
    } else {
        midiButtonCallback_(id, pressed);
    }
}

void InputController::setNavigationEncoderCallback(
    std::function<void(EncoderId, int32_t, int8_t)> callback) {
    navigationEncoderCallback_ = callback;
}

void InputController::setMidiEncoderCallback(
    std::function<void(EncoderId, int32_t, int8_t)> callback) {
    midiEncoderCallback_ = callback;
}

void InputController::setNavigationEncoderButtonCallback(
    std::function<void(EncoderId, bool)> callback) {
    navigationEncoderButtonCallback_ = callback;
}

void InputController::setMidiEncoderButtonCallback(std::function<void(EncoderId, bool)> callback) {
    midiEncoderButtonCallback_ = callback;
}

void InputController::setNavigationButtonCallback(std::function<void(ButtonId, bool)> callback) {
    navigationButtonCallback_ = callback;
}

void InputController::setMidiButtonCallback(std::function<void(ButtonId, bool)> callback) {
    midiButtonCallback_ = callback;
}

// Méthodes privées pour traiter les entrées via l'UIController

void InputController::handleNavigationEncoderTurn(EncoderId id, int32_t absolutePosition,
                                                  int8_t relativeChange) {
    // Vérifier que l'UIController est disponible
    if (!uiController_) return;

    // Simplification: supposons que l'encodeur avec ID 79 est celui de navigation
    // Dans une implémentation réelle, cette correspondance pourrait être configurable
    if (id == 79) {
        // Appeler la méthode appropriée de l'UIController
        uiController_->handleEncoderTurn(relativeChange);
    }
}

void InputController::handleNavigationEncoderButton(EncoderId id, bool pressed) {
    // Vérifier que l'UIController est disponible
    if (!uiController_) return;

    // Ne traiter que les appuis (pas les relâchements)
    if (!pressed) return;

    // Simplification: supposons que l'encodeur avec ID 79 est celui de navigation
    if (id == 79) {
        // Appeler la méthode appropriée de l'UIController
        uiController_->handleEncoderClick();
    }
}

void InputController::handleNavigationButton(ButtonId id, bool pressed) {
    // Vérifier que l'UIController est disponible
    if (!uiController_) return;

    // Ne traiter que les appuis (pas les relâchements)
    if (!pressed) return;

    // Simplification: supposons que les boutons avec ID 51 et 52 sont pour la navigation
    // Dans une implémentation réelle, cette correspondance pourrait être configurable
    switch (id) {
    case 51:  // Bouton Retour
        uiController_->handleBackButton();
        break;
    case 52:  // Bouton OK
        uiController_->handleOkButton();
        break;
    default:
        // Autres boutons non gérés par l'UI
        break;
    }
}
