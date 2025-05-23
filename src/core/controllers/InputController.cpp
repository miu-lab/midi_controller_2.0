#include "InputController.hpp"

#include "UIController.hpp"

InputController::InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                                 std::shared_ptr<OptimizedEventBus> eventBus)
    : m_navigationConfig(navigationConfig), m_uiController(nullptr), eventBus_(eventBus) {
    // Initialiser les callbacks avec des fonctions vides pour éviter les nullptr
    auto emptyEncoderCallback = [](EncoderId, int32_t, int8_t) {};
    auto emptyEncoderButtonCallback = [](EncoderId, bool) {};
    auto emptyButtonCallback = [](ButtonId, bool) {};

    m_navigationEncoderCallback = emptyEncoderCallback;
    m_midiEncoderCallback = emptyEncoderCallback;
    m_navigationEncoderButtonCallback = emptyEncoderButtonCallback;
    m_midiEncoderButtonCallback = emptyEncoderButtonCallback;
    m_navigationButtonCallback = emptyButtonCallback;
    m_midiButtonCallback = emptyButtonCallback;
}

void InputController::processEncoderTurn(EncoderId id, int32_t absolutePosition,
                                         int8_t relativeChange) {
    // Chemin événementiel optimisé pour les contrôles MIDI
    if (eventBus_ && !m_navigationConfig->isNavigationControl(id)) {
        // Créer un événement haute priorité pour le chemin critique MIDI
        HighPriorityEncoderChangedEvent event(id, absolutePosition, relativeChange);
        eventBus_->publishHighPriority(event);
    }

    // Déterminer si cet encodeur est dédié à la navigation
    if (m_navigationConfig->isNavigationControl(id)) {
        m_navigationEncoderCallback(id, absolutePosition, relativeChange);
    } else {
        // m_midiEncoderCallback(id, absolutePosition, relativeChange);
    }
}

void InputController::processEncoderButton(EncoderId id, bool pressed) {
    // Chemin événementiel optimisé pour les contrôles MIDI
    if (eventBus_ && !m_navigationConfig->isNavigationControl(id)) {
        // Créer un événement haute priorité pour le chemin critique MIDI
        HighPriorityEncoderButtonEvent event(id, pressed);
        eventBus_->publishHighPriority(event);
    }

    // Déterminer si ce bouton d'encodeur est dédié à la navigation
    if (m_navigationConfig->isNavigationControl(id)) {
        m_navigationEncoderButtonCallback(id, pressed);
    } else {
        // m_midiEncoderButtonCallback(id, pressed);
    }
}

void InputController::processButtonPress(ButtonId id, bool pressed) {
    // Chemin événementiel optimisé pour les contrôles MIDI
    if (eventBus_ && !m_navigationConfig->isNavigationControl(id)) {
        // Créer un événement haute priorité pour le chemin critique MIDI
        HighPriorityButtonPressEvent event(id, pressed);
        eventBus_->publishHighPriority(event);
    }

    // Déterminer si ce bouton est dédié à la navigation
    if (m_navigationConfig->isNavigationControl(id)) {
        m_navigationButtonCallback(id, pressed);
    } else {
        // m_midiButtonCallback(id, pressed);
    }
}

void InputController::setNavigationEncoderCallback(
    std::function<void(EncoderId, int32_t, int8_t)> callback) {
    m_navigationEncoderCallback = callback;
}

void InputController::setMidiEncoderCallback(
    std::function<void(EncoderId, int32_t, int8_t)> callback) {
    m_midiEncoderCallback = callback;
}

void InputController::setNavigationEncoderButtonCallback(
    std::function<void(EncoderId, bool)> callback) {
    m_navigationEncoderButtonCallback = callback;
}

void InputController::setMidiEncoderButtonCallback(std::function<void(EncoderId, bool)> callback) {
    m_midiEncoderButtonCallback = callback;
}

void InputController::setNavigationButtonCallback(std::function<void(ButtonId, bool)> callback) {
    m_navigationButtonCallback = callback;
}

void InputController::setMidiButtonCallback(std::function<void(ButtonId, bool)> callback) {
    m_midiButtonCallback = callback;
}

// Méthodes privées pour traiter les entrées via l'UIController

void InputController::handleNavigationEncoderTurn(EncoderId id, int32_t absolutePosition,
                                                  int8_t relativeChange) {
    // Vérifier que l'UIController est disponible
    if (!m_uiController) {
        return;
    }

    // Simplification: supposons que l'encodeur avec ID 79 est celui de navigation
    // Dans une implémentation réelle, cette correspondance pourrait être configurable
    if (id == 79) {
        // Appeler la méthode appropriée de l'UIController
        m_uiController->handleEncoderTurn(relativeChange);
    }
}

void InputController::handleNavigationEncoderButton(EncoderId id, bool pressed) {
    // Vérifier que l'UIController est disponible
    if (!m_uiController) {
        return;
    }

    // Ne traiter que les appuis (pas les relâchements)
    if (!pressed) {
        return;
    }

    // Simplification: supposons que l'encodeur avec ID 79 est celui de navigation
    if (id == 79) {
        // Appeler la méthode appropriée de l'UIController
        m_uiController->handleEncoderClick();
    }
}

void InputController::handleNavigationButton(ButtonId id, bool pressed) {
    // Vérifier que l'UIController est disponible
    if (!m_uiController) {
        return;
    }

    // Ne traiter que les appuis (pas les relâchements)
    if (!pressed) {
        return;
    }

    // Simplification: supposons que les boutons avec ID 51 et 52 sont pour la navigation
    // Dans une implémentation réelle, cette correspondance pourrait être configurable
    switch (id) {
    case 51:  // Bouton Retour
        m_uiController->handleBackButton();
        break;
    case 52:  // Bouton OK
        m_uiController->handleOkButton();
        break;
    default:
        // Autres boutons non gérés par l'UI
        break;
    }
}