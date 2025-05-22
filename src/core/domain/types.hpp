#pragma once
#include <cstdint>
#include <optional>

using DefaultId = uint16_t;
using DefaultMidiId = uint8_t;

using ControlId = DefaultId;
using PinId = DefaultId;
using EncoderId = DefaultId;
using ButtonId = DefaultId;

using MidiChannel = DefaultMidiId;
using MidiCC = DefaultMidiId;
using MidiNote = DefaultMidiId;

/**
 * @brief Type de contrôle (encodeur, bouton d'encodeur, ou bouton séparé)
 */
enum class MappingType { ENCODER, BUTTON, SIMPLE_BUTTON };

/**
 * @brief Configuration d'un controle MIDI
 */
struct MidiControl {
    MidiChannel channel{1};
    MidiCC control{0};
    bool isRelative{true};
    std::optional<bool> isCentered{true};
};

/**
 * @brief Affectation d'un controle MIDI à un encodeur
 */
struct ControlMapping {
    ControlId controlId;
    MappingType mappingType{MappingType::ENCODER};
    MidiControl midiMapping;
};

/**
 * @brief Evénement d'un bouton
 */
enum class ButtonEvent { None, Pressed, Released, LongPress };