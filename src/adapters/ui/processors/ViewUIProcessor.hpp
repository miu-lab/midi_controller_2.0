#pragma once

#include "BaseUIProcessor.hpp"
#include "adapters/ui/views/ViewManager.hpp"
#include "config/UISystemConstants.hpp"

/**
 * @brief Processor spécialisé pour la gestion des vues UI
 * 
 * Gère les opérations sur ViewManager avec validation et timeout.
 */
class ViewUIProcessor : public BaseUIProcessor {
public:
    ViewUIProcessor(std::shared_ptr<MidiController::Events::IEventBus> eventBus,
                   std::shared_ptr<ViewManager> viewManager)
        : BaseUIProcessor(eventBus), viewManager_(viewManager) {}
    
    /**
     * @brief Met à jour le gestionnaire de vues
     */
    bool updateViewManager() {
        if (!isValidContext() || !viewManager_) {
            return false;
        }
        
        if (!isWithinTimeout(UISystemConstants::Timing::VIEW_MANAGER_TIMEOUT_MS)) {
            return false;
        }
        
        viewManager_->update();
        return true;
    }
    
    /**
     * @brief Affiche un modal
     */
    bool showModal(const std::string& message) {
        if (!isValidContext() || !viewManager_) {
            return false;
        }
        
        if (message.length() > UISystemConstants::Modal::MAX_MESSAGE_LENGTH) {
            return false;
        }
        
        viewManager_->showModal(message.c_str());
        return true;
    }
    
    /**
     * @brief Cache le modal
     */
    bool hideModal() {
        if (!isValidContext() || !viewManager_) {
            return false;
        }
        
        viewManager_->hideModal();
        return true;
    }
    
    /**
     * @brief Obtient le ViewManager
     */
    std::shared_ptr<ViewManager> getViewManager() const {
        return viewManager_;
    }

protected:
    bool isValidContext() const override {
        return BaseUIProcessor::isValidContext() && viewManager_ != nullptr;
    }

private:
    std::shared_ptr<ViewManager> viewManager_;
};