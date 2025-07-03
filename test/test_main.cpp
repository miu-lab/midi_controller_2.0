#include <Arduino.h>
#include <unity.h>

// Fonctions setUp/tearDown requises par Unity
void setUp(void) {
    // Configuration avant chaque test
}

void tearDown(void) {
    // Nettoyage après chaque test
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
    
    UNITY_END();
    
    // Boucle infinie pour les tests embarqués
    while(1) {
        delay(1000);
    }
    
    return 0;
}