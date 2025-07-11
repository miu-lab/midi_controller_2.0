#pragma once

#include <memory>

#include "app/services/NavigationConfigService.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/types.hpp"
#include "InputProcessorManager.hpp"

/**
 * @brief Contrôleur dédié à la gestion des entrées physiques
 *
 * Ce contrôleur route les entrées physiques vers :
 * - Événements de navigation pour les contrôles configurés comme navigation
 * - Événements MIDI pour les contrôles configurés comme MIDI
 */
class InputController {
public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param navigationConfig Service de configuration de navigation
     * @param unifiedConfig Configuration unifiée des contrôles
     * @param eventBus Bus d'événements
     */
    InputController(std::shared_ptr<NavigationConfigService> navigationConfig,
                    std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                    std::shared_ptr<EventBus> eventBus);

    /**
     * @brief Traite la rotation d'un encodeur
     * @param id Identifiant de l'encodeur
     * @param absolutePosition Position absolue de l'encodeur
     * @param relativeChange Changement relatif de position
     */
    void processEncoderTurn(EncoderId id, int32_t absolutePosition, int8_t relativeChange);


    /**
     * @brief Traite l'appui sur un bouton
     * @param id Identifiant du bouton
     * @param pressed État du bouton (true = pressé, false = relâché)
     */
    void processButtonPress(ButtonId id, bool pressed);

private:
    // === SYSTÈME DE PROCESSORS ===
    std::unique_ptr<InputProcessorManager> processorManager_;
};