#pragma once
#include <cstdint>
#include <optional>
#include <string>

using DefaultId = uint16_t;
using DefaultMidiId = uint8_t;
using DefaultName = std::string;

using InputId = DefaultId;
using InputName = DefaultName;
using InputLabel = DefaultName;
using InputDescription = DefaultName;
using InputGroup = DefaultName;

using PinId = DefaultId;

using EncoderId = DefaultId;
using ButtonId = DefaultId;

using MidiChannel = DefaultMidiId;
using MidiCC = DefaultMidiId;
using MidiNote = DefaultMidiId;

/**
 * @brief Mode de configuration GPIO
 */
enum class PinMode {
    PULLUP,    ///< Entrée avec résistance de tirage vers le haut
    PULLDOWN,  ///< Entrée avec résistance de tirage vers le bas
    RAW        ///< Entrée sans résistance de tirage
};

/**
 * @brief Configuration sécurisée d'une broche GPIO
 */
struct GpioPin {
    uint8_t pin;
    PinMode mode = PinMode::PULLUP;

    /**
     * @brief Vérifie si la configuration de la broche est valide
     * @return true si la broche est dans la plage valide (0-99 pour Teensy)
     */
    constexpr bool isValid() const {
        return pin <= 99;  // Teensy 4.1 max pins
    }
};

/**
 * @brief Type de mapping (encodeur, bouton d'encodeur)
 */
enum class MappingType { ENCODER, BUTTON };

/**
 * @brief Type de contrôle (encodeur, bouton d'encodeur)
 */
using InputType = MappingType;

/**
 * @brief Type de contrôle (encodeur, bouton d'encodeur, ou bouton séparé)
 */
enum class MidiEventType {
    NOTE_ON,
    NOTE_OFF,
    CONTROL_CHANGE,
    CHANNEL_PRESSURE,
    POLY_PRESSURE,
    PITCH_BEND,
    PROGRAM_CHANGE,
    SYSEX,
    CLOCK,
    COMMON,
};

/**
 * @brief Configuration d'un controle MIDI
 */
struct MidiControl {
    MidiChannel channel{1};
    MidiCC control{0};
    MidiEventType type{MidiEventType::CONTROL_CHANGE};
    bool isRelative{true};
    std::optional<bool> isCentered{true};
};

/**
 * @brief Affectation d'un controle MIDI à un encodeur
 */
struct InputMapping {
    InputId controlId;
    MappingType mappingType{MappingType::ENCODER};
    MidiControl midiMapping;
};

/**
 * @brief Evénement d'un bouton
 */
enum class ButtonEvent { None, Pressed, Released, LongPress };