#pragma once

#include <Arduino.h>
#include <memory>
#include "commands/TestCommandRegistry.hpp"

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
        Serial.println(F(""));
        Serial.println(F("=========================================="));
        Serial.println(F("=== TEST COMMAND HANDLER INITIALIZED ==="));
        Serial.println(F("=========================================="));
        Serial.println(F("Type 'H' or '?' for help"));
        Serial.println(F(""));
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
            
            Serial.print(F("\nCommand received: "));
            Serial.println(command);
            
            if (!registry_->executeCommand(command)) {
                Serial.print(F("Unknown command: '"));
                Serial.print(command);
                Serial.println(F("'. Type 'H' for help."));
            }
            
            Serial.println(F(""));
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