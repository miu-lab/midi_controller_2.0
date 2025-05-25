/**
 * @file HardwareConfigurationTest.cpp
 * @brief Tests de non-régression pour HardwareConfiguration
 * 
 * Ces tests capturent le comportement actuel de HardwareConfiguration
 * pour garantir que la refactorisation ne casse rien.
 */

#include <unity.h>
#include <vector>
#include <optional>
#include "config/HardwareConfiguration.hpp"

// Variables globales pour les tests
HardwareConfiguration* hwConfig;

void setUp() {
    hwConfig = new HardwareConfiguration();
}

void tearDown() {
    delete hwConfig;
}

// === TESTS DE BASE ===

void test_hardware_configuration_creates_successfully() {
    TEST_ASSERT_NOT_NULL(hwConfig);
}

void test_get_all_configurations_returns_expected_count() {
    const auto& configs = hwConfig->getAllInputConfigurations();
    // 8 encodeurs MIDI + 1 encodeur nav + 1 encodeur optique + 2 boutons = 12
    TEST_ASSERT_EQUAL(12, configs.size());
}

void test_validate_all_configurations_returns_true() {
    TEST_ASSERT_TRUE(hwConfig->validateAllConfigurations());
}

// === TESTS DES ENCODEURS ===

void test_encoder_configurations_have_correct_ids() {
    auto encoders = hwConfig->getInputConfigurationsByType(InputType::ENCODER);
    TEST_ASSERT_EQUAL(10, encoders.size());
    
    // Vérifier les IDs des encodeurs MIDI (71-78)
    std::vector<InputId> expectedIds = {71, 72, 73, 74, 75, 76, 77, 78, 79, 80};
    
    for (size_t i = 0; i < encoders.size(); i++) {
        bool found = false;
        for (auto id : expectedIds) {
            if (encoders[i].id == id) {
                found = true;
                break;
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(found, "Encoder ID not found in expected list");
    }
}

void test_midi_encoders_have_button_configs() {
    // Vérifier que les encodeurs MIDI (71-78) ont des boutons
    for (InputId id = 71; id <= 78; id++) {
        auto config = hwConfig->getInputConfigurationById(id);
        TEST_ASSERT_TRUE(config.has_value());
        
        auto encoderConfig = config->getConfig<EncoderConfig>();
        TEST_ASSERT_TRUE(encoderConfig.has_value());
        TEST_ASSERT_TRUE(encoderConfig->buttonConfig.has_value());
        
        // Vérifier la convention ID bouton = ID encodeur + 1000
        TEST_ASSERT_EQUAL(id + 1000, encoderConfig->buttonConfig->id);
    }
}

void test_encoder_button_convention_1000_plus() {
    // Test spécifique de la convention 1000+
    std::vector<std::pair<InputId, InputId>> expectedPairs = {
        {71, 1071}, {72, 1072}, {73, 1073}, {74, 1074},
        {75, 1075}, {76, 1076}, {77, 1077}, {78, 1078}, {79, 1079}
    };
    
    for (const auto& [encoderId, buttonId] : expectedPairs) {
        auto config = hwConfig->getInputConfigurationById(encoderId);
        TEST_ASSERT_TRUE(config.has_value());
        
        auto encoderConfig = config->getConfig<EncoderConfig>();
        TEST_ASSERT_TRUE(encoderConfig.has_value());
        
        if (encoderConfig->buttonConfig.has_value()) {
            TEST_ASSERT_EQUAL(buttonId, encoderConfig->buttonConfig->id);
        }
    }
}

void test_navigation_encoder_has_special_properties() {
    auto navEncoder = hwConfig->getInputConfigurationById(79);
    TEST_ASSERT_TRUE(navEncoder.has_value());
    
    auto config = navEncoder->getConfig<EncoderConfig>();
    TEST_ASSERT_TRUE(config.has_value());
    
    // Vérifier les propriétés spéciales
    TEST_ASSERT_EQUAL(96, config->ppr);  // Plus haute résolution
    TEST_ASSERT_EQUAL_FLOAT(1.5f, config->sensitivity);
    TEST_ASSERT_TRUE(config->enableAcceleration);
    TEST_ASSERT_EQUAL(80, config->accelerationThreshold);
    
    // Le bouton a long press
    TEST_ASSERT_TRUE(config->buttonConfig.has_value());
    TEST_ASSERT_TRUE(config->buttonConfig->enableLongPress);
    TEST_ASSERT_EQUAL(800, config->buttonConfig->longPressMs);
}

void test_optical_encoder_has_high_resolution() {
    auto opticalEncoder = hwConfig->getInputConfigurationById(80);
    TEST_ASSERT_TRUE(opticalEncoder.has_value());
    
    auto config = opticalEncoder->getConfig<EncoderConfig>();
    TEST_ASSERT_TRUE(config.has_value());
    
    // Vérifier la haute résolution
    TEST_ASSERT_EQUAL(600, config->ppr);
    TEST_ASSERT_EQUAL_FLOAT(0.1f, config->sensitivity);
    TEST_ASSERT_FALSE(config->enableAcceleration);
    TEST_ASSERT_FALSE(config->buttonConfig.has_value());  // Pas de bouton
}

// === TESTS DES BOUTONS ===

void test_standalone_buttons_configuration() {
    auto buttons = hwConfig->getInputConfigurationsByType(InputType::BUTTON);
    TEST_ASSERT_EQUAL(2, buttons.size());
    
    // Vérifier les boutons Menu (51) et OK (52)
    auto menuButton = hwConfig->getInputConfigurationById(51);
    TEST_ASSERT_TRUE(menuButton.has_value());
    TEST_ASSERT_EQUAL_STRING("menu_button", menuButton->name.c_str());
    TEST_ASSERT_EQUAL_STRING("Menu", menuButton->label.c_str());
    
    auto okButton = hwConfig->getInputConfigurationById(52);
    TEST_ASSERT_TRUE(okButton.has_value());
    TEST_ASSERT_EQUAL_STRING("ok_button", okButton->name.c_str());
    TEST_ASSERT_EQUAL_STRING("OK", okButton->label.c_str());
}

void test_menu_button_has_long_press() {
    auto menuButton = hwConfig->getInputConfigurationById(51);
    TEST_ASSERT_TRUE(menuButton.has_value());
    
    auto config = menuButton->getConfig<ButtonConfig>();
    TEST_ASSERT_TRUE(config.has_value());
    TEST_ASSERT_TRUE(config->enableLongPress);
    TEST_ASSERT_EQUAL(1000, config->longPressMs);
    TEST_ASSERT_EQUAL(ButtonMode::TOGGLE, config->mode);
}

// === TESTS DES GROUPES ===

void test_input_groups_are_correct() {
    // Vérifier les groupes
    auto checkGroup = [this](InputId id, const char* expectedGroup) {
        auto config = hwConfig->getInputConfigurationById(id);
        TEST_ASSERT_TRUE(config.has_value());
        TEST_ASSERT_EQUAL_STRING(expectedGroup, config->group.c_str());
    };
    
    // Encodeurs MIDI
    for (InputId id = 71; id <= 78; id++) {
        checkGroup(id, "MIDI");
    }
    
    // Navigation
    checkGroup(51, "Navigation");  // Menu button
    checkGroup(52, "Navigation");  // OK button
    checkGroup(79, "Navigation");  // Nav encoder
    
    // Precision
    checkGroup(80, "Precision");   // Optical encoder
}

// === TESTS DES PINS ===

void test_encoder_pins_are_valid() {
    auto encoders = hwConfig->getInputConfigurationsByType(InputType::ENCODER);
    
    for (const auto& input : encoders) {
        auto config = input.getConfig<EncoderConfig>();
        TEST_ASSERT_TRUE(config.has_value());
        
        // Vérifier que les pins sont valides (0-99 pour Teensy)
        TEST_ASSERT_TRUE(config->pinA.isValid());
        TEST_ASSERT_TRUE(config->pinB.isValid());
        
        // Vérifier le mode des pins
        TEST_ASSERT_EQUAL(PinMode::PULLUP, config->pinA.mode);
        TEST_ASSERT_EQUAL(PinMode::PULLUP, config->pinB.mode);
    }
}

void test_no_duplicate_pins() {
    std::vector<uint8_t> usedPins;
    
    // Collecter tous les pins utilisés
    auto encoders = hwConfig->getInputConfigurationsByType(InputType::ENCODER);
    for (const auto& input : encoders) {
        auto config = input.getConfig<EncoderConfig>();
        if (config) {
            usedPins.push_back(config->pinA.pin);
            usedPins.push_back(config->pinB.pin);
            if (config->buttonConfig) {
                usedPins.push_back(config->buttonConfig->gpio.pin);
            }
        }
    }
    
    auto buttons = hwConfig->getInputConfigurationsByType(InputType::BUTTON);
    for (const auto& input : buttons) {
        auto config = input.getConfig<ButtonConfig>();
        if (config) {
            usedPins.push_back(config->gpio.pin);
        }
    }
    
    // Vérifier qu'il n'y a pas de doublons
    std::sort(usedPins.begin(), usedPins.end());
    auto last = std::unique(usedPins.begin(), usedPins.end());
    
    TEST_ASSERT_EQUAL(usedPins.size(), std::distance(usedPins.begin(), last));
}

// === TESTS DE MÉTADONNÉES ===

void test_display_order_follows_convention() {
    // Boutons : displayOrder = id - 50
    auto menuButton = hwConfig->getInputConfigurationById(51);
    TEST_ASSERT_TRUE(menuButton.has_value());
    TEST_ASSERT_EQUAL(1, menuButton->displayOrder);  // 51 - 50 = 1
    
    auto okButton = hwConfig->getInputConfigurationById(52);
    TEST_ASSERT_TRUE(okButton.has_value());
    TEST_ASSERT_EQUAL(2, okButton->displayOrder);    // 52 - 50 = 2
    
    // Encodeurs : displayOrder = id - 70
    auto encoder1 = hwConfig->getInputConfigurationById(71);
    TEST_ASSERT_TRUE(encoder1.has_value());
    TEST_ASSERT_EQUAL(1, encoder1->displayOrder);    // 71 - 70 = 1
}

// === FONCTION PRINCIPALE DES TESTS ===

void run_hardware_configuration_tests() {
    UNITY_BEGIN();
    
    // Tests de base
    RUN_TEST(test_hardware_configuration_creates_successfully);
    RUN_TEST(test_get_all_configurations_returns_expected_count);
    RUN_TEST(test_validate_all_configurations_returns_true);
    
    // Tests des encodeurs
    RUN_TEST(test_encoder_configurations_have_correct_ids);
    RUN_TEST(test_midi_encoders_have_button_configs);
    RUN_TEST(test_encoder_button_convention_1000_plus);
    RUN_TEST(test_navigation_encoder_has_special_properties);
    RUN_TEST(test_optical_encoder_has_high_resolution);
    
    // Tests des boutons
    RUN_TEST(test_standalone_buttons_configuration);
    RUN_TEST(test_menu_button_has_long_press);
    
    // Tests des groupes
    RUN_TEST(test_input_groups_are_correct);
    
    // Tests des pins
    RUN_TEST(test_encoder_pins_are_valid);
    RUN_TEST(test_no_duplicate_pins);
    
    // Tests de métadonnées
    RUN_TEST(test_display_order_follows_convention);
    
    UNITY_END();
}

// Pour PlatformIO
#ifndef NATIVE
void setup() {
    delay(2000);  // Délai pour le moniteur série
    run_hardware_configuration_tests();
}

void loop() {}
#endif
