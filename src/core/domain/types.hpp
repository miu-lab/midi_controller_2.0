#pragma once
#include <cstdint>
#include <string>

// =====================================================
// TYPES DE BASE - PHILOSOPHIE UNIFIÉE
// =====================================================

// === IDENTIFIANTS ===
using InputId = uint16_t;        ///< ID unique pour tous contrôles
using ButtonId = uint16_t;       ///< ID spécifique boutons
using EncoderId = uint16_t;      ///< ID spécifique encodeurs

// === TYPES MIDI ===
using MidiChannel = uint8_t;     ///< Canal MIDI (1-16)
using MidiCC = uint8_t;          ///< Numéro CC MIDI (0-127)
using MidiNote = uint8_t;        ///< Note MIDI (0-127)

// =====================================================
// ENUMS HARDWARE
// =====================================================

/**
 * @brief Mode de configuration GPIO
 */
enum class PinMode {
    PULLUP,                      ///< Résistance de tirage vers le haut
    PULLDOWN,                    ///< Résistance de tirage vers le bas
    RAW                          ///< Sans résistance de tirage
};

/**
 * @brief Mode de fonctionnement d'un bouton
 */
enum class ButtonMode {
    MOMENTARY,                   ///< Actif seulement quand pressé
    TOGGLE                       ///< Chaque pression inverse l'état
};

// =====================================================
// TYPES DE CONTRÔLES
// =====================================================

/**
 * @brief Type de contrôle physique
 */
enum class MappingControlType { 
    ENCODER,                     ///< Encodeur rotatif
    BUTTON                       ///< Bouton poussoir
};

/**
 * @brief Alias pour compatibilité
 */
using InputType = MappingControlType;

// =====================================================
// ENUMS MIDI ET MAPPINGS
// =====================================================

/**
 * @brief Types d'événements MIDI
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
    COMMON
};

/**
 * @brief Rôles des mappings dans le système unifié
 */
enum class MappingRole { 
    MIDI,                        ///< Contrôle MIDI
    NAVIGATION                   ///< Navigation interface
};

// =====================================================
// STRUCTURES HARDWARE
// =====================================================

/**
 * @brief Configuration sécurisée d'une broche GPIO
 */
struct GpioPin {
    /**
     * @brief Source de la pin
     */
    enum class Source {
        MCU,  ///< Pin Teensy directe
        MUX   ///< Pin via multiplexeur CD74HC4067
    };

    Source source = Source::MCU;
    uint8_t pin = 0;  ///< Numéro de pin (Teensy) ou canal (Mux 0-15)
    PinMode mode = PinMode::PULLUP;

    /**
     * @brief Constructeur par défaut
     */
    constexpr GpioPin() = default;

    /**
     * @brief Constructeur pour pin Teensy directe (rétrocompatibilité)
     */
    constexpr GpioPin(uint8_t pinNum, PinMode pinMode = PinMode::PULLUP)
        : source(Source::MCU), pin(pinNum), mode(pinMode) {}

    /**
     * @brief Constructeur complet avec source explicite
     */
    constexpr GpioPin(Source src, uint8_t pinNum, PinMode pinMode = PinMode::PULLUP)
        : source(src), pin(pinNum), mode(pinMode) {}

    /**
     * @brief Vérifie si la pin est multiplexée
     */
    constexpr bool isMultiplexed() const {
        return source == Source::MUX;
    }

    /**
     * @brief Obtient le canal du multiplexeur (si applicable)
     */
    constexpr uint8_t getMuxChannel() const {
        return isMultiplexed() ? pin : 0xFF;
    }

    /**
     * @brief Vérifie la validité de la configuration
     */
    constexpr bool isValid() const {
        if (source == Source::MUX) {
            return pin <= 15;  // Mux a 16 canaux (0-15)
        }
        return pin <= 99;  // Teensy 4.1 max pins
    }
};

// =====================================================
// HELPER FUNCTIONS POUR CRÉATION DE PINS
// =====================================================

/**
 * @brief Créer une pin MCU/Teensy directe (fonction helper globale)
 */
inline constexpr GpioPin mcuPin(uint8_t pinNum, PinMode mode = PinMode::PULLUP) {
    return GpioPin(GpioPin::Source::MCU, pinNum, mode);
}

/**
 * @brief Créer une pin multiplexée (fonction helper globale)
 */
inline constexpr GpioPin muxPin(uint8_t channel, PinMode mode = PinMode::PULLUP) {
    return GpioPin(GpioPin::Source::MUX, channel, mode);
}

// =====================================================
// ÉVÉNEMENTS
// =====================================================

/**
 * @brief Types d'événements bouton
 */
enum class ButtonEvent { 
    None, 
    Pressed, 
    Released, 
    LongPress 
};
