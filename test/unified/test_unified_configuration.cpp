#include <unity.h>
#include "config/unified/UnifiedConfiguration.hpp"
#include "config/unified/ConfigurationFactory.hpp"
#include "config/unified/ControlBuilder.hpp"

// === TESTS CONTROLBUILDER ===

void test_control_builder_basic_encoder() {
    auto control = ControlBuilder(71, "test_encoder")
        .withLabel("Test Encoder")
        .inGroup("Test")
        .asEncoder(22, 23, 24)
        .build();
    
    TEST_ASSERT_EQUAL(71, control.id);
    TEST_ASSERT_EQUAL_STRING("test_encoder", control.name.c_str());
    TEST_ASSERT_EQUAL_STRING("Test Encoder", control.label.c_str());
    TEST_ASSERT_EQUAL_STRING("Test", control.group.c_str());
    TEST_ASSERT_EQUAL(InputType::ENCODER, control.hardware.type);
    
    auto& enc = std::get<ControlDefinition::EncoderConfig>(control.hardware.config);
    TEST_ASSERT_EQUAL(22, enc.pinA.pin);
    TEST_ASSERT_EQUAL(23, enc.pinB.pin);
    TEST_ASSERT_EQUAL(24, enc.ppr);
}

void test_control_builder_encoder_with_button() {
    auto control = ControlBuilder(71, "test_encoder")
        .asEncoder(22, 23)
        .withEncoderButton(21)
        .withMidiCC(1)
        .withMidiNote(36)
        .build();
    
    TEST_ASSERT_TRUE(control.hardware.encoderButtonPin.has_value());
    TEST_ASSERT_EQUAL(21, control.hardware.encoderButtonPin->pin);
    TEST_ASSERT_EQUAL(1071, control.getEncoderButtonId());
    TEST_ASSERT_EQUAL(2, control.mappings.size());
}

void test_control_builder_button() {
    auto control = ControlBuilder(51, "menu_button")
        .withLabel("Menu")
        .asButton(32, ButtonMode::TOGGLE)
        .withLongPress(1000)
        .withNavigation("HOME", MappingControlType::BUTTON)
        .build();
    
    TEST_ASSERT_EQUAL(InputType::BUTTON, control.hardware.type);
    auto& btn = std::get<ControlDefinition::ButtonConfig>(control.hardware.config);
    TEST_ASSERT_EQUAL(32, btn.pin.pin);
    TEST_ASSERT_EQUAL(ButtonMode::TOGGLE, btn.mode);
    TEST_ASSERT_TRUE(btn.longPressMs.has_value());
    TEST_ASSERT_EQUAL(1000, *btn.longPressMs);
}

// === TESTS UNIFIED CONFIGURATION ===

void test_unified_configuration_add_control() {
    UnifiedConfiguration config;
    
    auto control1 = ControlBuilder(71, "encoder_1").asEncoder(22, 23).build();
    config.addControl(control1);
    
    TEST_ASSERT_EQUAL(1, config.getStats().totalControls);
    TEST_ASSERT_EQUAL(1, config.getStats().encoders);
    
    auto found = config.findControlById(71);
    TEST_ASSERT_TRUE(found.has_value());
    TEST_ASSERT_EQUAL_STRING("encoder_1", found->name.c_str());
}

void test_unified_configuration_duplicate_id_throws() {
    UnifiedConfiguration config;
    
    auto control1 = ControlBuilder(71, "encoder_1").asEncoder(22, 23).build();
    config.addControl(control1);
    
    auto control2 = ControlBuilder(71, "encoder_2").asEncoder(19, 20).build();
    
    // Cette ligne devrait lancer une exception
    bool exception_thrown = false;
    try {
        config.addControl(control2);
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
    }
    
    TEST_ASSERT_TRUE(exception_thrown);
}

void test_unified_configuration_encoder_button_id() {
    UnifiedConfiguration config;
    
    auto control = ControlBuilder(71, "encoder_1")
        .asEncoder(22, 23)
        .withEncoderButton(21)
        .build();
    
    config.addControl(control);
    
    // Devrait pouvoir trouver l'encodeur par son ID
    auto found = config.findControlById(71);
    TEST_ASSERT_TRUE(found.has_value());
    
    // Devrait aussi pouvoir trouver par l'ID du bouton (convention 1000+)
    auto foundButton = config.findControlById(1071);
    TEST_ASSERT_TRUE(foundButton.has_value());
    TEST_ASSERT_EQUAL(71, foundButton->id);  // Retourne le contrôle parent
}

void test_unified_configuration_get_by_role() {
    UnifiedConfiguration config;
    
    // Ajouter des contrôles MIDI
    config.addControl(
        ControlBuilder(71, "encoder_1")
            .asEncoder(22, 23)
            .withMidiCC(1)
            .build()
    );
    
    config.addControl(
        ControlBuilder(72, "encoder_2")
            .asEncoder(19, 20)
            .withMidiCC(2)
            .build()
    );
    
    // Ajouter un contrôle de navigation
    config.addControl(
        ControlBuilder(79, "nav_encoder")
            .asEncoder(9, 10)
            .withNavigation("ITEM_NAVIGATOR")
            .build()
    );
    
    auto midiControls = config.getControlsByRole(MappingRole::MIDI);
    TEST_ASSERT_EQUAL(2, midiControls.size());
    
    auto navControls = config.getControlsByRole(MappingRole::NAVIGATION);
    TEST_ASSERT_EQUAL(1, navControls.size());
    TEST_ASSERT_EQUAL(79, navControls[0].id);
}

// === TESTS GÉNÉRATION HARDWARE CONFIG ===

