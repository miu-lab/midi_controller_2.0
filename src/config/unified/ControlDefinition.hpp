#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core/domain/types.hpp"

/**
 * @brief Définition complète d'un contrôle unifiant hardware et mappings
 *
 * Cette structure combine la configuration hardware et tous les mappings
 * associés à un contrôle, éliminant la duplication et garantissant la cohérence.
 */
struct ControlDefinition {
    // === IDENTITÉ ===
    InputId id;         ///< ID unique du contrôle principal
    std::string name;   ///< Nom technique (ex: "encoder_1")
    std::string label;  ///< Label affiché (ex: "Volume")

    // === HARDWARE ===
    struct EncoderConfig {
        GpioPin pinA;
        GpioPin pinB;
        uint16_t ppr;
        float sensitivity;
        bool enableAcceleration;
        uint8_t stepsPerDetent;
    };

    struct ButtonConfig {
        GpioPin pin;
        bool activeLow;
        ButtonMode mode;
        uint16_t debounceMs;
        std::optional<uint16_t> longPressMs;
    };

    struct HardwareSpec {
        InputType type;  ///< ENCODER ou BUTTON

        // Configuration spécifique au type
        std::variant<EncoderConfig, ButtonConfig> config;

        // Bouton optionnel pour les encodeurs
        std::optional<GpioPin> encoderButtonPin;
        std::optional<uint16_t> encoderButtonDebounceMs;
    } hardware;

    // === MAPPINGS ===
    struct MidiConfig {
        uint8_t channel;
        uint8_t control;
        bool isRelative;
    };

    struct NavigationConfig {
        std::string action;  // "HOME", "BACK", "ITEM_NAVIGATOR", etc.
    };

    struct MappingSpec {
        MappingRole role;
        std::variant<MidiConfig, NavigationConfig> config;
        MappingControlType appliesTo;  // ENCODER ou BUTTON
    };

    std::vector<MappingSpec> mappings;

    // === MÉTADONNÉES ===
    std::string group = "General";  ///< Groupe logique
    std::string description = "";   ///< Description détaillée
    bool enabled = true;            ///< Actif/Inactif
    uint8_t displayOrder = 0;       ///< Ordre d'affichage

    // === MÉTHODES UTILITAIRES ===

    /**
     * @brief Génère l'ID du bouton associé à un encodeur
     * Convention : ID bouton = 1000 + ID encodeur
     */
    InputId getEncoderButtonId() const {
        if (hardware.type == InputType::ENCODER && hardware.encoderButtonPin) {
            return id + 1000;
        }
        return 0;
    }

    /**
     * @brief Vérifie si ce contrôle a un rôle spécifique
     */
    bool hasRole(MappingRole role) const {
        return std::any_of(mappings.begin(), mappings.end(), [role](const MappingSpec& m) {
            return m.role == role;
        });
    }

    /**
     * @brief Récupère les mappings pour un rôle donné
     */
    std::vector<MappingSpec> getMappingsForRole(MappingRole role) const {
        std::vector<MappingSpec> result;
        std::copy_if(mappings.begin(),
                     mappings.end(),
                     std::back_inserter(result),
                     [role](const MappingSpec& m) { return m.role == role; });
        return result;
    }
};
