#pragma once
#include <optional>
#include <unordered_map>

#include "config/unified/ControlDefinition.hpp"
#include "core/domain/types.hpp"
#include "core/utils/Result.hpp"

/**
 * Simule un stockage persistant pour les contrôles (en RAM).
 * Peut être remplacé par EEPROM/Flash plus tard.
 */
class ControlDefinitionStore {
public:
    Result<void> saveControlDefinition(const ControlDefinition& controlDef);
    Result<ControlDefinition> loadControlDefinition(InputId id) const;
    Result<std::vector<ControlDefinition>> loadAllControlDefinitions() const;
    Result<void> clearAll();

private:
    std::unordered_map<InputId, ControlDefinition> storage_;
};
