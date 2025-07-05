#include <unity.h>

#include "adapters/primary/ui/parameter/ParameterWidgetMappingManager.hpp"

class TestParameterWidgetMappingManager {
public:
    static void test_constructor_default_config() {
        ParameterWidgetMappingManager manager;

        TEST_ASSERT_FALSE(manager.isInitialized());
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForCC(1));
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForButton(81));
    }

    static void test_constructor_custom_config() {
        ParameterWidgetMappingManager::MappingConfig config;
        config.maxWidgets = 4;
        config.enableLogging = true;
        config.enableButtonMapping = false;

        ParameterWidgetMappingManager manager(config);

        TEST_ASSERT_FALSE(manager.isInitialized());
    }

    static void test_initialize_empty_controls() {
        ParameterWidgetMappingManager manager;

        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;

        manager.initializeMappings(midiControls, buttonInfos);
        
        TEST_ASSERT_TRUE(manager.isInitialized());
        
        auto stats = manager.getStats();
        TEST_ASSERT_EQUAL(0, stats.ccMapped);
        TEST_ASSERT_EQUAL(0, stats.buttonsMapped);
        TEST_ASSERT_EQUAL(0, stats.buttonsStandalone);
    }

    static void test_initialize_midi_controls() {
        ParameterWidgetMappingManager manager;

        // Créer des contrôles MIDI test
        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        for (int i = 0; i < 3; i++) {
            ConfigurationMidiExtractor::MidiControlInfo info;
            info.cc_number = i + 1;  // CC1, CC2, CC3
            info.channel = 0;
            info.name = "ENC" + String(i + 1);
            info.control_id = 71 + i;
            midiControls.push_back(info);
        }

        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;

        manager.initializeMappings(midiControls, buttonInfos);
        
        TEST_ASSERT_TRUE(manager.isInitialized());
        
        // Vérifier les mappings CC
        TEST_ASSERT_EQUAL(0, manager.getWidgetIndexForCC(1));  // CC1 → widget 0
        TEST_ASSERT_EQUAL(1, manager.getWidgetIndexForCC(2));  // CC2 → widget 1
        TEST_ASSERT_EQUAL(2, manager.getWidgetIndexForCC(3));  // CC3 → widget 2
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForCC(4)); // CC4 non mappé
        
        auto stats = manager.getStats();
        TEST_ASSERT_EQUAL(3, stats.ccMapped);
        TEST_ASSERT_EQUAL(0, stats.buttonsMapped);
    }

    static void test_initialize_button_mapping() {
        ParameterWidgetMappingManager manager;

        // Créer des contrôles MIDI test
        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        ConfigurationMidiExtractor::MidiControlInfo midiInfo;
        midiInfo.cc_number = 1;
        midiInfo.channel = 0;
        midiInfo.name = "ENC1";
        midiInfo.control_id = 71;  // ID de l'encodeur
        midiControls.push_back(midiInfo);
        
        // Créer des boutons test
        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;

        // Bouton enfant d'encodeur
        ConfigurationMidiExtractor::ButtonInfo buttonInfo1;
        buttonInfo1.button_id = 1071;  // Bouton de l'encodeur 71
        buttonInfo1.parent_encoder_id = 71;
        buttonInfo1.name = "ENC1 BTN";
        buttonInfos.push_back(buttonInfo1);
        
        // Bouton standalone
        ConfigurationMidiExtractor::ButtonInfo buttonInfo2;
        buttonInfo2.button_id = 81;
        buttonInfo2.parent_encoder_id = 0;  // Pas de parent
        buttonInfo2.name = "BTN1";
        buttonInfos.push_back(buttonInfo2);
        
        manager.initializeMappings(midiControls, buttonInfos);
        
        TEST_ASSERT_TRUE(manager.isInitialized());
        
        // Vérifier le mapping du bouton enfant
        TEST_ASSERT_EQUAL(0, manager.getWidgetIndexForButton(1071));  // Bouton → widget 0
        
        // Vérifier le bouton standalone
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForButton(81));   // Standalone, pas mappé
        
        auto stats = manager.getStats();
        TEST_ASSERT_EQUAL(1, stats.ccMapped);
        TEST_ASSERT_EQUAL(1, stats.buttonsMapped);
        TEST_ASSERT_EQUAL(1, stats.buttonsStandalone);
        
        // Vérifier les standalone buttons
        auto standaloneButtons = manager.getStandaloneButtons();
        TEST_ASSERT_EQUAL(1, standaloneButtons.size());
        TEST_ASSERT_EQUAL(81, standaloneButtons[0].button_id);
    }

    static void test_max_widgets_limit() {
        ParameterWidgetMappingManager::MappingConfig config;
        config.maxWidgets = 2;  // Limiter à 2 widgets
        ParameterWidgetMappingManager manager(config);

        // Créer 4 contrôles MIDI
        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        for (int i = 0; i < 4; i++) {
            ConfigurationMidiExtractor::MidiControlInfo info;
            info.cc_number = i + 1;
            info.channel = 0;
            info.name = "ENC" + String(i + 1);
            info.control_id = 71 + i;
            midiControls.push_back(info);
        }

        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;

        manager.initializeMappings(midiControls, buttonInfos);
        
        // Seuls les 2 premiers doivent être mappés
        TEST_ASSERT_EQUAL(0, manager.getWidgetIndexForCC(1));
        TEST_ASSERT_EQUAL(1, manager.getWidgetIndexForCC(2));
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForCC(3));  // Excédentaire
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForCC(4));  // Excédentaire
        
        auto stats = manager.getStats();
        TEST_ASSERT_EQUAL(2, stats.ccMapped);  // Seulement 2 mappés
    }

    static void test_reset_functionality() {
        ParameterWidgetMappingManager manager;

        // Initialiser avec des données
        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        ConfigurationMidiExtractor::MidiControlInfo info;
        info.cc_number = 1;
        info.channel = 0;
        info.name = "ENC1";
        info.control_id = 71;
        midiControls.push_back(info);

        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;

        manager.initializeMappings(midiControls, buttonInfos);
        TEST_ASSERT_TRUE(manager.isInitialized());
        TEST_ASSERT_EQUAL(0, manager.getWidgetIndexForCC(1));
        
        // Reset
        manager.reset();
        
        TEST_ASSERT_FALSE(manager.isInitialized());
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForCC(1));
        
        auto stats = manager.getStats();
        TEST_ASSERT_EQUAL(0, stats.ccMapped);
    }

    static void test_button_mapping_disabled() {
        ParameterWidgetMappingManager::MappingConfig config;
        config.enableButtonMapping = false;
        ParameterWidgetMappingManager manager(config);

        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        ConfigurationMidiExtractor::MidiControlInfo midiInfo;
        midiInfo.cc_number = 1;
        midiInfo.control_id = 71;
        midiControls.push_back(midiInfo);

        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;
        ConfigurationMidiExtractor::ButtonInfo buttonInfo;
        buttonInfo.button_id = 1071;
        buttonInfo.parent_encoder_id = 71;
        buttonInfos.push_back(buttonInfo);
        
        manager.initializeMappings(midiControls, buttonInfos);
        
        // Boutons ne doivent pas être mappés
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForButton(1071));
        
        auto stats = manager.getStats();
        TEST_ASSERT_EQUAL(1, stats.ccMapped);
        TEST_ASSERT_EQUAL(0, stats.buttonsMapped);    // Désactivé
        TEST_ASSERT_EQUAL(0, stats.buttonsStandalone); // Désactivé
    }

    static void test_invalid_cc_numbers() {
        ParameterWidgetMappingManager manager;

        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        ConfigurationMidiExtractor::MidiControlInfo info1;
        info1.cc_number = 150;  // Invalid (> 127)
        info1.control_id = 71;
        midiControls.push_back(info1);

        ConfigurationMidiExtractor::MidiControlInfo info2;
        info2.cc_number = 64;   // Valid
        info2.control_id = 72;
        midiControls.push_back(info2);

        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;

        manager.initializeMappings(midiControls, buttonInfos);
        
        // Seul le CC valide doit être mappé
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForCC(150)); // Out of range
        TEST_ASSERT_EQUAL(0, manager.getWidgetIndexForCC(64));   // Valid
        
        auto stats = manager.getStats();
        TEST_ASSERT_EQUAL(1, stats.ccMapped);  // Seul le valide
    }

    static void test_button_without_parent_encoder() {
        ParameterWidgetMappingManager manager;

        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        // Pas d'encodeur avec ID 99

        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;
        ConfigurationMidiExtractor::ButtonInfo buttonInfo;
        buttonInfo.button_id = 1099;
        buttonInfo.parent_encoder_id = 99;  // Parent inexistant
        buttonInfo.name = "Orphan Button";
        buttonInfos.push_back(buttonInfo);
        
        manager.initializeMappings(midiControls, buttonInfos);
        
        // Bouton ne doit pas être mappé (parent introuvable)
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForButton(1099));
        
        auto stats = manager.getStats();
        TEST_ASSERT_EQUAL(0, stats.buttonsMapped);
    }

    static void test_boundary_conditions() {
        ParameterWidgetMappingManager manager;

        // Test CC boundary
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForCC(128));  // Out of range
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForCC(255));  // Out of range
        
        // Test button ID 0
        TEST_ASSERT_EQUAL(-1, manager.getWidgetIndexForButton(0));
    }
};