void test_generate_hardware_config_encoders() {
    UnifiedConfiguration config;
    
    config.addControl(
        ControlBuilder(71, "encoder_1")
            .withLabel("Enc 1")
            .inGroup("MIDI")
            .asEncoder(22, 23, 24)
            .withEncoderButton(21)
            .build()
    );
    
    auto hwConfig = config.generateHardwareConfig();
    
    TEST_ASSERT_EQUAL(1, hwConfig->encoders.size());
    TEST_ASSERT_EQUAL(1, hwConfig->buttons.size());  // Le bouton de l'encodeur
    
    const auto& encoder = hwConfig->encoders[0];
    TEST_ASSERT_EQUAL(71, encoder.id);
    TEST_ASSERT_EQUAL_STRING("encoder_1", encoder.name.c_str());
    TEST_ASSERT_EQUAL(22, encoder.pinA.pin);
    TEST_ASSERT_EQUAL(23, encoder.pinB.pin);
    TEST_ASSERT_EQUAL(24, encoder.ppr);
    
    const auto& button = hwConfig->buttons[0];
    TEST_ASSERT_EQUAL(1071, button.id);  // Convention 1000+
    TEST_ASSERT_EQUAL(21, button.pin.pin);
}

// === TESTS GÉNÉRATION MAPPING CONFIG ===

void test_generate_mapping_config_midi() {
    UnifiedConfiguration config;
    
    config.addControl(
        ControlBuilder(71, "encoder_1")
            .asEncoder(22, 23)
            .withEncoderButton(21)
            .withMidiCC(1, 0, true)
            .withMidiNote(36, 0)
            .build()
    );
    
    auto mapConfig = config.generateMappingConfig();
    
    // Devrait avoir 2 mappings : 1 pour l'encodeur, 1 pour le bouton
    TEST_ASSERT_EQUAL(2, mapConfig->controlsMapping.size());
    
    // Vérifier le mapping de l'encodeur
    const auto* encoderMapping = mapConfig->getMidiMappingForControl(71, MappingControlType::ENCODER);
    TEST_ASSERT_NOT_NULL(encoderMapping);
    TEST_ASSERT_EQUAL(0, encoderMapping->channel);
    TEST_ASSERT_EQUAL(1, encoderMapping->control);
    TEST_ASSERT_TRUE(encoderMapping->isRelative);
    
    // Vérifier le mapping du bouton
    const auto* buttonMapping = mapConfig->getMidiMappingForControl(1071, MappingControlType::BUTTON);
    TEST_ASSERT_NOT_NULL(buttonMapping);
    TEST_ASSERT_EQUAL(0, buttonMapping->channel);
    TEST_ASSERT_EQUAL(36, buttonMapping->control);
}

// === TESTS CONFIGURATION FACTORY ===

void test_configuration_factory_default() {
    auto config = ConfigurationFactory::createDefaultConfiguration();
    
    TEST_ASSERT_NOT_NULL(config);
    
    auto stats = config->getStats();
    
    // Vérifier le nombre total de contrôles
    TEST_ASSERT_EQUAL(12, stats.totalControls);  // 10 encodeurs + 2 boutons
    TEST_ASSERT_EQUAL(10, stats.encoders);       // 8 MIDI + 1 nav + 1 optique
    TEST_ASSERT_EQUAL(10, stats.buttons);        // 8 boutons d'encodeurs + 2 standalone
    
    // Vérifier qu'on peut générer les configurations
    auto hwConfig = config->generateHardwareConfig();
    auto mapConfig = config->generateMappingConfig();
    
    TEST_ASSERT_NOT_NULL(hwConfig);
    TEST_ASSERT_NOT_NULL(mapConfig);
    
    // Vérifier des contrôles spécifiques
    auto encoder1 = config->findControlById(71);
    TEST_ASSERT_TRUE(encoder1.has_value());
    TEST_ASSERT_EQUAL_STRING("Enc 1", encoder1->label.c_str());
    
    auto navEncoder = config->findControlById(79);
    TEST_ASSERT_TRUE(navEncoder.has_value());
    TEST_ASSERT_EQUAL(96, std::get<ControlDefinition::EncoderConfig>(navEncoder->hardware.config).ppr);
}

void test_unified_validation() {
    UnifiedConfiguration config;
    
    // Configuration valide
    config.addControl(
        ControlBuilder(71, "encoder_1")
            .asEncoder(22, 23)
            .build()
    );
    
    TEST_ASSERT_TRUE(config.validate());
    
    // Configuration invalide avec ID 0
    UnifiedConfiguration invalidConfig;
    bool exception_thrown = false;
    try {
        invalidConfig.addControl(
            ControlBuilder(0, "invalid")
                .asEncoder(22, 23)
                .build()
        );
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
    }
    
    TEST_ASSERT_TRUE(exception_thrown);
}

// === POINT D'ENTRÉE DES TESTS ===

void setup() {
    UNITY_BEGIN();
    
    // Tests ControlBuilder
    RUN_TEST(test_control_builder_basic_encoder);
    RUN_TEST(test_control_builder_encoder_with_button);
    RUN_TEST(test_control_builder_button);
    
    // Tests UnifiedConfiguration
    RUN_TEST(test_unified_configuration_add_control);
    RUN_TEST(test_unified_configuration_duplicate_id_throws);
    RUN_TEST(test_unified_configuration_encoder_button_id);
    RUN_TEST(test_unified_configuration_get_by_role);
    
    // Tests génération
    RUN_TEST(test_generate_hardware_config_encoders);
    RUN_TEST(test_generate_mapping_config_midi);
    
    // Tests Factory
    RUN_TEST(test_configuration_factory_default);
    RUN_TEST(test_unified_validation);
    
    UNITY_END();
}

void loop() {
    // Rien à faire
}
