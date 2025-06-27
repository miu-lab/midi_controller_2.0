#pragma once

#include <Arduino.h>
#include <memory>
#include "commands/TestCommandRegistry.hpp"
#include "config/debug/DebugMacros.hpp"

/**
 * @brief Handler principal pour gérer les commandes de test via Serial
 */
class TestCommandHandler {
public:
    TestCommandHandler() : registry_(std::make_shared<TestCommandRegistry>()) {}
    
    /**
     * @brief Retourne le registry pour enregistrer des commandes
     */
    std::shared_ptr<TestCommandRegistry> getRegistry() { return registry_; }
    
    /**
     * @brief Initialise le handler et affiche le message de bienvenue
     */
    void init() {
        DEBUG_LOG(DEBUG_LEVEL_INFO, "");
        DEBUG_LOG(DEBUG_LEVEL_INFO, "==========================================");
        DEBUG_LOG(DEBUG_LEVEL_INFO, "=== TEST COMMAND HANDLER INITIALIZED ===");
        DEBUG_LOG(DEBUG_LEVEL_INFO, "==========================================");
        DEBUG_LOG(DEBUG_LEVEL_INFO, "Type 'H' or '?' for help");
        DEBUG_LOG(DEBUG_LEVEL_INFO, "");
        printPrompt();
    }
    
    /**
     * @brief Traite les commandes reçues via Serial
     */
    void handleSerialInput() {
        if (Serial.available()) {
            char command = Serial.read();
            
            // Ignorer les caractères de contrôle
            if (command == '\n' || command == '\r') {
                return;
            }
            
            DEBUG_LOG(DEBUG_LEVEL_INFO, "\nCommand received: %c", command);
            
            if (!registry_->executeCommand(command)) {
                DEBUG_LOG(DEBUG_LEVEL_WARNING, "Unknown command: '%c'. Type 'H' for help.", command);
            }
            
            DEBUG_LOG(DEBUG_LEVEL_INFO, "");
            printPrompt();
        }
    }
    
    /**
     * @brief Affiche l'invite de commande
     */
    void printPrompt() {
        Serial.print(F("TEST> "));
    }
    
private:
    std::shared_ptr<TestCommandRegistry> registry_;
};
