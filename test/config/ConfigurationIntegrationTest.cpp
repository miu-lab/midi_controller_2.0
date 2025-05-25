/**
 * @file ConfigurationIntegrationTest.cpp
 * @brief Tests d'intégration pour vérifier la cohérence Hardware/Mapping
 * 
 * Ces tests vérifient que HardwareConfiguration et MappingConfiguration
 * sont cohérents et fonctionnent ensemble correctement.
 */

#include <unity.h>
#include <memory>
#include <set>
#include "config/HardwareConfiguration.hpp"
#include "config/MappingConfiguration.hpp"

// Variables globales pour les tests
std::unique_ptr<HardwareConfiguration> hwConfig;
std::unique_ptr<MappingConfiguration> mapConfig;

void setUp() {
    hwConfig = std::make_unique<HardwareConfiguration>();
    mapConfig = std::make_unique<MappingConfiguration>();
}

void tearDown() {
    hwConfig.reset();
    mapConfig.reset();
}

// === TESTS DE COHÉRENCE DES IDS ===

void test_all_hardware_inputs_have_mappings() {
    const auto& inputs = hwConfig->getAllInputConfigurations();
    const auto& mappings = mapConfig->getMappedControls();
    
    // Créer un set de tous les IDs mappés
    std::set<InputId> mappedIds;
    for (const auto& mapping : mappings) {
        mappedIds.insert(mapping.controlId);
    }
    
    // Vérifier que chaque input hardware a un mapping
    for (const auto& input : inputs) {
        // Les encodeurs doivent avoir un mapping
        if (input.type == InputType::ENCODER) {
            TEST_ASSERT_TRUE_MESSAGE(
                mappedIds.count(input.id) > 0,
                "Encoder without mapping found"
            );
            
            // Si l'encodeur a un bouton, il doit aussi avoir un mapping
            auto encConfig = input.getConfig<EncoderConfig>();
            if (encConfig && encConfig->buttonConfig) {
                InputId buttonId = encConfig->buttonConfig->id;
                TEST_ASSERT_TRUE_MESSAGE(
                    mappedIds.count(buttonId) > 0,
                    "Encoder button without mapping found"
                );
            }
        }
        // Les boutons standalone doivent avoir un mapping
        else if (input.type == InputType::BUTTON) {
            TEST_ASSERT_TRUE_MESSAGE(
                mappedIds.count(input.id) > 0,
                "Button without mapping found"
            );
        }
    }
}

void test_no_orphan_mappings() {
    const auto& mappings = mapConfig->getMappedControls();
    
    // Créer un set de tous les IDs hardware
    std::set<InputId> hardwareIds;
    
    // Ajouter tous les IDs des inputs
    const auto& inputs = hwConfig->getAllInputConfigurations();
    for (const auto& input : inputs) {
        hardwareIds.insert(input.id);
        
        // Ajouter aussi les IDs des boutons d'encodeurs
        if (input.type == InputType::ENCODER) {
            auto encConfig = input.getConfig<EncoderConfig>();
            if (encConfig && encConfig->buttonConfig) {
                hardwareIds.insert(encConfig->buttonConfig->id);
            }
        }
    }
    
    // Vérifier que chaque mapping correspond à un hardware existant
    for (const auto& mapping : mappings) {
        TEST_ASSERT_TRUE_MESSAGE(
            hardwareIds.count(mapping.controlId) > 0,
            "Mapping without corresponding hardware found"
        );
    }
}

// === TESTS DE LA CONVENTION 1000+ ===

void test_encoder_button_convention_consistency() {
    // Pour chaque encodeur avec bouton, vérifier la cohérence
    const auto& inputs = hwConfig->getAllInputConfigurations();
    
    for (const auto& input : inputs) {
        if (input.type == InputType::ENCODER) {
            auto encConfig = input.getConfig<EncoderConfig>();
            if (encConfig && encConfig->buttonConfig) {
                InputId encoderId = input.id;
                InputId buttonId = encConfig->buttonConfig->id;
                
                // Vérifier la convention
                TEST_ASSERT_EQUAL(encoderId + 1000, buttonId);
                
                // Vérifier que les deux ont des mappings du bon type
                auto encMapping = mapConfig->getMidiMappingForControl(
                    encoderId, MappingControlType::ENCODER);
                auto btnMapping = mapConfig->getMidiMappingForControl(
                    buttonId, MappingControlType::BUTTON);
                
                // Au moins un des deux doit exister
                TEST_ASSERT_TRUE(encMapping != nullptr || 
                               mapConfig->isNavigationControl(encoderId) ||
                               btnMapping != nullptr ||
                               mapConfig->isNavigationControl(buttonId));
            }
        }
    }
}

// === TESTS DES TYPES DE CONTRÔLES ===

void test_mapping_types_match_hardware_types() {
    const auto& mappings = mapConfig->getMappedControls();
    
    for (const auto& mapping : mappings) {
        auto hwInput = hwConfig->getInputConfigurationById(mapping.controlId);
        
        if (hwInput) {
            // Le type de mapping doit correspondre au type hardware
            if (hwInput->type == InputType::ENCODER) {
                TEST_ASSERT_EQUAL(MappingControlType::ENCODER, mapping.mappingType);
            } else if (hwInput->type == InputType::BUTTON) {
                TEST_ASSERT_EQUAL(MappingControlType::BUTTON, mapping.mappingType);
            }
        } else {
            // C'est peut-être un bouton d'encodeur (ID > 1000)
            if (mapping.controlId > 1000) {
                InputId encoderId = mapping.controlId - 1000;
                auto encoder = hwConfig->getInputConfigurationById(encoderId);
                TEST_ASSERT_TRUE(encoder.has_value());
                TEST_ASSERT_EQUAL(MappingControlType::BUTTON, mapping.mappingType);
            }
        }
    }
}

