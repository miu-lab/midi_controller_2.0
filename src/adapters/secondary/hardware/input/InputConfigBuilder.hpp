#pragma once
#include <stdexcept>
#include <string>

#include "adapters/secondary/hardware/input/InputConfig.hpp"

/**
 * @brief Builder pour créer des configurations InputConfig de manière sécurisée
 *
 * Cette classe facilite la création de configurations InputConfig en fournissant
 * des méthodes typées et en validant automatiquement les configurations.
 */
class InputConfigBuilder {
public:
    /**
     * @brief Crée une configuration de bouton
     * @param id Identifiant unique du bouton
     * @param name Nom technique
     * @param label Étiquette d'affichage
     * @param pin Numéro de broche GPIO
     * @param mode Mode du bouton (MOMENTARY ou TOGGLE)
     * @param activeLow true si actif à LOW, false si actif à HIGH
     * @return Configuration InputConfig validée
     * @throws std::invalid_argument si la configuration est invalide
     */
    static InputConfig createButton(InputId id, const std::string& name, const std::string& label,
                                    uint8_t pin, ButtonMode mode = ButtonMode::MOMENTARY,
                                    bool activeLow = true) {
        ButtonConfig btnConfig{.id = id,
                               .gpio = {.pin = pin, .mode = PinMode::PULLUP},
                               .activeLow = activeLow,
                               .mode = mode};

        InputConfig input{.id = id,
                          .name = name,
                          .type = InputType::BUTTON,
                          .label = label,
                          .config = btnConfig};

        if (!input.isValid()) {
            throw std::invalid_argument("Configuration de bouton invalide: " + name);
        }

        return input;
    }

    /**
     * @brief Crée une configuration de bouton avancée avec debounce et long press
     * @param id Identifiant unique du bouton
     * @param name Nom technique
     * @param label Étiquette d'affichage
     * @param pin Numéro de broche GPIO
     * @param mode Mode du bouton
     * @param activeLow Logique du bouton
     * @param debounceMs Délai anti-rebond en ms
     * @param enableLongPress Activer la détection de pression longue
     * @param longPressMs Seuil de pression longue en ms
     * @return Configuration InputConfig validée
     * @throws std::invalid_argument si la configuration est invalide
     */
    static InputConfig createAdvancedButton(InputId id, const std::string& name,
                                            const std::string& label, uint8_t pin,
                                            ButtonMode mode = ButtonMode::MOMENTARY,
                                            bool activeLow = true, uint16_t debounceMs = 50,
                                            bool enableLongPress = false,
                                            uint16_t longPressMs = 800) {
        ButtonConfig btnConfig{.id = id,
                               .gpio = {.pin = pin, .mode = PinMode::PULLUP},
                               .activeLow = activeLow,
                               .mode = mode,
                               .debounceMs = debounceMs,
                               .longPressMs = longPressMs,
                               .enableLongPress = enableLongPress};

        InputConfig input{.id = id,
                          .name = name,
                          .type = InputType::BUTTON,
                          .label = label,
                          .config = btnConfig};

        if (!input.isValid()) {
            throw std::invalid_argument("Configuration de bouton avancée invalide: " + name);
        }

        return input;
    }

    /**
     * @brief Crée une configuration d'encodeur simple
     * @param id Identifiant unique de l'encodeur
     * @param name Nom technique
     * @param label Étiquette d'affichage
     * @param pinA Broche GPIO du signal A
     * @param pinB Broche GPIO du signal B
     * @param ppr Impulsions par rotation
     * @return Configuration InputConfig validée
     * @throws std::invalid_argument si la configuration est invalide
     */
    static InputConfig createEncoder(InputId id, const std::string& name, const std::string& label,
                                     uint8_t pinA, uint8_t pinB, uint16_t ppr = 24) {
        EncoderConfig encConfig{.id = id,
                                .pinA = {.pin = pinA, .mode = PinMode::PULLUP},
                                .pinB = {.pin = pinB, .mode = PinMode::PULLUP},
                                .ppr = ppr};

        InputConfig input{.id = id,
                          .name = name,
                          .type = InputType::ENCODER,
                          .label = label,
                          .config = encConfig};

        if (!input.isValid()) {
            throw std::invalid_argument("Configuration d'encodeur invalide: " + name);
        }

        return input;
    }

