#pragma once

#include "TestCommandRegistry.hpp"
#include "../../adapters/secondary/hardware/display/Ili9341Driver.hpp"
#include "../../adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "../../adapters/primary/ui/DisplayTestSuite.hpp"

/**
 * @brief Classe pour enregistrer toutes les commandes de test du display
 */
class DisplayTestCommands {
public:
    /**
     * @brief Enregistre toutes les commandes de test dans le registry
     */
    static void registerCommands(TestCommandRegistry& registry,
                                std::shared_ptr<Ili9341Driver> driver,
                                std::shared_ptr<Ili9341LvglBridge> bridge,
                                std::shared_ptr<DisplayTestSuite> testSuite) {
        
        // === TESTS COMPLETS ===
        registry.registerCommand('T', "Run full test suite", [testSuite]() {
            testSuite->runFullHardwareTestSuite();
        });
        
        // === TESTS HARDWARE ===
        registry.registerCommand('I', "Test multiple init", [testSuite]() {
            // TODO: Implémenter dans DisplayTestSuite
            Serial.println(F("Multiple init test not yet implemented in new architecture"));
        });
        
        registry.registerCommand('R', "Test all rotations", [testSuite]() {
            if (testSuite->testAllRotations()) {
                Serial.println(F("All rotations: PASSED"));
            } else {
                Serial.println(F("All rotations: FAILED"));
            }
        });
        
        registry.registerCommand('E', "Run endurance test (1000 cycles)", [testSuite]() {
            if (testSuite->testEndurance(1000)) {
                Serial.println(F("Endurance: PASSED"));
            } else {
                Serial.println(F("Endurance: FAILED"));
            }
        });
        
        registry.registerCommand('F', "Direct framebuffer test (no LVGL)", [testSuite]() {
            if (testSuite->testDirectFramebuffer()) {
                Serial.println(F("Direct framebuffer test: PASSED"));
            } else {
                Serial.println(F("Direct framebuffer test: FAILED"));
            }
        });
        
        // === TESTS PERFORMANCE ===
        registry.registerCommand('P', "Run performance benchmark", [testSuite]() {
            testSuite->runPerformanceBenchmark();
        });
        
        registry.registerCommand('S', "Show flush profiler stats", []() {
            // TODO: Implémenter profiler dans nouvelle architecture
            Serial.println(F("Profiler not yet implemented in new architecture"));
        });
        
        // === TESTS MÉMOIRE ===
        registry.registerCommand('M', "Show memory diagnostics", [driver]() {
            driver->debugMemory();
        });
        
        // === TESTS LVGL ===
        registry.registerCommand('C', "Clear screen test", [testSuite]() {
            testSuite->clearScreen();
        });
        
        registry.registerCommand('L', "Simple LVGL test", [testSuite]() {
            testSuite->testSimpleLvgl();
        });
        
        registry.registerCommand('D', "Demo screen", [testSuite]() {
            testSuite->demoScreen();
        });
        
        // === TESTS WIDGETS ===
        registry.registerCommand('W', "ParameterWidget visual demo", [testSuite]() {
            if (testSuite->demoParameterWidget()) {
                Serial.println(F("ParameterWidget demo: COMPLETED"));
            } else {
                Serial.println(F("ParameterWidget demo: FAILED"));
            }
        });
        
        // === COMMANDES SYSTÈME ===
        registry.registerCommand('H', "Show this help", [&registry]() {
            registry.printHelp();
        });
        
        registry.registerCommand('?', "Show this help", [&registry]() {
            registry.printHelp();
        });
    }
};