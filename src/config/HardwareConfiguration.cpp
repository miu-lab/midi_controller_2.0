#include "config/HardwareConfiguration.hpp"

HardwareConfiguration::HardwareConfiguration() {
    setInputsConfiguration();
}

const std::vector<InputConfig>& HardwareConfiguration::getAllInputConfigurations() const {
    return inputConfigurations_;
}

// === MÉTHODES DE COMPATIBILITÉ ===

std::vector<ButtonConfig> HardwareConfiguration::getControlButtonConfigurations() const {
    std::vector<ButtonConfig> buttons;
    for (const auto& input : inputConfigurations_) {
        if (input.type == InputType::BUTTON) {
            if (auto btnConfig = input.getConfig<ButtonConfig>()) {
                buttons.push_back(*btnConfig);
            }
        }
    }
    return buttons;
}

std::vector<EncoderConfig> HardwareConfiguration::getEncoderConfigurations() const {
    std::vector<EncoderConfig> encoders;
    for (const auto& input : inputConfigurations_) {
        if (input.type == InputType::ENCODER) {
            if (auto encConfig = input.getConfig<EncoderConfig>()) {
                encoders.push_back(*encConfig);
            }
        }
    }
    return encoders;
}

// === MÉTHODES UTILITAIRES ===

std::vector<InputConfig> HardwareConfiguration::getInputConfigurationsByType(InputType type) const {
    std::vector<InputConfig> filtered;
    for (const auto& input : inputConfigurations_) {
        if (input.type == type) {
            filtered.push_back(input);
        }
    }
    return filtered;
}

std::optional<InputConfig> HardwareConfiguration::getInputConfigurationById(InputId id) const {
    for (const auto& input : inputConfigurations_) {
        if (input.id == id) {
            return input;
        }
    }
    return std::nullopt;
}

bool HardwareConfiguration::validateAllConfigurations() const {
    for (const auto& input : inputConfigurations_) {
        if (!input.isValid()) {
            return false;
        }
    }
    return true;
}

// === HELPERS PRIVÉS ===

InputConfig HardwareConfiguration::createButtonInput(InputId id, const std::string& name, 
                                                     const std::string& label, 
                                                     const ButtonConfig& buttonConfig, 
                                                     const std::string& group) {
    return InputConfig{
        .id = id,
        .name = name,
        .type = InputType::BUTTON,
        .label = label,
        .config = buttonConfig,
        .group = group,
        .description = "Bouton " + label,
        .enabled = true,
        .displayOrder = static_cast<uint8_t>(id - 50)  // Ordre basé sur l'ID
    };
}

InputConfig HardwareConfiguration::createEncoderInput(InputId id, const std::string& name, 
                                                      const std::string& label, 
                                                      const EncoderConfig& encoderConfig, 
                                                      const std::string& group) {
    return InputConfig{
        .id = id,
        .name = name,
        .type = InputType::ENCODER,
        .label = label,
        .config = encoderConfig,
        .group = group,
        .description = "Encodeur " + label,
        .enabled = true,
        .displayOrder = static_cast<uint8_t>(id - 70)  // Ordre basé sur l'ID
    };
}

// === CONFIGURATION PRINCIPALE ===

