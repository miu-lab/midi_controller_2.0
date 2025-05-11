#pragma once

#include "core/domain/interfaces/IUISystem.hpp"
#include "core/utils/Result.hpp"
#include <vector>
#include <string>

class MockUISubsystem : public IUISystem {
public:
    bool initCalled = false;
    bool updateCalled = false;
    bool fullUIEnabled = false;
    std::vector<std::string> displayedMessages;
    bool displayCleared = false;

    // Implémentation de IUISystem
    Result<bool, std::string> init(bool enableFullUI = false) override {
        initCalled = true;
        fullUIEnabled = enableFullUI;
        return Result<bool, std::string>::success(true);
    }

    void update() override {
        updateCalled = true;
    }

    Result<bool, std::string> showMessage(const std::string& message) override {
        displayedMessages.push_back(message);
        return Result<bool, std::string>::success(true);
    }

    Result<bool, std::string> clearDisplay() override {
        displayCleared = true;
        displayedMessages.clear();
        return Result<bool, std::string>::success(true);
    }
    
    // Méthodes d'aide pour les tests
    bool hasDisplayedMessage(const std::string& message) {
        for (const auto& msg : displayedMessages) {
            if (msg == message) {
                return true;
            }
        }
        return false;
    }
    
    void resetState() {
        initCalled = false;
        updateCalled = false;
        fullUIEnabled = false;
        displayedMessages.clear();
        displayCleared = false;
    }
};
