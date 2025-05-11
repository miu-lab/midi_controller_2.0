#include "../TestFramework.hpp"
#include "../mocks/MockInput.hpp"
#include "../mocks/MockMidi.hpp"
#include "../mocks/MockUI.hpp"

// Un test simple pour valider le framework de test
bool testMockInitialization() {
    MockInput input;
    MockMidi midi;
    MockUI ui;
    
    // Initialiser les mocks
    input.init();
    midi.init();
    ui.init(true);
    
    // Vérifier que l'initialisation a fonctionné
    TEST_ASSERT(input.initialized);
    TEST_ASSERT(midi.initialized);
    TEST_ASSERT(ui.initialized);
    TEST_ASSERT(ui.fullUIEnabled);
    
    return true;
}

// Point d'entrée pour exécuter ce test spécifique
int main() {
    TestSuite suite;
    REGISTER_TEST(suite, "Mock Initialization", testMockInitialization);
    
    return suite.runAll() ? 0 : 1;
}
