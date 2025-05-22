#pragma once
#include <cstdint>

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
enum class ControlType { ENCODER_ROTATION, ENCODER_BUTTON, BUTTON };

/**
 * @brief Configuration d'un controle MIDI
 */
struct MidiControl {
    MidiChannel channel{1};
    MidiCC control{0};
    bool relative{true};
    ControlType controlType{ControlType::ENCODER_ROTATION};
};

/**
 * @brief Affectation d'un controle MIDI à un encodeur
 */
struct MidiControlMapping {
    ControlId controlId;
    MidiControl midiControl;
};

/**
 * @brief Evénement d'un bouton
 */
enum class ButtonEvent { None, Pressed, Released, LongPress };