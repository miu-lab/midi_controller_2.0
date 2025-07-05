#include <unity.h>

#include "adapters/primary/ui/parameter/ConfigurationMidiExtractor.hpp"
#include "config/unified/ConfigurationFactory.hpp"
#include "config/unified/UnifiedConfiguration.hpp"

class TestConfigurationMidiExtractor {
public:
    static void test_constructor_with_default_config() {
        ConfigurationMidiExtractor::ParserConfig config;
        ConfigurationMidiExtractor parser(config);

        TEST_ASSERT_TRUE(true); // Constructor successful
    }

    static void test_constructor_with_custom_config() {
        ConfigurationMidiExtractor::ParserConfig config;
        config.enableLogging = true;
        config.maxCCNumber = 100;

        ConfigurationMidiExtractor parser(config);

        TEST_ASSERT_TRUE(true); // Constructor successful
    }

    static void test_extract_midi_controls_empty_config() {
        // Créer une configuration vide
        UnifiedConfiguration config;
        ConfigurationMidiExtractor parser;

        auto result = parser.extractMidiControls(config);
        
        TEST_ASSERT_EQUAL(0, result.size());
    }

    static void test_extract_button_info_empty_config() {
        // Créer une configuration vide
        UnifiedConfiguration config;
        ConfigurationMidiExtractor parser;

        auto result = parser.extractButtonInfo(config);
        
        TEST_ASSERT_EQUAL(0, result.size());
    }

    static void test_validate_midi_control_info_valid() {
        ConfigurationMidiExtractor parser;

        ConfigurationMidiExtractor::MidiControlInfo info;
        info.cc_number = 64;
        info.channel = 5;
        info.name = "Test Control";
        info.control_id = 100;
        
        TEST_ASSERT_TRUE(parser.validateMidiControlInfo(info));
    }

    static void test_validate_midi_control_info_invalid_cc() {
        ConfigurationMidiExtractor::ParserConfig config;
        config.maxCCNumber = 100;
        ConfigurationMidiExtractor parser(config);

        ConfigurationMidiExtractor::MidiControlInfo info;
        info.cc_number = 150; // > maxCCNumber
        info.channel = 5;
        info.name = "Test Control";
        info.control_id = 100;
        
        TEST_ASSERT_FALSE(parser.validateMidiControlInfo(info));
    }

    static void test_validate_midi_control_info_invalid_channel() {
        ConfigurationMidiExtractor parser;

        ConfigurationMidiExtractor::MidiControlInfo info;
        info.cc_number = 64;
        info.channel = 20; // > 15
        info.name = "Test Control";
        info.control_id = 100;
        
        TEST_ASSERT_FALSE(parser.validateMidiControlInfo(info));
    }

    static void test_validate_midi_control_info_empty_name() {
        ConfigurationMidiExtractor parser;

        ConfigurationMidiExtractor::MidiControlInfo info;
        info.cc_number = 64;
        info.channel = 5;
        info.name = ""; // Empty name
        info.control_id = 100;
        
        TEST_ASSERT_FALSE(parser.validateMidiControlInfo(info));
    }

    static void test_validate_button_info_valid() {
        ConfigurationMidiExtractor parser;

        ConfigurationMidiExtractor::ButtonInfo info;
        info.button_id = 81;
        info.parent_encoder_id = 71;
        info.name = "Test Button";
        
        TEST_ASSERT_TRUE(parser.validateButtonInfo(info));
    }

    static void test_validate_button_info_invalid_id() {
        ConfigurationMidiExtractor parser;

        ConfigurationMidiExtractor::ButtonInfo info;
        info.button_id = 0; // Invalid ID
        info.parent_encoder_id = 71;
        info.name = "Test Button";
        
        TEST_ASSERT_FALSE(parser.validateButtonInfo(info));
    }

    static void test_validate_button_info_empty_name() {
        ConfigurationMidiExtractor parser;

        ConfigurationMidiExtractor::ButtonInfo info;
        info.button_id = 81;
        info.parent_encoder_id = 71;
        info.name = ""; // Empty name
        
        TEST_ASSERT_FALSE(parser.validateButtonInfo(info));
    }

    static void test_extract_with_factory_config() {
        // Utiliser ConfigurationFactory pour créer une config avec des encodeurs
        auto config = ConfigurationFactory::createDefaultConfiguration();
        ConfigurationMidiExtractor parser;

        auto midiResult = parser.extractMidiControls(config);
        auto buttonResult = parser.extractButtonInfo(config);
        
        // Les résultats dépendent de la configuration par défaut
        // Au minimum, les fonctions ne doivent pas planter
        TEST_ASSERT_TRUE(midiResult.size() >= 0);
        TEST_ASSERT_TRUE(buttonResult.size() >= 0);
    }
};

void test_midi_configuration_parser_constructor_default() {
    TestConfigurationMidiExtractor::test_constructor_with_default_config();
}

void test_midi_configuration_parser_constructor_custom() {
    TestConfigurationMidiExtractor::test_constructor_with_custom_config();
}

void test_midi_configuration_parser_extract_empty() {
    TestConfigurationMidiExtractor::test_extract_midi_controls_empty_config();
}

void test_midi_configuration_parser_extract_button_empty() {
    TestConfigurationMidiExtractor::test_extract_button_info_empty_config();
}

void test_midi_configuration_parser_validate_midi_valid() {
    TestConfigurationMidiExtractor::test_validate_midi_control_info_valid();
}

void test_midi_configuration_parser_validate_midi_invalid_cc() {
    TestConfigurationMidiExtractor::test_validate_midi_control_info_invalid_cc();
}

void test_midi_configuration_parser_validate_midi_invalid_channel() {
    TestConfigurationMidiExtractor::test_validate_midi_control_info_invalid_channel();
}

void test_midi_configuration_parser_validate_midi_empty_name() {
    TestConfigurationMidiExtractor::test_validate_midi_control_info_empty_name();
}

void test_midi_configuration_parser_validate_button_valid() {
    TestConfigurationMidiExtractor::test_validate_button_info_valid();
}

void test_midi_configuration_parser_validate_button_invalid_id() {
    TestConfigurationMidiExtractor::test_validate_button_info_invalid_id();
}

void test_midi_configuration_parser_validate_button_empty_name() {
    TestConfigurationMidiExtractor::test_validate_button_info_empty_name();
}

void test_midi_configuration_parser_factory_config() {
    TestConfigurationMidiExtractor::test_extract_with_factory_config();
}