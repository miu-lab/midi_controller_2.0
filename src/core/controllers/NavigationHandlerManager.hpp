#pragma once

#include <memory>
#include <etl/vector.h>

#include "handlers/BaseNavigationHandler.hpp"
#include "handlers/SpecialActionHandler.hpp"
#include "handlers/MenuNavigationHandler.hpp"
#include "handlers/ParameterActionHandler.hpp"
#include "handlers/ContextualActionHandler.hpp"
#include "core/domain/navigation/NavigationStateManager.hpp"
#include "config/SystemConstants.hpp"

/**
 * @brief Gestionnaire centralisé des handlers de navigation
 * 
 * Applique le Strategy Pattern pour déléguer les actions
 * aux handlers spécialisés appropriés.
 */
class NavigationHandlerManager {
public:
    /**
     * @brief Constructeur
     * @param stateManager Gestionnaire d'état partagé
     */
    explicit NavigationHandlerManager(std::shared_ptr<NavigationStateManager> stateManager)
        : stateManager_(stateManager) {
        initializeHandlers();
    }
    
    /**
     * @brief Traite une action de navigation
     * @param action Action à traiter
     * @param parameter Paramètre optionnel
     * @return true si l'action a été traitée par un handler
     */
    bool handleAction(NavigationAction action, int parameter = 0) {
        // Parcourir les handlers dans l'ordre de priorité
        for (auto& handler : handlers_) {
            if (handler->handleAction(action, parameter)) {
                return true;
            }
        }
        
        // Aucun handler n'a pu traiter l'action
        return false;
    }
    
    /**
     * @brief Vérifie si une action peut être traitée
     * @param action Action à vérifier
     * @return true si au moins un handler peut traiter l'action
     */
    bool canHandleAction(NavigationAction action) const {
        for (const auto& handler : handlers_) {
            if (handler->handleAction(action, 0)) {
                return true;
            }
        }
        return false;
    }

private:
    void initializeHandlers() {
        // Allocation statique avec ETL pour éviter les problèmes d'allocation dynamique
        handlers_.clear();
        
        // Ordre d'importance : spéciales d'abord, puis contextuelles
        handlers_.push_back(std::make_shared<SpecialActionHandler>(stateManager_));
        handlers_.push_back(std::make_shared<MenuNavigationHandler>(stateManager_));
        handlers_.push_back(std::make_shared<ParameterActionHandler>(stateManager_));
        handlers_.push_back(std::make_shared<ContextualActionHandler>(stateManager_));
    }

private:
    std::shared_ptr<NavigationStateManager> stateManager_;
    
    // Utiliser ETL pour allocation statique (max 8 handlers prévus)
    etl::vector<std::shared_ptr<BaseNavigationHandler>, 8> handlers_;
};