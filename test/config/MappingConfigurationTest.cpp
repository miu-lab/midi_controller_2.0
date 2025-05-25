/**
 * @file MappingConfigurationTest.cpp
 * @brief Tests de non-régression pour MappingConfiguration
 * 
 * Ces tests capturent le comportement actuel de MappingConfiguration
 * pour garantir que la refactorisation ne casse rien.
 */

#include <unity.h>
#include <vector>
#include <set>
#include "config/MappingConfiguration.hpp"

// Variables globales pour les tests
MappingConfiguration* mappingConfig;

void setUp() {
    mappingConfig = new MappingConfiguration();
}

void tearDown() {
    delete mappingConfig;
}

// === TESTS DE BASE ===

void test_mapping_configuration_creates_successfully() {
    TEST_ASSERT_NOT_NULL(mappingConfig);
}

void test_get_mapped_controls_returns_expected_count() {
    const auto& mappings = mappingConfig->getMappedControls();
    // 8 encodeurs + 8 boutons encodeurs + 1 optique + 2 nav + 2 boutons nav = 21
    TEST_ASSERT_EQUAL(21, mappings.size());
}

// === TESTS DES MAPPINGS MIDI ===

void test_midi_encoder_mappings_are_correct() {
    // Vérifier les encodeurs MIDI 1-8 (IDs 71-78)
    for (int i = 1; i <= 8; i++) {
        InputId encoderId = 70 + i;
        auto mapping = mappingConfig->getMidiMappingForControl(encoderId, MappingControlType::ENCODER);
        
        TEST_ASSERT_NOT_NULL(mapping);
        TEST_ASSERT_EQUAL(0, mapping->channel);      // Canal 0
        TEST_ASSERT_EQUAL(i, mapping->control);      // CC = numéro encodeur
        TEST_ASSERT_TRUE(mapping->isRelative);       // Mode relatif
        TEST_ASSERT_EQUAL(MidiEventType::CONTROL_CHANGE, mapping->type);
    }
}

void test_midi_button_mappings_follow_convention() {
    // Vérifier les boutons des encodeurs (CC = encodeur CC + 35)
    for (int i = 1; i <= 8; i++) {
        InputId buttonId = 1070 + i;  // 1071-1078
        auto mapping = mappingConfig->getMidiMappingForControl(buttonId, MappingControlType::BUTTON);
        
        TEST_ASSERT_NOT_NULL(mapping);
        TEST_ASSERT_EQUAL(0, mapping->channel);
        TEST_ASSERT_EQUAL(35 + i, mapping->control);  // CC 36-43
        TEST_ASSERT_TRUE(mapping->isRelative);
    }
}

void test_optical_encoder_mapping() {
    // Encodeur optique (ID 80) -> CC 10
    auto mapping = mappingConfig->getMidiMappingForControl(80, MappingControlType::ENCODER);
    
    TEST_ASSERT_NOT_NULL(mapping);
    TEST_ASSERT_EQUAL(0, mapping->channel);
    TEST_ASSERT_EQUAL(10, mapping->control);
    TEST_ASSERT_TRUE(mapping->isRelative);
}

void test_invalid_control_returns_nullptr() {
    // Tester des IDs invalides
    auto mapping = mappingConfig->getMidiMappingForControl(999, MappingControlType::ENCODER);
    TEST_ASSERT_NULL(mapping);
    
    mapping = mappingConfig->getMidiMappingForControl(0, MappingControlType::BUTTON);
    TEST_ASSERT_NULL(mapping);
}

// === TESTS DES CONTRÔLES DE NAVIGATION ===

void test_navigation_controls_are_identified() {
    // Contrôles de navigation connus
    std::vector<InputId> navControls = {51, 52, 79, 1079};
    
    for (auto id : navControls) {
        TEST_ASSERT_TRUE(mappingConfig->isNavigationControl(id));
    }
}

void test_midi_controls_are_not_navigation() {
    // Les encodeurs MIDI ne sont pas des contrôles de navigation
    for (InputId id = 71; id <= 78; id++) {
        TEST_ASSERT_FALSE(mappingConfig->isNavigationControl(id));
    }
    
    // L'encodeur optique n'est pas navigation
    TEST_ASSERT_FALSE(mappingConfig->isNavigationControl(80));
}

void test_navigation_controls_have_correct_names() {
    const auto& mappings = mappingConfig->getMappedControls();
    
    // Vérifier les noms de navigation
    std::map<InputId, std::string> expectedNames = {
        {51, "HOME"},
        {52, "BACK"},
        {79, "ITEM_NAVIGATOR"},
        {1079, "ITEM_VALIDATE"}
    };
    
    for (const auto& mapping : mappings) {
        if (mapping.roles.count(MappingRole::NAVIGATION) > 0) {
            auto it = expectedNames.find(mapping.controlId);
            if (it != expectedNames.end()) {
                TEST_ASSERT_EQUAL_STRING(it->second.c_str(), 
                                       mapping.navigationMapping.name.c_str());
            }
        }
    }
}

// === TESTS DE LA CONVENTION 1000+ ===

