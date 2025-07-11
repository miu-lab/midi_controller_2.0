#pragma once

#include "BaseInputProcessor.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "config/SystemConstants.hpp"

/**
 * @brief Processor spécialisé pour les entrées MIDI
 * 
 * Gère le routage des entrées physiques vers les événements MIDI haute priorité.
 */
class MidiInputProcessor : public BaseInputProcessor {
public:
    MidiInputProcessor(std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                      std::shared_ptr<EventBus> eventBus)
        : BaseInputProcessor(unifiedConfig, eventBus) {}
    
    /**
     * @brief Traite un encodeur pour MIDI
     */
    void processEncoder(EncoderId id, int32_t absolutePosition, int8_t relativeChange) {
        if (!isValidContext()) {
            return;
        }
        
        // Validation des limites
        if (SystemConstants::Validation::STRICT_INPUT_VALIDATION) {
            if (!isValidRelativeChange(relativeChange) || !isValidAbsolutePosition(absolutePosition)) {
                return;
            }
        }
        
        HighPriorityEncoderChangedEvent event(id, absolutePosition, relativeChange);
        eventBus_->publish(event);
    }
    
    /**
     * @brief Traite un bouton pour MIDI
     */
    void processButton(ButtonId id, bool pressed) {
        if (!isValidContext()) {
            return;
        }
        
        HighPriorityButtonPressEvent event(id, pressed);
        eventBus_->publish(event);
    }

private:
    bool isValidRelativeChange(int8_t relativeChange) const {
        return relativeChange >= SystemConstants::Validation::MIN_RELATIVE_CHANGE &&
               relativeChange <= SystemConstants::Validation::MAX_RELATIVE_CHANGE;
    }
    
    bool isValidAbsolutePosition(int32_t absolutePosition) const {
        return absolutePosition >= SystemConstants::Validation::MIN_ABSOLUTE_POSITION && 
               absolutePosition <= SystemConstants::Validation::MAX_ABSOLUTE_POSITION;
    }
};