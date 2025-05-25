#include "adapters/secondary/storage/SettingsStore.hpp"

#include <optional>

void SettingsStore::saveControlDefinition(const ControlDefinition& controlDef) {
    storage_[controlDef.id] = controlDef;
}

std::optional<ControlDefinition> SettingsStore::loadControlDefinition(InputId id) const {
    auto it = storage_.find(id);
    if (it != storage_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ControlDefinition> SettingsStore::loadAllControlDefinitions() const {
    std::vector<ControlDefinition> result;
    result.reserve(storage_.size());
    
    for (const auto& pair : storage_) {
        result.push_back(pair.second);
    }
    
    return result;
}

void SettingsStore::clearAll() {
    storage_.clear();
}