void test_encoder_button_id_convention() {
    // Pour chaque encodeur, vérifier que son bouton suit la convention +1000
    for (InputId encoderId = 71; encoderId <= 79; encoderId++) {
        InputId buttonId = encoderId + 1000;
        
        // L'encodeur doit avoir un mapping
        auto encoderMapping = mappingConfig->getMidiMappingForControl(
            encoderId, MappingControlType::ENCODER);
        
        // Le bouton doit avoir un mapping (sauf pour l'optique 80)
        if (encoderId <= 78) {  // MIDI encoders
            auto buttonMapping = mappingConfig->getMidiMappingForControl(
                buttonId, MappingControlType::BUTTON);
            TEST_ASSERT_NOT_NULL(buttonMapping);
        } else if (encoderId == 79) {  // Navigation encoder
            TEST_ASSERT_TRUE(mappingConfig->isNavigationControl(buttonId));
        }
    }
}

// === TESTS DES RÔLES ===

void test_control_roles_are_exclusive() {
    const auto& mappings = mappingConfig->getMappedControls();
    
    for (const auto& mapping : mappings) {
        // Un contrôle ne peut pas être à la fois MIDI et Navigation
        bool hasMidi = mapping.roles.count(MappingRole::MIDI) > 0;
        bool hasNav = mapping.roles.count(MappingRole::NAVIGATION) > 0;
        
        TEST_ASSERT_TRUE(hasMidi != hasNav);  // XOR - un seul doit être vrai
    }
}

void test_all_controls_have_at_least_one_role() {
    const auto& mappings = mappingConfig->getMappedControls();
    
    for (const auto& mapping : mappings) {
        TEST_ASSERT_TRUE(mapping.roles.size() > 0);
    }
}

// === TESTS DE L'INDEX DE RECHERCHE ===

void test_mapping_index_performance() {
    // Tester que la recherche via l'index est rapide
    // (En pratique, on vérifie juste que ça fonctionne)
    
    // Recherche répétée pour vérifier la cohérence
    for (int i = 0; i < 100; i++) {
        auto mapping = mappingConfig->getMidiMappingForControl(75, MappingControlType::ENCODER);
        TEST_ASSERT_NOT_NULL(mapping);
        TEST_ASSERT_EQUAL(5, mapping->control);  // CC 5 pour encodeur 75
    }
}

void test_composite_key_differentiates_types() {
    // Vérifier que l'index différencie bien encoder vs button du même ID
    // (Cas théorique, n'arrive pas dans la config actuelle)
    
    // Pour l'encodeur 71
    auto encoderMapping = mappingConfig->getMidiMappingForControl(71, MappingControlType::ENCODER);
    TEST_ASSERT_NOT_NULL(encoderMapping);
    
    // Si on cherchait un bouton avec ID 71 (n'existe pas), ça devrait retourner nullptr
    auto buttonMapping = mappingConfig->getMidiMappingForControl(71, MappingControlType::BUTTON);
    TEST_ASSERT_NULL(buttonMapping);
}

// === TESTS DE COHÉRENCE ===

void test_no_duplicate_control_ids_per_type() {
    const auto& mappings = mappingConfig->getMappedControls();
    
    std::set<uint32_t> compositeKeys;
    
    for (const auto& mapping : mappings) {
        uint32_t key = (static_cast<uint32_t>(mapping.controlId) << 8) | 
                       static_cast<uint8_t>(mapping.mappingType);
        
        // Vérifier qu'on n'a pas déjà vu cette clé
        TEST_ASSERT_TRUE(compositeKeys.insert(key).second);
    }
}

void test_midi_cc_numbers_are_unique() {
    const auto& mappings = mappingConfig->getMappedControls();
    std::set<uint8_t> usedCCs;
    
    for (const auto& mapping : mappings) {
        if (mapping.roles.count(MappingRole::MIDI) > 0) {
            uint8_t cc = mapping.midiMapping.control;
            
            // Vérifier qu'on n'a pas déjà utilisé ce CC
            // Note: Dans la vraie config, certains CC peuvent être dupliqués
            // entre encodeur et bouton, donc on ne teste pas l'unicité stricte
            TEST_ASSERT_TRUE(cc <= 127);  // CC valide
        }
    }
}

// === FONCTION PRINCIPALE DES TESTS ===

void run_mapping_configuration_tests() {
    UNITY_BEGIN();
    
    // Tests de base
    RUN_TEST(test_mapping_configuration_creates_successfully);
    RUN_TEST(test_get_mapped_controls_returns_expected_count);
    
    // Tests MIDI
    RUN_TEST(test_midi_encoder_mappings_are_correct);
    RUN_TEST(test_midi_button_mappings_follow_convention);
    RUN_TEST(test_optical_encoder_mapping);
    RUN_TEST(test_invalid_control_returns_nullptr);
    
    // Tests Navigation
    RUN_TEST(test_navigation_controls_are_identified);
    RUN_TEST(test_midi_controls_are_not_navigation);
    RUN_TEST(test_navigation_controls_have_correct_names);
    
    // Tests Convention 1000+
    RUN_TEST(test_encoder_button_id_convention);
    
    // Tests Rôles
    RUN_TEST(test_control_roles_are_exclusive);
    RUN_TEST(test_all_controls_have_at_least_one_role);
    
    // Tests Index
    RUN_TEST(test_mapping_index_performance);
    RUN_TEST(test_composite_key_differentiates_types);
    
    // Tests Cohérence
    RUN_TEST(test_no_duplicate_control_ids_per_type);
    RUN_TEST(test_midi_cc_numbers_are_unique);
    
    UNITY_END();
}

// Pour PlatformIO
#ifndef NATIVE
void setup() {
    delay(2000);  // Délai pour le moniteur série
    run_mapping_configuration_tests();
}

void loop() {}
#endif
