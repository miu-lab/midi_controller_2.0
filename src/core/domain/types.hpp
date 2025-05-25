#pragma once
#include <cstdint>
#include <optional>
#include <set>
#include <string>

using DefaultId = uint16_t;
using DefaultMidiId = uint8_t;
using DefaultName = std::string;

using InputId = DefaultId;
using InputName = DefaultName;
using InputLabel = DefaultName;
using InputDescription = DefaultName;
using InputGroup = DefaultName;

using NavigationName = InputName;
using NavigationGroup = InputGroup;

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
 * @brief Mode de fonctionnement d'un bouton
 */
enum class ButtonMode {
    MOMENTARY,  ///< Mode par défaut : actif uniquement lorsqu'il est pressé
    TOGGLE      ///< Mode toggle : chaque pression inverse l'état
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
enum class MappingControlType { ENCODER, BUTTON };

/**
 * @brief Type de contrôle (encodeur, bouton d'encodeur)
 */
using InputType = MappingControlType;

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
 * @brief Type de contrôle (encodeur, bouton d'encodeur, ou bouton séparé)
 */
enum class MappingRole { MIDI, NAVIGATION };

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

struct NavigationControl {
    NavigationName name{"HOME"};  ///< Nom du contrôle de navigation
};

/**
 * @brief Affectation d'un controle MIDI à un encodeur
 */
struct InputMapping {
    InputId controlId;
    std::set<MappingRole> roles;
    MappingControlType mappingType{MappingControlType::ENCODER};
    MidiControl midiMapping;
    NavigationControl navigationMapping;
};

/**
 * @brief Evénement d'un bouton
 */
enum class ButtonEvent { None, Pressed, Released, LongPress };