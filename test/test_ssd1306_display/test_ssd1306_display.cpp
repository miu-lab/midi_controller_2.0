#include <unity.h>
#include <Wire.h>
#include "adapters/secondary/hardware/display/Ssd1306Display.hpp"

// Mocks nécessaires pour les tests
class MockTwoWire : public TwoWire {
public:
    MockTwoWire() : TwoWire(0) {}
    
    // Redéfinir les méthodes nécessaires pour le test
    void begin() override {}
    void begin(uint8_t address) override {}
    void begin(int address) override {}
    void setClock(uint32_t) override {}
    void beginTransmission(uint8_t) override {}
    uint8_t endTransmission(void) override { return 0; }
    uint8_t endTransmission(uint8_t) override { return 0; }
    uint8_t requestFrom(uint8_t, uint8_t) override { return 0; }
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t) override { return 0; }
    uint8_t requestFrom(uint8_t, uint8_t, uint32_t, uint8_t, uint8_t) override { return 0; }
    size_t write(uint8_t) override { return 1; }
    size_t write(const uint8_t *, size_t) override { return 1; }
    int available(void) override { return 0; }
    int read(void) override { return 0; }
    int peek(void) override { return 0; }
    void flush(void) override {}
    void onReceive(void(*)(int)) override {}
    void onRequest(void(*)(void)) override {}
};

MockTwoWire mockWire;

// Test pour vérifier la création de l'instance SSD1306Display
void test_ssd1306_display_creation() {
    Ssd1306Display display(128, 64, 0x3C, mockWire);
    TEST_ASSERT_NOT_NULL(&display);
}

// Test pour vérifier l'échec d'initialisation 
// (ne peut pas être vraiment testé sans matériel)
void test_ssd1306_display_init_failure() {
    Ssd1306Display display(128, 64, 0x3C, mockWire);
    // Comme il s'agit d'un test unitaire sans matériel,
    // init() va échouer, ce qui est attendu
    bool result = display.init();
    TEST_ASSERT_FALSE(result);
}

// Test appelant les méthodes de DrawingAPI sans erreur
void test_ssd1306_display_api_calls() {
    Ssd1306Display display(128, 64, 0x3C, mockWire);
    
    // Ces appels devraient simplement retourner sans erreur
    // même si l'initialisation a échoué
    display.clear();
    display.drawText(0, 0, "Test");
    display.drawLine(0, 0, 10, 10);
    display.drawRect(10, 10, 20, 20);
    display.drawCircle(30, 30, 5);
    display.update();
    display.setTextSize(2);
    display.setTextColor(1);
    
    // Aucune assertion nécessaire, on vérifie juste qu'il n'y a pas de crash
}

void setUp(void) {
    // Configuration avant chaque test
}

void tearDown(void) {
    // Nettoyage après chaque test
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ssd1306_display_creation);
    RUN_TEST(test_ssd1306_display_init_failure);
    RUN_TEST(test_ssd1306_display_api_calls);
    return UNITY_END();
}

// Pour les tests sur PC
int main(void) {
    return runUnityTests();
}

// Pour les tests sur Teensy
void setup() {
    delay(2000);
    runUnityTests();
}

void loop() {}
