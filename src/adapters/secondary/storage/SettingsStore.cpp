#include "adapters/secondary/storage/SettingsStore.hpp"

#include <optional>

void SettingsStore::saveMapping(ControlId id, MidiControl binding) {
    storage_[id] = binding;
}

std::optional<MidiControl> SettingsStore::loadMapping(ControlId id) const {
    auto it = storage_.find(id);
    if (it != storage_.end()) {
        return it->second;
    }
    return std::nullopt;
}
