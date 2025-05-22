#include "app/services/NavigationConfigService.hpp"

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
