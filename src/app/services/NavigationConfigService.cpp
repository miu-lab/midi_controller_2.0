#include "app/services/NavigationConfigService.hpp"

NavigationConfigService::NavigationConfigService() : initialized_(false) {
}

bool NavigationConfigService::init() {
    if (initialized_) {
        return true;
    }
    
    // Réinitialiser les contrôles de navigation
    navigationControls_.clear();
    
    initialized_ = true;
    return true;
}

void NavigationConfigService::setControlForNavigation(InputId id, bool isNavigation) {
    if (isNavigation) {
        // Ajouter à l'ensemble des contrôles de navigation
        navigationControls_.insert(id);
    } else {
        // Retirer de l'ensemble si présent
        navigationControls_.erase(id);
    }
}

bool NavigationConfigService::isNavigationControl(InputId id) const {
    // Vérifie si l'identifiant est présent dans l'ensemble des contrôles de navigation
    return navigationControls_.find(id) != navigationControls_.end();
}

void NavigationConfigService::setNavigationControls(const std::set<InputId>& navigationControlIds) {
    navigationControls_ = navigationControlIds;
}

const std::set<InputId>& NavigationConfigService::getNavigationControlIds() const {
    return navigationControls_;
}

void NavigationConfigService::clearNavigationControls() {
    navigationControls_.clear();
}

bool NavigationConfigService::isInitialized() const {
    return initialized_;
}
