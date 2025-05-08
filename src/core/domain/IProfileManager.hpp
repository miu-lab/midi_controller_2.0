#pragma once
#include "types.hpp"
#include <optional>

/**
 * Interface pour accéder aux liaisons MIDI.
 */
class IProfileManager {
public:
    virtual std::optional<MidiControl> getBinding(ControlId id) const = 0;
    virtual void setBinding(ControlId id, MidiControl binding) = 0;
    virtual ~IProfileManager() = default;
};
