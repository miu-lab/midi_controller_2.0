#pragma once

#include <Arduino.h>
#include <memory>

#include "TestCommandHandler.hpp"
#include "commands/DisplayTestCommands.hpp"
#include "../adapters/secondary/hardware/display/Ili9341Driver.hpp"
#include "../adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "../adapters/primary/ui/DisplayTestSuite.hpp"

/**
 * @brief Mode de test hardware complet
 * 
 * Cette classe encapsule toute la logique de test hardware
 * pour garder main.cpp propre et simple
 */
class HardwareTestMode {
public:
    HardwareTestMode() = default;
    
    /**
     * @brief Initialise le mode test hardware
     * @return true si l'initialisation réussit
     */
    bool init() {
        Serial.println(F(""));
        Serial.println(F("========================================="));
        Serial.println(F("=== HARDWARE TEST MODE - PHASE 1 ==="));
        Serial.println(F("========================================="));
        Serial.println(F(""));
        
        // 1. Initialiser le driver hardware
        if (!initializeDriver()) {
            return false;
        }
        
        // 2. Initialiser le bridge LVGL
        if (!initializeLvglBridge()) {
            return false;
        }
        
        // 3. Créer la suite de tests
        if (!createTestSuite()) {
            return false;
        }
        
        // 4. Initialiser le handler de commandes
        initializeCommandHandler();
        
        // 5. Lancer les tests automatiques au démarrage
        if (runStartupTests_) {
            Serial.println(F("Running automated test suite..."));
            testSuite_->runFullHardwareTestSuite();
        }
        
        // 6. Afficher l'aide
        commandHandler_->init();
        
        return true;
    }
    
    /**
     * @brief Méthode update à appeler dans loop()
     */
    void update() {
        // Traiter les commandes Serial
        commandHandler_->handleSerialInput();
        
        // Rafraîchir l'affichage LVGL
        if (bridge_) {
            bridge_->refreshDisplay();
        }
    }
    
    /**
     * @brief Active/désactive les tests au démarrage
     */
    void setRunStartupTests(bool run) { runStartupTests_ = run; }
    
private:
    // Composants du système
    std::shared_ptr<Ili9341Driver> driver_;
    std::shared_ptr<Ili9341LvglBridge> bridge_;
    std::shared_ptr<DisplayTestSuite> testSuite_;
    std::shared_ptr<TestCommandHandler> commandHandler_;
    
    // Configuration
    bool runStartupTests_ = true;
    
    /**
     * @brief Initialise le driver hardware
     */
    bool initializeDriver() {
        Serial.println(F("Initializing hardware driver..."));
        
        Ili9341Driver::Config driverConfig = Ili9341Driver::getDefaultConfig();
        driver_ = std::make_shared<Ili9341Driver>(driverConfig);
        
        if (!driver_->initialize()) {
            Serial.println(F("FATAL: Hardware driver initialization failed!"));
            Serial.println(F("Please check:"));
            Serial.println(F("  - SPI connections (MOSI, MISO, SCK)"));
            Serial.println(F("  - Power supply (3.3V)"));
            Serial.println(F("  - CS, DC, RST pins"));
            return false;
        }
        
        Serial.println(F("Hardware driver initialized successfully!"));
        return true;
    }
    
    /**
     * @brief Initialise le bridge LVGL
     */
    bool initializeLvglBridge() {
        Serial.println(F("Initializing LVGL bridge..."));
        
        Ili9341LvglBridge::LvglConfig lvglConfig = Ili9341LvglBridge::getDefaultLvglConfig();
        bridge_ = std::make_shared<Ili9341LvglBridge>(driver_, lvglConfig);
        
        if (!bridge_->initialize()) {
            Serial.println(F("FATAL: LVGL bridge initialization failed!"));
            return false;
        }
        
        Serial.println(F("LVGL bridge initialized successfully!"));
        return true;
    }
    
    /**
     * @brief Crée la suite de tests
     */
    bool createTestSuite() {
        Serial.println(F("Creating test suite..."));
        testSuite_ = std::make_shared<DisplayTestSuite>(driver_, bridge_);
        Serial.println(F("Test suite ready!"));
        return true;
    }
    
    /**
     * @brief Initialise le handler de commandes et enregistre toutes les commandes
     */
    void initializeCommandHandler() {
        commandHandler_ = std::make_shared<TestCommandHandler>();
        
        // Enregistrer toutes les commandes de test du display
        DisplayTestCommands::registerCommands(
            *commandHandler_->getRegistry(),
            driver_,
            bridge_,
            testSuite_
        );
        
        Serial.print(F("Registered "));
        Serial.print(commandHandler_->getRegistry()->getCommandCount());
        Serial.println(F(" test commands"));
    }
};