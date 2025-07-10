#pragma once

#include <functional>
#include <memory>

#include "app/services/NavigationConfigService.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/events/core/EventTypes.hpp"
#include "core/domain/navigation/NavigationEvent.hpp"
#include "core/domain/types.hpp"

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
    // === DÉPENDANCES ===
    std::shared_ptr<NavigationConfigService> navigationConfig_;
    std::shared_ptr<UnifiedConfiguration> unifiedConfig_;
    std::shared_ptr<EventBus> eventBus_;
    
    // === MÉTHODES PRIVÉES ===
    
    /**
     * @brief Traite un input de navigation
     */
    void handleNavigationInput(InputId id, bool isPressed = true, int8_t relativeChange = 0);
    
    /**
     * @brief Extrait l'action de navigation depuis la configuration
     */
    NavigationAction extractNavigationAction(const ControlDefinition& control, bool isEncoder) const;
    
    /**
     * @brief Détermine le paramètre pour l'action de navigation
     */
    int determineNavigationParameter(const ControlDefinition& control, int8_t relativeChange) const;
    
    /**
     * @brief Émet un événement de navigation
     */
    void emitNavigationEvent(NavigationAction action, int parameter = 0);
    
    /**
     * @brief Émet un événement MIDI haute priorité
     */
    void emitMidiEvent(InputId id, bool isEncoder, int32_t absolutePosition = 0, int8_t relativeChange = 0, bool pressed = false);
    
    /**
     * @brief Vérifie si un contrôle est configuré pour la navigation
     */
    bool isNavigationControlInConfig(InputId id) const;
};