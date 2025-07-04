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

// Tests ConfigurationService
extern void test_config_service_get_all_control_definitions_minimal();
extern void test_config_service_get_by_type_encoders();
extern void test_config_service_get_by_type_buttons();
extern void test_config_service_get_by_id_existing();
extern void test_config_service_get_by_id_non_existing();
extern void test_config_service_get_by_group();
extern void test_config_service_get_available_groups();
extern void test_config_service_get_input_count_by_type();
extern void test_config_service_validate_all_configurations_valid();
extern void test_config_service_validate_all_configurations_empty();
extern void test_config_service_set_application_configuration();

// Tests ConfigurationRegistry
extern void test_config_registry_register_subsystem_valid();
extern void test_config_registry_register_subsystem_null();
extern void test_config_registry_register_unified_config_valid();
extern void test_config_registry_register_unified_config_null();
extern void test_config_registry_with_null_container();
extern void test_config_registry_multiple_registrations();

// Tests DisplayManager
extern void test_display_manager_constructor_valid_bridge();
extern void test_display_manager_constructor_null_bridge();
extern void test_display_manager_update_first_call();
extern void test_display_manager_update_fast_calls();
extern void test_display_manager_force_refresh();
extern void test_display_manager_set_refresh_interval();
extern void test_display_manager_get_refresh_interval_default();
extern void test_display_manager_update_with_null_bridge();

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
    
    // Tests ConfigurationService
    RUN_TEST(test_config_service_get_all_control_definitions_minimal);
    RUN_TEST(test_config_service_get_by_type_encoders);
    RUN_TEST(test_config_service_get_by_type_buttons);
    RUN_TEST(test_config_service_get_by_id_existing);
    RUN_TEST(test_config_service_get_by_id_non_existing);
    RUN_TEST(test_config_service_get_by_group);
    RUN_TEST(test_config_service_get_available_groups);
    RUN_TEST(test_config_service_get_input_count_by_type);
    RUN_TEST(test_config_service_validate_all_configurations_valid);
    RUN_TEST(test_config_service_validate_all_configurations_empty);
    RUN_TEST(test_config_service_set_application_configuration);
    
    // Tests ConfigurationRegistry
    RUN_TEST(test_config_registry_register_subsystem_valid);
    RUN_TEST(test_config_registry_register_subsystem_null);
    RUN_TEST(test_config_registry_register_unified_config_valid);
    RUN_TEST(test_config_registry_register_unified_config_null);
    RUN_TEST(test_config_registry_with_null_container);
    RUN_TEST(test_config_registry_multiple_registrations);
    
    // Tests DisplayManager
    RUN_TEST(test_display_manager_constructor_valid_bridge);
    RUN_TEST(test_display_manager_constructor_null_bridge);
    RUN_TEST(test_display_manager_update_first_call);
    RUN_TEST(test_display_manager_update_fast_calls);
    RUN_TEST(test_display_manager_force_refresh);
    RUN_TEST(test_display_manager_set_refresh_interval);
    RUN_TEST(test_display_manager_get_refresh_interval_default);
    RUN_TEST(test_display_manager_update_with_null_bridge);
    
    UNITY_END();
    
    // Boucle infinie pour les tests embarqués
    while(1) {
        delay(1000);
    }
    
    return 0;
}