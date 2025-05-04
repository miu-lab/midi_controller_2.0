#pragma once
#include "domain/IProfileManager.hpp"
#include <unordered_map>

/**
 * Implémentation simple en RAM.
 */
class ProfileManager : public IProfileManager {
public:
    std::optional<MidiControl> getBinding(ControlId id) const override;
    void setBinding(ControlId id, MidiControl binding) override;

private:
    std::unordered_map<ControlId, MidiControl> bindings_;
};
