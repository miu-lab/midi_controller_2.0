#pragma once

#include <memory>
#include "processors/NavigationInputProcessor.hpp"
#include "processors/MidiInputProcessor.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "config/InputConstants.hpp"

/**
 * @brief Gestionnaire centralisé des processors d'input
 * 
 * Applique le Strategy Pattern pour déléguer le traitement des entrées
 * aux processors spécialisés appropriés (Navigation vs MIDI).
 */
class InputProcessorManager {
public:
    /**
     * @brief Constructeur
     * @param navigationConfig Service de configuration de navigation
     * @param unifiedConfig Configuration unifiée
     * @param eventBus Bus d'événements
     */
    InputProcessorManager(std::shared_ptr<NavigationConfigService> navigationConfig,
                         std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                         std::shared_ptr<EventBus> eventBus)
        : navigationConfig_(navigationConfig)
        , navigationProcessor_(std::make_unique<NavigationInputProcessor>(unifiedConfig, eventBus))
        , midiProcessor_(std::make_unique<MidiInputProcessor>(unifiedConfig, eventBus)) {}
    
    /**
     * @brief Traite la rotation d'un encodeur
     */
    void processEncoderTurn(EncoderId id, int32_t absolutePosition, int8_t relativeChange) {
        // Priorité 1: Vérifier NavigationConfigService
        if (navigationConfig_ && navigationConfig_->isNavigationControl(id)) {
            navigationProcessor_->processEncoder(id, absolutePosition, relativeChange);
            return;
        }
        
        // Priorité 2: Vérifier dans la configuration unifiée
        if (navigationProcessor_->processEncoder(id, absolutePosition, relativeChange)) {
            return;
        }
        
        // Fallback: Traiter comme MIDI
        midiProcessor_->processEncoder(id, absolutePosition, relativeChange);
    }
    
    /**
     * @brief Traite l'appui sur un bouton
     */
    void processButtonPress(ButtonId id, bool pressed) {
        // Priorité 1: Vérifier dans la configuration unifiée pour navigation
        if (navigationProcessor_->processButton(id, pressed)) {
            return;
        }
        
        // Fallback: Traiter comme MIDI
        midiProcessor_->processButton(id, pressed);
    }

private:
    std::shared_ptr<NavigationConfigService> navigationConfig_;
    std::unique_ptr<NavigationInputProcessor> navigationProcessor_;
    std::unique_ptr<MidiInputProcessor> midiProcessor_;
};