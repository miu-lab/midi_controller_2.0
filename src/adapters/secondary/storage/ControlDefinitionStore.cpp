#include <optional>

#include "adapters/secondary/storage/ControlDefinitionStore.hpp"

void ControlDefinitionStore::saveControlDefinition(const ControlDefinition& controlDef) {
    storage_[controlDef.id] = controlDef;
}

std::optional<ControlDefinition> ControlDefinitionStore::loadControlDefinition(InputId id) const {
    auto it = storage_.find(id);
    if (it != storage_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ControlDefinition> ControlDefinitionStore::loadAllControlDefinitions() const {
    std::vector<ControlDefinition> result;
    result.reserve(storage_.size());
    
    for (const auto& pair : storage_) {
        result.push_back(pair.second);
    }
    
    return result;
}

void ControlDefinitionStore::clearAll() {
    storage_.clear();
}
