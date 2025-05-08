#pragma once

#include "core/ports/output/MidiOutputPort.hpp"

/**
 * @brief Alias pour maintenir la compatibilité
 * @deprecated Utiliser MidiOutputPort à la place
 */
class IMidiOut : public MidiOutputPort {
public:
    // Méthodes virtuelles pour maintenir la compatibilité avec l'ancienne interface
    virtual void sendProgramChange(MidiChannel ch, uint8_t program) override {}

    virtual void sendPitchBend(MidiChannel ch, uint16_t value) override {}

    virtual void sendChannelPressure(MidiChannel ch, uint8_t pressure) override {}

    virtual void sendSysEx(const uint8_t* data, uint16_t length) override {}
};
