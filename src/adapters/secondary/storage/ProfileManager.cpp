#include "adapters/secondary/storage/ProfileManager.hpp"
#include "config/unified/ControlBuilder.hpp"

std::optional<ControlDefinition> ProfileManager::getControlDefinition(InputId id) const {
    auto it = controlDefinitions_.find(id);
    if (it != controlDefinitions_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ProfileManager::setControlDefinition(const ControlDefinition& controlDef) {
    controlDefinitions_[controlDef.id] = controlDef;
}

std::vector<ControlDefinition> ProfileManager::getAllControlDefinitions() const {
    std::vector<ControlDefinition> result;
    result.reserve(controlDefinitions_.size());

    for (const auto& pair : controlDefinitions_) {
        result.push_back(pair.second);
    }

    return result;
}

bool ProfileManager::saveProfile() {
    // Implémentation simple - à développer avec un vrai stockage persistant
    return true;
}

bool ProfileManager::loadProfile() {
    // Implémentation simple - à développer avec un vrai stockage persistant
    return true;
}

void ProfileManager::resetToDefaults() {

}

bool ProfileManager::removeBinding(InputId id) {
    auto it = controlDefinitions_.find(id);
    if (it != controlDefinitions_.end()) {
        controlDefinitions_.erase(it);
        return true;
    }
    return false;
}
