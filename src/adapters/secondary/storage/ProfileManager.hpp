#pragma once
#include <unordered_map>
#include <vector>

#include "core/ports/output/ProfileStoragePort.hpp"

/**
 * Implémentation simple en RAM du gestionnaire de profils.
 */
class ProfileManager : public ProfileStoragePort {
public:
    std::optional<MidiControl> getBinding(InputId id) const override;
    void setBinding(InputId id, MidiControl binding) override;
    std::vector<InputMapping> getAllMappings() const override;
    bool saveProfile() override;
    bool loadProfile() override;
    void resetToDefaults() override;
    bool removeBinding(InputId id) override;

private:
    std::unordered_map<InputId, MidiControl> bindings_;
};
