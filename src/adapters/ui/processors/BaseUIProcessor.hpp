#pragma once

#include <memory>
#include "core/domain/interfaces/IDisplayManager.hpp"
#include "core/domain/events/core/IEventBus.hpp"
#include "config/UISystemConstants.hpp"

namespace MidiController::Events {
    class IEventBus;
}

/**
 * @brief Classe de base pour tous les processors UI
 * 
 * Applique le Template Method Pattern pour factoriser la logique commune
 * de validation et de traitement UI.
 */
class BaseUIProcessor {
public:
    /**
     * @brief Constructeur
     * @param eventBus Bus d'événements
     * @param displayManager Gestionnaire d'affichage (optionnel)
     */
    BaseUIProcessor(std::shared_ptr<MidiController::Events::IEventBus> eventBus,
                   std::unique_ptr<IDisplayManager> displayManager = nullptr)
        : eventBus_(eventBus), displayManager_(std::move(displayManager)) {}
    
    /**
     * @brief Destructeur virtuel
     */
    virtual ~BaseUIProcessor() = default;

protected:
    /**
     * @brief Vérifie si le contexte est valide (Template Method)
     */
    virtual bool isValidContext() const {
        return eventBus_ != nullptr;
    }
    
    /**
     * @brief Vérifie si l'affichage est disponible
     */
    bool isDisplayAvailable() const {
        return displayManager_ != nullptr;
    }
    
    /**
     * @brief Valide les timeouts d'opération
     */
    bool isWithinTimeout(uint32_t timeoutMs) const {
        // Implémentation simplifiée - pourrait être étendue avec chronométrage réel
        return timeoutMs <= UISystemConstants::Timing::UPDATE_INTERVAL_MS * 5;
    }
    
    /**
     * @brief Traite les événements de base
     */
    virtual void processBaseEvents() {
        if (eventBus_ && UISystemConstants::Validation::VALIDATE_EVENT_BUS_REQUIRED) {
            eventBus_->update();
        }
    }
    
    /**
     * @brief Met à jour l'affichage si disponible
     */
    virtual void updateDisplay() {
        if (displayManager_) {
            displayManager_->update();
        }
    }

protected:
    std::shared_ptr<MidiController::Events::IEventBus> eventBus_;
    std::unique_ptr<IDisplayManager> displayManager_;
};