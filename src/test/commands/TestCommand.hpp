#pragma once

#include <Arduino.h>
#include <functional>
#include <memory>

/**
 * @brief Interface de base pour une commande de test
 */
class TestCommand {
public:
    using ExecuteFunction = std::function<void()>;
    
    TestCommand(char key, const String& description, ExecuteFunction execute)
        : key_(key), description_(description), execute_(execute) {}
    
    virtual ~TestCommand() = default;
    
    char getKey() const { return key_; }
    const String& getDescription() const { return description_; }
    
    virtual void execute() {
        if (execute_) {
            execute_();
        }
    }
    
    virtual bool matches(char input) const {
        return (input == key_ || input == (key_ + 32) || input == (key_ - 32));
    }
    
protected:
    char key_;
    String description_;
    ExecuteFunction execute_;
};