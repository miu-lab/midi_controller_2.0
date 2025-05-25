#pragma once
#include <optional>
#include <unordered_map>

#include "config/unified/ControlDefinition.hpp"
#include "core/domain/types.hpp"

/**
 * Simule un stockage persistant pour les contrôles (en RAM).
 * Peut être remplacé par EEPROM/Flash plus tard.
 */
class SettingsStore {
public:
    void saveControlDefinition(const ControlDefinition& controlDef);
    std::optional<ControlDefinition> loadControlDefinition(InputId id) const;
    std::vector<ControlDefinition> loadAllControlDefinitions() const;
    void clearAll();

private:
    std::unordered_map<InputId, ControlDefinition> storage_;
};
