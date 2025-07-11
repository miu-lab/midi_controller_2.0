#pragma once

#include <memory>
#include "processors/EventUIProcessor.hpp"
#include "processors/ViewUIProcessor.hpp"
#include "processors/DisplayUIProcessor.hpp"
#include "config/UISystemConstants.hpp"

/**
 * @brief Gestionnaire centralisé des processors UI
 * 
 * Applique le Strategy Pattern pour coordonner les processors spécialisés
 * selon l'ordre de priorité défini dans les constantes.
 */
class UIProcessorManager {
public:
    /**
     * @brief Constructeur
     * @param viewManager Gestionnaire de vues
     * @param displayManager Gestionnaire d'affichage
     * @param eventBus Bus d'événements
     */
    UIProcessorManager(std::shared_ptr<ViewManager> viewManager,
                      std::unique_ptr<IDisplayManager> displayManager,
                      std::shared_ptr<MidiController::Events::IEventBus> eventBus)
        : eventProcessor_(std::make_unique<EventUIProcessor>(eventBus))
        , viewProcessor_(std::make_unique<ViewUIProcessor>(eventBus, viewManager))
        , displayProcessor_(std::make_unique<DisplayUIProcessor>(eventBus, std::move(displayManager))) {}
    
    /**
     * @brief Exécute le cycle de mise à jour complet
     */
    void executeUpdateCycle() {
        // Ordre de priorité selon UISystemConstants::Priority::UpdateOrder
        
        // 1. EVENTS - Traiter les événements en premier
        if (eventProcessor_ && eventProcessor_->isEventProcessingEnabled()) {
            eventProcessor_->processEvents();
        }
        
        // 2. VIEW_MANAGER - Puis mettre à jour les vues
        if (viewProcessor_) {
            viewProcessor_->updateViewManager();
        }
        
        // 3. DISPLAY - Enfin rafraîchir l'affichage
        if (displayProcessor_ && displayProcessor_->isDisplayRefreshEnabled()) {
            displayProcessor_->refreshDisplay();
        }
    }
    
    /**
     * @brief Affiche un message modal
     */
    bool showMessage(const std::string& message) {
        if (!viewProcessor_) {
            return false;
        }
        return viewProcessor_->showModal(message);
    }
    
    /**
     * @brief Cache l'affichage modal
     */
    bool clearDisplay() {
        if (!viewProcessor_) {
            return false;
        }
        return viewProcessor_->hideModal();
    }
    
    /**
     * @brief Obtient le ViewManager via le processor
     */
    std::shared_ptr<ViewManager> getViewManager() const {
        if (!viewProcessor_) {
            return nullptr;
        }
        return viewProcessor_->getViewManager();
    }
    
    /**
     * @brief Vérifie si tous les processors sont opérationnels
     */
    bool areProcessorsOperational() const {
        bool eventOk = !eventProcessor_ || eventProcessor_->isEventProcessingEnabled();
        bool viewOk = viewProcessor_ != nullptr;
        bool displayOk = !displayProcessor_ || displayProcessor_->isDisplayRefreshEnabled();
        
        return eventOk && viewOk && displayOk;
    }

private:
    std::unique_ptr<EventUIProcessor> eventProcessor_;
    std::unique_ptr<ViewUIProcessor> viewProcessor_;
    std::unique_ptr<DisplayUIProcessor> displayProcessor_;
};