#include "storage/ProfileManager.hpp"

std::optional<MidiControl> ProfileManager::getBinding(ControlId id) const {
    auto it = bindings_.find(id);
    if (it != bindings_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ProfileManager::setBinding(ControlId id, MidiControl binding) {
    bindings_[id] = binding;
}
