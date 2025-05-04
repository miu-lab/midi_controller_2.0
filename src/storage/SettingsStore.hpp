#pragma once
#include "domain/types.hpp"
#include <unordered_map>
#include <optional>


/**
 * Simule un stockage persistant pour les mappings (en RAM).
 * Peut être remplacé par EEPROM/Flash plus tard.
 */
class SettingsStore {
public:
    void saveMapping(ControlId id, MidiControl binding);
    std::optional<MidiControl> loadMapping(ControlId id) const;

private:
    std::unordered_map<ControlId, MidiControl> storage_;
};
