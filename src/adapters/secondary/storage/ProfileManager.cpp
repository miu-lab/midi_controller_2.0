#include "adapters/secondary/storage/ProfileManager.hpp"
#include "config/unified/ControlBuilder.hpp"

Result<ControlDefinition> ProfileManager::getControlDefinition(InputId id) const {
    auto it = controlDefinitions_.find(id);
    if (it != controlDefinitions_.end()) {
        return Result<ControlDefinition>::success(it->second);
    }
    return Result<ControlDefinition>::error({ErrorCode::OperationFailed, "Control definition not found"});
}

Result<void> ProfileManager::setControlDefinition(const ControlDefinition& controlDef) {
    controlDefinitions_[controlDef.id] = controlDef;
    return Result<void>::success();
}

Result<std::vector<ControlDefinition>> ProfileManager::getAllControlDefinitions() const {
    std::vector<ControlDefinition> result;
    result.reserve(controlDefinitions_.size());

    for (const auto& pair : controlDefinitions_) {
        result.push_back(pair.second);
    }

    return Result<std::vector<ControlDefinition>>::success(result);
}

Result<void> ProfileManager::saveProfile() {
    // Implémentation simple - à développer avec un vrai stockage persistant
    return Result<void>::success();
}

Result<void> ProfileManager::loadProfile() {
    // Implémentation simple - à développer avec un vrai stockage persistant
    return Result<void>::success();
}

Result<void> ProfileManager::resetToDefaults() {
    controlDefinitions_.clear();
    return Result<void>::success();
}

Result<void> ProfileManager::removeBinding(InputId id) {
    auto it = controlDefinitions_.find(id);
    if (it != controlDefinitions_.end()) {
        controlDefinitions_.erase(it);
        return Result<void>::success();
    }
    return Result<void>::error({ErrorCode::OperationFailed, "Control definition not found to remove"});
}