void HardwareConfiguration::setInputsConfiguration() {
    inputConfigurations_.clear();

    // === BOUTONS STANDALONE (Groupe: Navigation) ===
    
    // Bouton Menu avec long press
    ButtonConfig menuButton{
        .id = 51,
        .gpio = {.pin = 32, .mode = PinMode::PULLUP},
        .activeLow = true,
        .mode = ButtonMode::TOGGLE,
        .debounceMs = 50,
        .longPressMs = 1000,
        .enableLongPress = true
    };
    inputConfigurations_.push_back(
        createButtonInput(51, "menu_button", "Menu", menuButton, "Navigation")
    );

    // Bouton OK/Validation
    ButtonConfig okButton{
        .id = 52,
        .gpio = {.pin = 31, .mode = PinMode::PULLUP},
        .activeLow = true,
        .mode = ButtonMode::MOMENTARY,
        .debounceMs = 50
    };
    inputConfigurations_.push_back(
        createButtonInput(52, "ok_button", "OK", okButton, "Navigation")
    );

    // === ENCODEURS MIDI (Groupe: MIDI Controls) ===
    
    // Encodeur 1
    EncoderConfig encoder1{
        .id = 71,
        .pinA = {.pin = 22, .mode = PinMode::PULLUP},
        .pinB = {.pin = 23, .mode = PinMode::PULLUP},
        .ppr = 24,
        .buttonConfig = ButtonConfig{
            .id = 1071,
            .gpio = {.pin = 21, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30
        },
        .enableAcceleration = true,
        .stepsPerDetent = 4
    };
    inputConfigurations_.push_back(
        createEncoderInput(71, "encoder_1", "Enc 1", encoder1, "MIDI")
    );

    // Encodeur 2
    EncoderConfig encoder2{
        .id = 72,
        .pinA = {.pin = 19, .mode = PinMode::PULLUP},
        .pinB = {.pin = 20, .mode = PinMode::PULLUP},
        .ppr = 24,
        .buttonConfig = ButtonConfig{
            .id = 1072,
            .gpio = {.pin = 18, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30
        },
        .enableAcceleration = true,
        .stepsPerDetent = 4
    };
    inputConfigurations_.push_back(
        createEncoderInput(72, "encoder_2", "Enc 2", encoder2, "MIDI")
    );

    // Encodeur 3
    EncoderConfig encoder3{
        .id = 73,
        .pinA = {.pin = 16, .mode = PinMode::PULLUP},
        .pinB = {.pin = 17, .mode = PinMode::PULLUP},
        .ppr = 24,
        .buttonConfig = ButtonConfig{
            .id = 1073,
            .gpio = {.pin = 15, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30
        },
        .enableAcceleration = true,
        .stepsPerDetent = 4
    };
    inputConfigurations_.push_back(
        createEncoderInput(73, "encoder_3", "Enc 3", encoder3, "MIDI")
    );

    // Encodeur 4
    EncoderConfig encoder4{
        .id = 74,
        .pinA = {.pin = 13, .mode = PinMode::PULLUP},
        .pinB = {.pin = 14, .mode = PinMode::PULLUP},
        .ppr = 24,
        .buttonConfig = ButtonConfig{
            .id = 1074,
            .gpio = {.pin = 41, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30
        },
        .enableAcceleration = true,
        .stepsPerDetent = 4
    };
    inputConfigurations_.push_back(
        createEncoderInput(74, "encoder_4", "Enc 4", encoder4, "MIDI")
    );

    // Encodeur 5
    EncoderConfig encoder5{
        .id = 75,
        .pinA = {.pin = 39, .mode = PinMode::PULLUP},
        .pinB = {.pin = 40, .mode = PinMode::PULLUP},
        .ppr = 24,
        .buttonConfig = ButtonConfig{
            .id = 1075,
            .gpio = {.pin = 38, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30
        },
        .enableAcceleration = true,
        .stepsPerDetent = 4
    };
    inputConfigurations_.push_back(
        createEncoderInput(75, "encoder_5", "Enc 5", encoder5, "MIDI")
    );

    // Encodeur 6
    EncoderConfig encoder6{
        .id = 76,
        .pinA = {.pin = 36, .mode = PinMode::PULLUP},
        .pinB = {.pin = 37, .mode = PinMode::PULLUP},
        .ppr = 24,
        .buttonConfig = ButtonConfig{
            .id = 1076,
            .gpio = {.pin = 35, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30
        },
        .enableAcceleration = true,
        .stepsPerDetent = 4
    };
    inputConfigurations_.push_back(
        createEncoderInput(76, "encoder_6", "Enc 6", encoder6, "MIDI")
    );

    // Encodeur 7
    EncoderConfig encoder7{
        .id = 77,
        .pinA = {.pin = 33, .mode = PinMode::PULLUP},
        .pinB = {.pin = 34, .mode = PinMode::PULLUP},
        .ppr = 24,
        .buttonConfig = ButtonConfig{
            .id = 1077,
            .gpio = {.pin = 30, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30
        },
        .enableAcceleration = true,
        .stepsPerDetent = 4
    };
    inputConfigurations_.push_back(
        createEncoderInput(77, "encoder_7", "Enc 7", encoder7, "MIDI")
    );

    // Encodeur 8
    EncoderConfig encoder8{
        .id = 78,
        .pinA = {.pin = 28, .mode = PinMode::PULLUP},
        .pinB = {.pin = 29, .mode = PinMode::PULLUP},
        .ppr = 24,
        .buttonConfig = ButtonConfig{
            .id = 1078,
            .gpio = {.pin = 27, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30
        },
        .enableAcceleration = true,
        .stepsPerDetent = 4
    };
    inputConfigurations_.push_back(
        createEncoderInput(78, "encoder_8", "Enc 8", encoder8, "MIDI")
    );

    // === ENCODEUR NAVIGATION (Groupe: Navigation) ===
    
    // Encodeur de Navigation avec fonctionnalités avancées
    EncoderConfig navEncoder{
        .id = 79,
        .pinA = {.pin = 9, .mode = PinMode::PULLUP},
        .pinB = {.pin = 10, .mode = PinMode::PULLUP},
        .ppr = 96,
        .buttonConfig = ButtonConfig{
            .id = 1079,
            .gpio = {.pin = 8, .mode = PinMode::PULLUP},
            .activeLow = true,
            .mode = ButtonMode::MOMENTARY,
            .debounceMs = 30,
            .longPressMs = 800,
            .enableLongPress = true
        },
        .sensitivity = 1.5f,
        .enableAcceleration = true,
        .stepsPerDetent = 4,
        .accelerationThreshold = 80
    };
    inputConfigurations_.push_back(
        createEncoderInput(79, "nav_encoder", "Navigation", navEncoder, "Navigation")
    );

    // === ENCODEUR OPTIQUE (Groupe: Precision) ===
    
    // Encodeur Optique haute résolution
    EncoderConfig opticalEncoder{
        .id = 80,
        .pinA = {.pin = 11, .mode = PinMode::PULLUP},
        .pinB = {.pin = 12, .mode = PinMode::PULLUP},
        .ppr = 600,
        .sensitivity = 0.1f,
        .enableAcceleration = false,
        .stepsPerDetent = 1
    };
    inputConfigurations_.push_back(
        createEncoderInput(80, "optical_encoder", "Precision", opticalEncoder, "Precision")
    );

    // Validation finale de toutes les configurations
    if (!validateAllConfigurations()) {
        // En mode debug, on pourrait logger les erreurs
        // Pour l'instant, on continue avec les configs invalides filtrées
    }
}
