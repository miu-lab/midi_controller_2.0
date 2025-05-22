#pragma once
#include <optional>
#include <unordered_map>

#include "core/domain/types.hpp"

/**
 * Simule un stockage persistant pour les mappings (en RAM).
 * Peut être remplacé par EEPROM/Flash plus tard.
 */
class SettingsStore {
public:
    void saveMapping(InputId id, MidiControl binding);
    std::optional<MidiControl> loadMapping(InputId id) const;

private:
    std::unordered_map<InputId, MidiControl> storage_;
};
