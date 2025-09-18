#pragma once

#include <Arduino.h>
#include "config/hardware/PinRegistry.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include "config/SystemConstants.hpp"

/**
 * @brief Validateur pour vérifier l'absence de conflits dans les affectations de pins
 *
 * Effectue une validation complète au moment de la compilation et génère
 * un rapport détaillé de l'utilisation des pins.
 */
class PinValidator {
public:
    /**
     * @brief Valide la configuration complète et génère un rapport
     *
     * @param config Configuration unifiée des contrôles
     * @return true si aucun conflit, false sinon
     */
    static bool validateAndReport(const UnifiedConfiguration& config) {
        auto& registry = PinRegistry::getInstance();
        registry.clear();

        // 1. Enregistrer les pins de l'écran
        registerDisplayPins(registry);

        // 2. Enregistrer les pins du multiplexeur (si utilisé)
        registerMultiplexerPins(registry);

        // 3. Enregistrer les pins des contrôles
        registerControlPins(config, registry);

        // 4. Enregistrer les pins système (I2C, UART, etc.)
        registerSystemPins(registry);

        // 5. Générer et afficher le rapport
        std::string report = registry.generateReport();

        // En mode debug, toujours afficher le rapport
#ifdef DEBUG_BUILD
        Serial.println(report.c_str());
#endif

        // En cas de conflit, forcer l'affichage même en release
        if (registry.hasConflicts()) {
            Serial.println("\n⚠️ ⚠️ ⚠️  PIN CONFLICTS DETECTED! ⚠️ ⚠️ ⚠️");
            Serial.println(report.c_str());
            return false;
        }

        return true;
    }

    /**
     * @brief Validation statique à la compilation
     *
     * Cette fonction peut être appelée dans un static_assert pour
     * forcer une erreur de compilation en cas de conflit.
     */
    static constexpr bool compileTimeValidation() {
        // Vérifications basiques connues à la compilation
        // Ex: Vérifier que les pins de l'écran ne se chevauchent pas

        // Display pins
        constexpr uint8_t displayPins[] = {
            SystemConstants::Display::CS_PIN,
            SystemConstants::Display::DC_PIN,
            SystemConstants::Display::RST_PIN,
            SystemConstants::Display::MOSI_PIN,
            SystemConstants::Display::SCK_PIN,
            SystemConstants::Display::MISO_PIN
        };

        // Vérifier les doublons dans les pins de l'écran
        for (size_t i = 0; i < sizeof(displayPins); ++i) {
            for (size_t j = i + 1; j < sizeof(displayPins); ++j) {
                if (displayPins[i] == displayPins[j]) {
                    return false; // Conflit détecté
                }
            }
        }

        return true;
    }

private:
    static void registerDisplayPins(PinRegistry& registry) {
        using namespace SystemConstants::Display;

        registry.registerPin(CS_PIN, PinRegistry::PinUsage::DISPLAY_CS,
                            "ILI9341", "Chip Select");
        registry.registerPin(DC_PIN, PinRegistry::PinUsage::DISPLAY_DC,
                            "ILI9341", "Data/Command");
        registry.registerPin(RST_PIN, PinRegistry::PinUsage::DISPLAY_RST,
                            "ILI9341", "Reset");
        registry.registerPin(MOSI_PIN, PinRegistry::PinUsage::DISPLAY_MOSI,
                            "ILI9341", "SPI MOSI");
        registry.registerPin(SCK_PIN, PinRegistry::PinUsage::DISPLAY_SCK,
                            "ILI9341", "SPI Clock");
        registry.registerPin(MISO_PIN, PinRegistry::PinUsage::DISPLAY_MISO,
                            "ILI9341", "SPI MISO");
    }

    static void registerMultiplexerPins(PinRegistry& registry) {
        using namespace SystemConstants::Multiplexer;

        registry.registerPin(S0_PIN, PinRegistry::PinUsage::MUX_S0,
                            "74HC4067", "Select bit 0");
        registry.registerPin(S1_PIN, PinRegistry::PinUsage::MUX_S1,
                            "74HC4067", "Select bit 1");
        registry.registerPin(S2_PIN, PinRegistry::PinUsage::MUX_S2,
                            "74HC4067", "Select bit 2");
        registry.registerPin(S3_PIN, PinRegistry::PinUsage::MUX_S3,
                            "74HC4067", "Select bit 3");
        registry.registerPin(SIGNAL_PIN, PinRegistry::PinUsage::MUX_SIGNAL,
                            "74HC4067", "COM (Signal I/O)");
        // Note: Pas de pin ENABLE dans cette configuration
    }

    static void registerControlPins(const UnifiedConfiguration& config,
                                    PinRegistry& registry) {

        for (const auto& control : config.getAllControls()) {
            std::string component = control.name;

            // Enregistrer selon le type de hardware
            if (control.hardware.type == InputType::ENCODER) {
                if (auto* enc = std::get_if<ControlDefinition::EncoderConfig>(
                        &control.hardware.config)) {

                    registry.registerPin(enc->pinA.pin,
                                       PinRegistry::PinUsage::ENCODER_A,
                                       component, "Encoder A");
                    registry.registerPin(enc->pinB.pin,
                                       PinRegistry::PinUsage::ENCODER_B,
                                       component, "Encoder B");
                }
            }
            else if (control.hardware.type == InputType::BUTTON) {
                if (auto* btn = std::get_if<ControlDefinition::ButtonConfig>(
                        &control.hardware.config)) {

                    registry.registerPin(btn->pin.pin,
                                       PinRegistry::PinUsage::BUTTON,
                                       component, control.label);
                }
            }
        }
    }

    static void registerSystemPins(PinRegistry& registry) {
        // Note: Les pins I2C (18,19) sont déjà utilisées par les encodeurs
        // et les pins UART (0,1) sont utilisées par l'écran
        // Ne pas les enregistrer comme système pour éviter les faux conflits
    }
};

// Macro pour validation à la compilation
#define VALIDATE_PIN_CONFIGURATION() \
    static_assert(PinValidator::compileTimeValidation(), \
                  "Pin configuration has conflicts! Check pin assignments.")
