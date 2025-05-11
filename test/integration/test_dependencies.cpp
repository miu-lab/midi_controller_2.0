#include <unity.h>
#include "../mocks/MockInput.h"
#include "../mocks/MockMidi.h"
#include "../mocks/MockUI.h"

// Simuler les dépendances pour tester l'intégration de base
// Note: Ces tests seront étendus plus tard quand nous aurons le système d'injection de dépendances

void setUp(void) {
    // Setup for test
}

void tearDown(void) {
    // Clean up after test
}

// Test de base pour valider que les interfaces fonctionnent ensemble
void test_systems_integration_basic(void) {
    // Créer des instances des mocks
    MockInput input;
    MockMidi midi;
    MockUI ui;
    
    // Initialiser les systèmes
    input.init();
    midi.init();
    ui.init(true);
    
    // Simuler quelques interactions
    input.update(); // Supposons que cela déclenche une action MIDI
    midi.sendControlChange(0, 7, 127); // Supposons que cela change l'UI
    ui.showMessage("CC: 7=127"); // L'UI affiche le changement
    
    // Vérifier que tout s'est bien passé
    TEST_ASSERT_EQUAL_INT(1, input.updateCount);
    TEST_ASSERT_EQUAL_INT(1, midi.ccMessages.size());
    TEST_ASSERT_EQUAL_INT(1, ui.messages.size());
    TEST_ASSERT_EQUAL_STRING("CC: 7=127", ui.messages[0].c_str());
}

// Point d'entrée pour l'exécution des tests
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_systems_integration_basic);
    
    return UNITY_END();
}
