#include "adapters/secondary/storage/ProfileManager.hpp"

std::optional<MidiControl> ProfileManager::getBinding(InputId id) const {
    auto it = bindings_.find(id);
    if (it != bindings_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ProfileManager::setBinding(InputId id, MidiControl binding) {
    bindings_[id] = binding;
}

std::vector<InputMapping> ProfileManager::getAllMappings() const {
    std::vector<InputMapping> result;
    result.reserve(bindings_.size());

    for (const auto& pair : bindings_) {
        result.push_back({.controlId = pair.first, .midiMapping = pair.second});
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
    bindings_.clear();

    // Ajouter ici les mappings par défaut
    // Par exemple :
    setBinding(71,
               {.channel = 0, .control = 1, .isRelative = false});  // Encodeur 1 -> CC 1, Canal 1
    setBinding(72,
               {.channel = 0, .control = 2, .isRelative = false});  // Encodeur 2 -> CC 2, Canal 1
    // ... etc.
}

bool ProfileManager::removeBinding(InputId id) {
    auto it = bindings_.find(id);
    if (it != bindings_.end()) {
        bindings_.erase(it);
        return true;
    }
    return false;
}
