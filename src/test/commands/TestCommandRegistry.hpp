#pragma once

#include <Arduino.h>
#include <memory>
#include <vector>
#include "TestCommand.hpp"
#include "config/debug/DebugMacros.hpp"

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
                DEBUG_LOG(DEBUG_LEVEL_INFO, "Executing command '%c': %s", key, cmd->getDescription().c_str());
                
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
        DEBUG_LOG(DEBUG_LEVEL_INFO, "==========================================");
        DEBUG_LOG(DEBUG_LEVEL_INFO, "=== AVAILABLE TEST COMMANDS ===");
        DEBUG_LOG(DEBUG_LEVEL_INFO, "==========================================");
        
        for (const auto& cmd : commands_) {
            if (cmd) {
                DEBUG_LOG(DEBUG_LEVEL_INFO, "  %c - %s", cmd->getKey(), cmd->getDescription().c_str());
            }
        }
        
        DEBUG_LOG(DEBUG_LEVEL_INFO, "==========================================");
    }
    
    /**
     * @brief Retourne le nombre de commandes enregistrées
     */
    size_t getCommandCount() const { return commands_.size(); }
    
private:
    std::vector<std::shared_ptr<TestCommand>> commands_;
};