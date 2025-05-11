#include <unity.h>

// Inclure tous les fichiers de test
#include "di_tests.h"
#include "integration/MidiControllerAppIntegrationTests.h"
#include "mock_tests.h"
#include "service_locator_adapter_tests.h"
#include "unit/ConfigurationSubsystemTests.h"
#include "unit/InputSubsystemTests.h"
#include "unit/MidiControllerAppTests.h"
#include "unit/MidiSubsystemTests.h"
#include "unit/UISubsystemTests.h"

// Fonctions de configuration communes
void setUp(void) {
    // Setup commun pour tous les tests
}

void tearDown(void) {
    // Nettoyage commun pour tous les tests
}

// Point d'entrée principal pour tous les tests
int main(void) {
    UNITY_BEGIN();

    // Tests pour le DependencyContainer
    RUN_TEST(test_dependency_container_basic);
    RUN_TEST(test_dependency_container_interface);

    // Tests pour les mocks
    RUN_TEST(test_mock_initialization);
    RUN_TEST(test_mock_updates);
    RUN_TEST(test_midi_messages);
    RUN_TEST(test_ui_messages);

    // Tests pour le ServiceLocatorAdapter
    RUN_TEST(test_service_locator_adapter_initialize);
    RUN_TEST(test_service_locator_adapter_container);
    RUN_TEST(test_service_locator_adapter_simple_service);

    // Tests pour le ConfigurationSubsystem
    RUN_TEST(test_configuration_subsystem_initialization);
    RUN_TEST(test_configuration_navigation_controls);
    RUN_TEST(test_configuration_accessors);

    // Tests pour InputSubsystem
    RUN_TEST(test_input_subsystem_initialization);
    RUN_TEST(test_input_subsystem_configure);
    RUN_TEST(test_input_subsystem_update);

    // Tests pour MidiSubsystem
    RUN_TEST(test_midi_subsystem_initialization);
    RUN_TEST(test_midi_subsystem_send_messages);
    RUN_TEST(test_midi_subsystem_default_channel);

    // Tests pour UISubsystem
    RUN_TEST(test_ui_subsystem_initialization);
    RUN_TEST(test_ui_subsystem_display_methods);
    RUN_TEST(test_ui_subsystem_full_ui);

    // Tests pour MidiControllerApp
    RUN_TEST(test_midi_controller_app2_initialization);
    RUN_TEST(test_midi_controller_app2_update);
    RUN_TEST(test_midi_controller_app2_navigation_api);

    // Tests d'intégration pour MidiControllerApp
    RUN_TEST(test_midi_controller_app2_full_integration);
    RUN_TEST(test_midi_controller_app2_subsystem_interactions);

    return UNITY_END();
}
