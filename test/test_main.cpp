#include <Arduino.h>
#include <unity.h>

// Déclarations des tests
extern void test_init_with_missing_application_configuration();
extern void test_init_with_existing_application_configuration();
extern void test_control_access_after_init();
extern void test_default_configuration_properties();
extern void test_navigation_functions();

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
    
    UNITY_END();
    
    // Boucle infinie pour les tests embarqués
    while(1) {
        delay(1000);
    }
    
    return 0;
}