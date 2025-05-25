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
    std::optional<ControlDefinition> getControlDefinition(InputId id) const override;
    void setControlDefinition(const ControlDefinition& controlDef) override;
    std::vector<ControlDefinition> getAllControlDefinitions() const override;
    bool saveProfile() override;
    bool loadProfile() override;
    void resetToDefaults() override;
    bool removeBinding(InputId id) override;

private:
    std::unordered_map<InputId, ControlDefinition> controlDefinitions_;
};
