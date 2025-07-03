#include <Arduino.h>
#include <unity.h>

// Déclarations des tests
extern void test_init_with_missing_application_configuration();
extern void test_init_with_existing_application_configuration();
extern void test_control_access_after_init();
extern void test_default_configuration_properties();
extern void test_navigation_functions();

// Tests InputSubsystem
extern void test_input_init_with_missing_configuration();
extern void test_input_init_with_empty_configuration();
extern void test_input_init_with_missing_input_controller();
extern void test_input_init_with_valid_dependencies();
extern void test_input_double_initialization();
extern void test_input_validation_after_init();
extern void test_input_init_with_complex_configuration();

// Tests UISubsystem
extern void test_ui_init_with_missing_configuration();
extern void test_ui_init_with_simple_ui();
extern void test_ui_init_with_full_ui_but_missing_dependencies();
extern void test_ui_init_with_full_ui_and_valid_dependencies();
extern void test_ui_double_initialization();
extern void test_ui_show_message_without_initialization();
extern void test_ui_clear_display_without_initialization();
extern void test_ui_update_with_simple_ui();
extern void test_ui_init_with_complex_configuration();

// Tests MidiSubsystem
extern void test_midi_init_with_missing_configuration();
extern void test_midi_init_with_valid_configuration();
extern void test_midi_init_with_existing_command_manager();
extern void test_midi_double_initialization();
extern void test_midi_send_note_on_without_initialization();
extern void test_midi_send_control_change_without_initialization();
extern void test_midi_update_with_initialized_system();
extern void test_midi_init_with_complex_configuration();
extern void test_midi_get_midi_mapper_after_init();

// Tests ConfigurationLoader
extern void test_config_loader_load_with_null_configuration();
extern void test_config_loader_load_with_valid_configuration();
extern void test_config_loader_validate_without_loading();
extern void test_config_loader_validate_after_valid_loading();
extern void test_config_loader_complete_load_and_validate_sequence();
extern void test_config_loader_multiple_load_operations();

// Fonctions setUp/tearDown par défaut pour Unity
// Note: les tests spécifiques peuvent redéfinir ces fonctions
void setUp(void) __attribute__((weak));
void tearDown(void) __attribute__((weak));

void setUp(void) {
    // Configuration par défaut avant chaque test
}

void tearDown(void) {
    // Nettoyage par défaut après chaque test
}

// Test dummy pour vérifier que le framework Unity fonctionne
void test_unity_framework() {
    TEST_ASSERT_EQUAL(1, 1);
}

// Test dummy pour vérifier la compilation de base
void test_basic_compilation() {
    int result = 2 + 2;
    TEST_ASSERT_EQUAL(4, result);
}

int main() {
    // Initialiser le port série pour les sorties de test
    Serial.begin(115200);
    
    // Pause pour laisser le temps au port série de s'initialiser
    delay(2000);
    
    UNITY_BEGIN();
    
    // Tests basiques pour valider le framework
    RUN_TEST(test_unity_framework);
    RUN_TEST(test_basic_compilation);
    
    // Tests ConfigurationSubsystem
    RUN_TEST(test_init_with_missing_application_configuration);
    RUN_TEST(test_init_with_existing_application_configuration);
    RUN_TEST(test_control_access_after_init);
    RUN_TEST(test_default_configuration_properties);
    RUN_TEST(test_navigation_functions);
    
    // Tests InputSubsystem
    RUN_TEST(test_input_init_with_missing_configuration);
    RUN_TEST(test_input_init_with_empty_configuration);
    RUN_TEST(test_input_init_with_missing_input_controller);
    RUN_TEST(test_input_init_with_valid_dependencies);
    RUN_TEST(test_input_double_initialization);
    RUN_TEST(test_input_validation_after_init);
    RUN_TEST(test_input_init_with_complex_configuration);
    
    // Tests UISubsystem
    RUN_TEST(test_ui_init_with_missing_configuration);
    RUN_TEST(test_ui_init_with_simple_ui);
    RUN_TEST(test_ui_init_with_full_ui_but_missing_dependencies);
    RUN_TEST(test_ui_init_with_full_ui_and_valid_dependencies);
    RUN_TEST(test_ui_double_initialization);
    RUN_TEST(test_ui_show_message_without_initialization);
    RUN_TEST(test_ui_clear_display_without_initialization);
    RUN_TEST(test_ui_update_with_simple_ui);
    RUN_TEST(test_ui_init_with_complex_configuration);
    
    // Tests MidiSubsystem
    RUN_TEST(test_midi_init_with_missing_configuration);
    RUN_TEST(test_midi_init_with_valid_configuration);
    RUN_TEST(test_midi_init_with_existing_command_manager);
    RUN_TEST(test_midi_double_initialization);
    RUN_TEST(test_midi_send_note_on_without_initialization);
    RUN_TEST(test_midi_send_control_change_without_initialization);
    RUN_TEST(test_midi_update_with_initialized_system);
    RUN_TEST(test_midi_init_with_complex_configuration);
    RUN_TEST(test_midi_get_midi_mapper_after_init);
    
    // Tests ConfigurationLoader
    RUN_TEST(test_config_loader_load_with_null_configuration);
    RUN_TEST(test_config_loader_load_with_valid_configuration);
    RUN_TEST(test_config_loader_validate_without_loading);
    RUN_TEST(test_config_loader_validate_after_valid_loading);
    RUN_TEST(test_config_loader_complete_load_and_validate_sequence);
    RUN_TEST(test_config_loader_multiple_load_operations);
    
    UNITY_END();
    
    // Boucle infinie pour les tests embarqués
    while(1) {
        delay(1000);
    }
    
    return 0;
}