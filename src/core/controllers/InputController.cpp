#include "InputController.hpp"

InputController::InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                                 std::shared_ptr<OptimizedEventBus> eventBus)
    : m_navigationConfig(navigationConfig), eventBus_(eventBus) {
    // Initialiser les callbacks avec des fonctions vides pour éviter les nullptr
    auto emptyEncoderCallback = [](EncoderId, int32_t, int8_t) {};
    auto emptyEncoderButtonCallback = [](EncoderId, bool) {};
    auto emptyButtonCallback = [](ButtonId, bool) {};

    m_navigationEncoderCallback = emptyEncoderCallback;
    m_navigationEncoderButtonCallback = emptyEncoderButtonCallback;
    m_navigationButtonCallback = emptyButtonCallback;
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
    }
    // Les contrôles MIDI sont gérés via le bus d'événements
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
    }
    // Les contrôles MIDI sont gérés via le bus d'événements
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
    }
    // Les contrôles MIDI sont gérés via le bus d'événements
}

void InputController::setNavigationEncoderCallback(
    std::function<void(EncoderId, int32_t, int8_t)> callback) {
    m_navigationEncoderCallback = callback;
}

void InputController::setNavigationEncoderButtonCallback(
    std::function<void(EncoderId, bool)> callback) {
    m_navigationEncoderButtonCallback = callback;
}

void InputController::setNavigationButtonCallback(std::function<void(ButtonId, bool)> callback) {
    m_navigationButtonCallback = callback;
}