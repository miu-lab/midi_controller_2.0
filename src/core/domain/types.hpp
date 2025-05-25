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
    uint8_t pin;
    PinMode mode = PinMode::PULLUP;

    /**
     * @brief Vérifie la validité de la configuration
     * @return true si la broche est valide pour Teensy 4.1
     */
    constexpr bool isValid() const {
        return pin <= 99;            // Teensy 4.1 max pins
    }
};

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
