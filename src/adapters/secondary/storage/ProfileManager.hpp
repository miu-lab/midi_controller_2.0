#pragma once
#include <unordered_map>
#include <vector>

#include "core/domain/IProfileManager.hpp"

/**
 * Implémentation simple en RAM du gestionnaire de profils.
 */
class ProfileManager : public IProfileManager {
public:
    std::optional<MidiControl> getBinding(ControlId id) const override;
    void setBinding(ControlId id, MidiControl binding) override;
    std::vector<MidiControlMapping> getAllMappings() const override;
    bool saveProfile() override;
    bool loadProfile() override;
    void resetToDefaults() override;
    bool removeBinding(ControlId id) override;

private:
    std::unordered_map<ControlId, MidiControl> bindings_;
};
