#pragma once

#include "BaseUIProcessor.hpp"
#include "config/SystemConstants.hpp"

/**
 * @brief Processor spécialisé pour le traitement des événements UI
 * 
 * Gère le cycle de traitement des événements avec timeout et optimisations.
 */
class EventUIProcessor : public BaseUIProcessor {
public:
    EventUIProcessor(std::shared_ptr<MidiController::Events::IEventBus> eventBus)
        : BaseUIProcessor(eventBus) {}
    
    /**
     * @brief Traite les événements UI
     */
    bool processEvents() {
        if (!isValidContext()) {
            return false;
        }
        
        if (!isWithinTimeout(SystemConstants::Timing::EVENT_PROCESSING_TIMEOUT_MS)) {
            return false;
        }
        
        processBaseEvents();
        return true;
    }
    
    /**
     * @brief Vérifie si le traitement d'événements est configuré
     */
    bool isEventProcessingEnabled() const {
        return SystemConstants::UI::DEFAULT_ENABLE_EVENT_PROCESSING;
    }

protected:
    bool isValidContext() const override {
        return BaseUIProcessor::isValidContext() && 
               SystemConstants::Validation::VALIDATE_EVENT_BUS_REQUIRED;
    }
};