void test_widget_mapping_manager_constructor_default() {
    TestParameterWidgetMappingManager::test_constructor_default_config();
}

void test_widget_mapping_manager_constructor_custom() {
    TestParameterWidgetMappingManager::test_constructor_custom_config();
}

void test_widget_mapping_manager_initialize_empty() {
    TestParameterWidgetMappingManager::test_initialize_empty_controls();
}

void test_widget_mapping_manager_initialize_midi() {
    TestParameterWidgetMappingManager::test_initialize_midi_controls();
}

void test_widget_mapping_manager_initialize_buttons() {
    TestParameterWidgetMappingManager::test_initialize_button_mapping();
}

void test_widget_mapping_manager_max_widgets() {
    TestParameterWidgetMappingManager::test_max_widgets_limit();
}

void test_widget_mapping_manager_reset() {
    TestParameterWidgetMappingManager::test_reset_functionality();
}

void test_widget_mapping_manager_buttons_disabled() {
    TestParameterWidgetMappingManager::test_button_mapping_disabled();
}

void test_widget_mapping_manager_invalid_cc() {
    TestParameterWidgetMappingManager::test_invalid_cc_numbers();
}

void test_widget_mapping_manager_orphan_button() {
    TestParameterWidgetMappingManager::test_button_without_parent_encoder();
}

void test_widget_mapping_manager_boundaries() {
    TestParameterWidgetMappingManager::test_boundary_conditions();
}