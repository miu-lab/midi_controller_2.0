#include <optional>

#include "adapters/secondary/storage/ControlDefinitionStore.hpp"

Result<void> ControlDefinitionStore::saveControlDefinition(const ControlDefinition& controlDef) {
    storage_[controlDef.id] = controlDef;
    return Result<void>::success();
}

Result<ControlDefinition> ControlDefinitionStore::loadControlDefinition(InputId id) const {
    auto it = storage_.find(id);
    if (it != storage_.end()) {
        return Result<ControlDefinition>::success(it->second);
    }
    return Result<ControlDefinition>::error({ErrorCode::OperationFailed, "Control definition not found"});
}

Result<std::vector<ControlDefinition>> ControlDefinitionStore::loadAllControlDefinitions() const {
    std::vector<ControlDefinition> result;
    result.reserve(storage_.size());
    
    for (const auto& pair : storage_) {
        result.push_back(pair.second);
    }
    
    return Result<std::vector<ControlDefinition>>::success(result);
}

Result<void> ControlDefinitionStore::clearAll() {
    storage_.clear();
    return Result<void>::success();
}