    /**
     * @brief Crée une configuration d'encodeur avec bouton intégré
     * @param id Identifiant unique de l'encodeur
     * @param name Nom technique
     * @param label Étiquette d'affichage
     * @param pinA Broche GPIO du signal A
     * @param pinB Broche GPIO du signal B
     * @param buttonPin Broche GPIO du bouton intégré
     * @param buttonId Identifiant du bouton (typiquement 1000 + encoderId)
     * @param ppr Impulsions par rotation
     * @param buttonMode Mode du bouton intégré
     * @return Configuration InputConfig validée
     * @throws std::invalid_argument si la configuration est invalide
     */
    static InputConfig createEncoderWithButton(InputId id, const std::string& name,
                                               const std::string& label, uint8_t pinA, uint8_t pinB,
                                               uint8_t buttonPin, ButtonId buttonId,
                                               uint16_t ppr = 24,
                                               ButtonMode buttonMode = ButtonMode::MOMENTARY) {
        ButtonConfig btnConfig{.id = buttonId,
                               .gpio = {.pin = buttonPin, .mode = PinMode::PULLUP},
                               .activeLow = true,
                               .mode = buttonMode};

        EncoderConfig encConfig{.id = id,
                                .pinA = {.pin = pinA, .mode = PinMode::PULLUP},
                                .pinB = {.pin = pinB, .mode = PinMode::PULLUP},
                                .ppr = ppr,
                                .buttonConfig = btnConfig};

        InputConfig input{.id = id,
                          .name = name,
                          .type = InputType::ENCODER,
                          .label = label,
                          .config = encConfig};

        if (!input.isValid()) {
            throw std::invalid_argument("Configuration d'encodeur avec bouton invalide: " + name);
        }

        return input;
    }

    /**
     * @brief Crée une configuration d'encodeur haute performance
     * @param id Identifiant unique de l'encodeur
     * @param name Nom technique
     * @param label Étiquette d'affichage
     * @param pinA Broche GPIO du signal A
     * @param pinB Broche GPIO du signal B
     * @param ppr Impulsions par rotation
     * @param sensitivity Multiplicateur de sensibilité
     * @param enableAcceleration Activer l'accélération
     * @param invertDirection Inverser le sens de rotation
     * @param stepsPerDetent Steps par cran mécanique
     * @return Configuration InputConfig validée
     * @throws std::invalid_argument si la configuration est invalide
     */
    static InputConfig createHighPerformanceEncoder(
        InputId id, const std::string& name, const std::string& label, uint8_t pinA, uint8_t pinB,
        uint16_t ppr = 600, float sensitivity = 1.0f, bool enableAcceleration = true,
        bool invertDirection = false, uint8_t stepsPerDetent = 4) {
        EncoderConfig encConfig{.id = id,
                                .pinA = {.pin = pinA, .mode = PinMode::PULLUP},
                                .pinB = {.pin = pinB, .mode = PinMode::PULLUP},
                                .ppr = ppr,
                                .invertDirection = invertDirection,
                                .sensitivity = sensitivity,
                                .enableAcceleration = enableAcceleration,
                                .stepsPerDetent = stepsPerDetent};

        InputConfig input{.id = id,
                          .name = name,
                          .type = InputType::ENCODER,
                          .label = label,
                          .config = encConfig};

        if (!input.isValid()) {
            throw std::invalid_argument("Configuration d'encodeur haute performance invalide: " +
                                        name);
        }

        return input;
    }

    /**
     * @brief Définit les métadonnées UI d'une configuration
     * @param input Configuration à modifier
     * @param group Groupe logique
     * @param description Description détaillée
     * @param displayOrder Ordre d'affichage
     * @param enabled État activé/désactivé
     * @return Configuration modifiée
     */
    static InputConfig& setUIMetadata(InputConfig& input, const std::string& group = "General",
                                      const std::string& description = "", uint8_t displayOrder = 0,
                                      bool enabled = true) {
        input.group = group;
        input.description = description;
        input.displayOrder = displayOrder;
        input.enabled = enabled;
        return input;
    }
};
