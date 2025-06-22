#pragma once

#include "TestCommandRegistry.hpp"
#include "../../adapters/secondary/hardware/display/Ili9341Driver.hpp"
#include "../../adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "../../adapters/primary/ui/DisplayTestSuite.hpp"
#include "../../adapters/primary/ui/LvglParameterView.hpp"
#include "../../config/unified/ConfigurationFactory.hpp"

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
        
        registry.registerCommand('V', "LvglParameterView test (contrôles MIDI)", [bridge]() {
            Serial.println(F("=== LVGL PARAMETER VIEW TEST ==="));
            
            // Créer configuration par défaut pour obtenir les contrôles MIDI
            auto unifiedConfig = ConfigurationFactory::createDefaultConfiguration();
            
            // Créer et tester la vue paramètres avec les contrôles MIDI réels
            LvglParameterView::Config config;
            config.enable_animations = true;
            
            auto parameterView = std::make_unique<LvglParameterView>(bridge, std::shared_ptr<UnifiedConfiguration>(unifiedConfig.release()), config);
            
            if (parameterView->init()) {
                Serial.println(F("LvglParameterView: Initialisée avec succès"));
                
                parameterView->activate();
                delay(2000);
                
                // Test mise à jour des valeurs pour tous les contrôles MIDI
                uint8_t midiCount = parameterView->getMidiControlCount();
                Serial.print(F("Testant "));
                Serial.print(midiCount);
                Serial.println(F(" contrôles MIDI"));
                
                for (uint8_t i = 0; i < midiCount; i++) {
                    parameterView->updateParameterValue(i, (i * 16) % 128);
                    delay(200);
                }
                
                // Test focus
                for (uint8_t i = 0; i < midiCount; i++) {
                    parameterView->focusParameter(i);
                    delay(500);
                }
                parameterView->clearFocus();
                
                Serial.println(F("LvglParameterView test: COMPLETED"));
                delay(3000);
                parameterView->deactivate();
            } else {
                Serial.println(F("LvglParameterView test: FAILED - Initialization error"));
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