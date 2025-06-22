#pragma once

#include <Arduino.h>
#include <memory>
#include <vector>
#include "TestCommand.hpp"

/**
 * @brief Registry pour gérer toutes les commandes de test
 */
class TestCommandRegistry {
public:
    static constexpr size_t MAX_COMMANDS = 20;
    
    TestCommandRegistry() {
        commands_.reserve(MAX_COMMANDS);
    }
    
    /**
     * @brief Enregistre une nouvelle commande
     */
    void registerCommand(std::shared_ptr<TestCommand> command) {
        if (commands_.size() < MAX_COMMANDS) {
            commands_.push_back(command);
        }
    }
    
    /**
     * @brief Enregistre une commande simple avec fonction
     */
    void registerCommand(char key, const String& description, TestCommand::ExecuteFunction func) {
        registerCommand(std::make_shared<TestCommand>(key, description, func));
    }
    
    /**
     * @brief Exécute la commande correspondant à la touche
     */
    bool executeCommand(char key) {
        for (const auto& cmd : commands_) {
            if (cmd && cmd->matches(key)) {
                Serial.print(F("Executing command '"));
                Serial.print(key);
                Serial.print(F("': "));
                Serial.println(cmd->getDescription());
                
                cmd->execute();
                return true;
            }
        }
        return false;
    }
    
    /**
     * @brief Affiche toutes les commandes disponibles
     */
    void printHelp() const {
        Serial.println(F("=========================================="));
        Serial.println(F("=== AVAILABLE TEST COMMANDS ==="));
        Serial.println(F("=========================================="));
        
        for (const auto& cmd : commands_) {
            if (cmd) {
                Serial.print(F("  "));
                Serial.print(cmd->getKey());
                Serial.print(F(" - "));
                Serial.println(cmd->getDescription());
            }
        }
        
        Serial.println(F("=========================================="));
    }
    
    /**
     * @brief Retourne le nombre de commandes enregistrées
     */
    size_t getCommandCount() const { return commands_.size(); }
    
private:
    std::vector<std::shared_ptr<TestCommand>> commands_;
};