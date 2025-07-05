#pragma once
#include <unordered_map>
#include <vector>

#include "config/unified/ControlDefinition.hpp"
#include "core/ports/output/ProfileStoragePort.hpp"

/**
 * Implémentation simple en RAM du gestionnaire de profils.
 */
class ProfileManager : public ProfileStoragePort {
public:
    Result<ControlDefinition> getControlDefinition(InputId id) const override;
    Result<void> setControlDefinition(const ControlDefinition& controlDef) override;
    Result<std::vector<ControlDefinition>> getAllControlDefinitions() const override;
    Result<void> saveProfile() override;
    Result<void> loadProfile() override;
    Result<void> resetToDefaults() override;
    Result<void> removeBinding(InputId id) override;

private:
    std::unordered_map<InputId, ControlDefinition> controlDefinitions_;
};
