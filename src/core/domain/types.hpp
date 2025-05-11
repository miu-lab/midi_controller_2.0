#pragma once
#include <cstdint>

using ControlId = uint8_t;
using EncoderId = ControlId;
using ButtonId = ControlId;
using MidiChannel = uint8_t;
using MidiCC = uint8_t;
using MidiNote = uint8_t;

/**
 * @brief Type de contrôle (encodeur, bouton d'encodeur, ou bouton séparé)
 */
enum class ControlType {
    ENCODER_ROTATION,
    ENCODER_BUTTON,
    BUTTON
};

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