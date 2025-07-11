#pragma once

#include "BaseUIProcessor.hpp"
#include "config/SystemConstants.hpp"

/**
 * @brief Processor spécialisé pour la gestion de l'affichage
 * 
 * Gère les opérations de rafraîchissement d'affichage avec optimisations.
 */
class DisplayUIProcessor : public BaseUIProcessor {
public:
    DisplayUIProcessor(std::shared_ptr<MidiController::Events::IEventBus> eventBus,
                      std::unique_ptr<IDisplayManager> displayManager)
        : BaseUIProcessor(eventBus, std::move(displayManager)) {}
    
    /**
     * @brief Rafraîchit l'affichage
     */
    bool refreshDisplay() {
        if (!isValidContext() || !isDisplayAvailable()) {
            return false;
        }
        
        if (!isDisplayRefreshEnabled()) {
            return true; // Pas d'erreur si désactivé
        }
        
        if (!isWithinTimeout(SystemConstants::Timing::DISPLAY_REFRESH_TIMEOUT_MS)) {
            return false;
        }
        
        updateDisplay();
        return true;
    }
    
    /**
     * @brief Vérifie si le rafraîchissement d'affichage est activé
     */
    bool isDisplayRefreshEnabled() const {
        return SystemConstants::UI::DEFAULT_ENABLE_DISPLAY_REFRESH;
    }

protected:
    bool isValidContext() const override {
        return BaseUIProcessor::isValidContext() && 
               (!isDisplayRefreshEnabled() || isDisplayAvailable());
    }
};