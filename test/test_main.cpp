#include <unity.h>

// Inclure tous les fichiers de test
#include "di_tests.h"
#include "mock_tests.h"
#include "service_locator_adapter_tests.h"

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
    
    return UNITY_END();
}