// === TESTS DES GROUPES NAVIGATION ===

void test_navigation_controls_are_in_navigation_group() {
    // Les contrôles de navigation doivent être dans le groupe "Navigation"
    std::vector<InputId> navIds = {51, 52, 79};  // Menu, OK, Nav encoder
    
    for (auto id : navIds) {
        auto input = hwConfig->getInputConfigurationById(id);
        TEST_ASSERT_TRUE(input.has_value());
        TEST_ASSERT_EQUAL_STRING("Navigation", input->group.c_str());
        TEST_ASSERT_TRUE(mapConfig->isNavigationControl(id));
    }
}

void test_midi_controls_are_not_navigation() {
    // Les encodeurs MIDI ne doivent pas être navigation
    for (InputId id = 71; id <= 78; id++) {
        auto input = hwConfig->getInputConfigurationById(id);
        TEST_ASSERT_TRUE(input.has_value());
        TEST_ASSERT_EQUAL_STRING("MIDI", input->group.c_str());
        TEST_ASSERT_FALSE(mapConfig->isNavigationControl(id));
    }
}

// === TESTS DES PINS UNIQUES ===

void test_no_pin_conflicts_between_configs() {
    std::set<uint8_t> usedPins;
    const auto& inputs = hwConfig->getAllInputConfigurations();
    
    for (const auto& input : inputs) {
        if (input.type == InputType::ENCODER) {
            auto config = input.getConfig<EncoderConfig>();
            if (config) {
                // Chaque pin doit être unique
                TEST_ASSERT_TRUE(usedPins.insert(config->pinA.pin).second);
                TEST_ASSERT_TRUE(usedPins.insert(config->pinB.pin).second);
                
                if (config->buttonConfig) {
                    TEST_ASSERT_TRUE(usedPins.insert(config->buttonConfig->gpio.pin).second);
                }
            }
        } else if (input.type == InputType::BUTTON) {
            auto config = input.getConfig<ButtonConfig>();
            if (config) {
                TEST_ASSERT_TRUE(usedPins.insert(config->gpio.pin).second);
            }
        }
    }
}

// === TESTS DE PERFORMANCE ===

void test_configuration_access_performance() {
    // Tester que l'accès aux configurations est rapide
    const int iterations = 1000;
    
    // Test de recherche hardware par ID
    unsigned long start = millis();
    for (int i = 0; i < iterations; i++) {
        auto config = hwConfig->getInputConfigurationById(75);
        TEST_ASSERT_TRUE(config.has_value());
    }
    unsigned long hwTime = millis() - start;
    
    // Test de recherche mapping
    start = millis();
    for (int i = 0; i < iterations; i++) {
        auto mapping = mapConfig->getMidiMappingForControl(75, MappingControlType::ENCODER);
        TEST_ASSERT_NOT_NULL(mapping);
    }
    unsigned long mapTime = millis() - start;
    
    // Les deux devraient être rapides (< 100ms pour 1000 itérations)
    TEST_ASSERT_LESS_THAN(100, hwTime);
    TEST_ASSERT_LESS_THAN(100, mapTime);
}

// === TESTS DE VALIDITÉ COMPLÈTE ===

void test_complete_system_validity() {
    // Test global de cohérence
    TEST_ASSERT_TRUE(hwConfig->validateAllConfigurations());
    
    // Compter les éléments
    const auto& inputs = hwConfig->getAllInputConfigurations();
    const auto& mappings = mapConfig->getMappedControls();
    
    // On devrait avoir au moins autant de mappings que d'inputs
    // (car les encodeurs ont aussi leurs boutons)
    TEST_ASSERT_GREATER_OR_EQUAL(inputs.size(), mappings.size());
    
    // Vérifier les totaux attendus
    auto encoders = hwConfig->getInputConfigurationsByType(InputType::ENCODER);
    auto buttons = hwConfig->getInputConfigurationsByType(InputType::BUTTON);
    
    TEST_ASSERT_EQUAL(10, encoders.size());  // 8 MIDI + 1 nav + 1 optique
    TEST_ASSERT_EQUAL(2, buttons.size());    // Menu + OK
    
    // Vérifier qu'on a des mappings pour tous les types
    int midiMappings = 0;
    int navMappings = 0;
    
    for (const auto& mapping : mappings) {
        if (mapping.roles.count(MappingRole::MIDI) > 0) midiMappings++;
        if (mapping.roles.count(MappingRole::NAVIGATION) > 0) navMappings++;
    }
    
    TEST_ASSERT_GREATER_THAN(0, midiMappings);
    TEST_ASSERT_GREATER_THAN(0, navMappings);
}

// === FONCTION PRINCIPALE DES TESTS ===

void run_configuration_integration_tests() {
    UNITY_BEGIN();
    
    // Tests de cohérence
    RUN_TEST(test_all_hardware_inputs_have_mappings);
    RUN_TEST(test_no_orphan_mappings);
    
    // Tests convention 1000+
    RUN_TEST(test_encoder_button_convention_consistency);
    
    // Tests des types
    RUN_TEST(test_mapping_types_match_hardware_types);
    
    // Tests navigation
    RUN_TEST(test_navigation_controls_are_in_navigation_group);
    RUN_TEST(test_midi_controls_are_not_navigation);
    
    // Tests pins
    RUN_TEST(test_no_pin_conflicts_between_configs);
    
    // Tests performance
    RUN_TEST(test_configuration_access_performance);
    
    // Test global
    RUN_TEST(test_complete_system_validity);
    
    UNITY_END();
}

// Pour PlatformIO
#ifndef NATIVE
void setup() {
    delay(2000);  // Délai pour le moniteur série
    run_configuration_integration_tests();
}

void loop() {}
#endif
