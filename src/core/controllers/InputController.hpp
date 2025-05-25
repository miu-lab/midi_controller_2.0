#pragma once

#include <functional>
#include <memory>

#include "app/services/NavigationConfigService.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include "core/domain/events/core/OptimizedEventBus.hpp"
#include "core/domain/types.hpp"

// Forward declaration
class UIController;

/**
 * @brief Contrôleur dédié à la gestion des entrées physiques
 *
 * Ce contrôleur est responsable du routage des entrées physiques (encodeurs, boutons)
 * vers les contrôleurs appropriés en fonction de la configuration.
 * 
 * Architecture:
 * - Les contrôles de navigation utilisent des callbacks directs
 * - Les contrôles MIDI utilisent le bus d'événements haute priorité (OptimizedEventBus)
 * - Cette séparation permet d'optimiser les performances pour les événements MIDI critiques
 */
class InputController {
public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param navigationConfig Service de configuration de navigation
     * @param eventBus Bus d'événements optimisé
     */
    InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                    std::shared_ptr<OptimizedEventBus> eventBus = nullptr);

    /**
     * @brief Traite la rotation d'un encodeur
     * @param id Identifiant de l'encodeur
     * @param absolutePosition Position absolue de l'encodeur
     * @param relativeChange Changement relatif de position
     */
    void processEncoderTurn(EncoderId id, int32_t absolutePosition, int8_t relativeChange);

    /**
     * @brief Traite l'appui sur le bouton d'un encodeur
     * @param id Identifiant de l'encodeur
     * @param pressed État du bouton (true = pressé, false = relâché)
     */
    void processEncoderButton(EncoderId id, bool pressed);

    /**
     * @brief Traite l'appui sur un bouton
     * @param id Identifiant du bouton
     * @param pressed État du bouton (true = pressé, false = relâché)
     */
    void processButtonPress(ButtonId id, bool pressed);

    /**
     * @brief Définit le callback pour les encodeurs de navigation
     * @param callback Fonction à appeler lors de la rotation d'un encodeur de navigation
     */
    void setNavigationEncoderCallback(std::function<void(EncoderId, int32_t, int8_t)> callback);

    /**
     * @brief Définit le callback pour les boutons d'encodeur de navigation
     * @param callback Fonction à appeler lors de l'appui sur un bouton d'encodeur de navigation
     */
    void setNavigationEncoderButtonCallback(std::function<void(EncoderId, bool)> callback);

    /**
     * @brief Définit le callback pour les boutons de navigation
     * @param callback Fonction à appeler lors de l'appui sur un bouton de navigation
     */
    void setNavigationButtonCallback(std::function<void(ButtonId, bool)> callback);

private:
    std::shared_ptr<NavigationConfigService> m_navigationConfig;
    std::shared_ptr<OptimizedEventBus> eventBus_;  // Bus d'événements optimisé

    // Stockage des callbacks de navigation uniquement
    std::function<void(EncoderId, int32_t, int8_t)> m_navigationEncoderCallback;
    std::function<void(EncoderId, bool)> m_navigationEncoderButtonCallback;
    std::function<void(ButtonId, bool)> m_navigationButtonCallback;
};