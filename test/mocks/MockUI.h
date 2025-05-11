#pragma once
#include "core/domain/interfaces/IUISystem.hpp"
#include <vector>

/**
 * @brief Mock du système UI pour les tests
 */
class MockUI : public IUISystem {
public:
    Result<bool, std::string> init(bool enableFullUI = false) override { 
        initialized = true; 
        fullUIEnabled = enableFullUI;
        return Result<bool, std::string>::success(true);
    }
    
    void update() override { updateCount++; }
    
    Result<bool, std::string> showMessage(const std::string& message) override {
        messages.push_back(message);
        return Result<bool, std::string>::success(true);
    }
    
    Result<bool, std::string> clearDisplay() override {
        displayCleared = true;
        return Result<bool, std::string>::success(true);
    }
    
    // Variables pour vérifier l'état
    bool initialized = false;
    bool fullUIEnabled = false;
    int updateCount = 0;
    bool displayCleared = false;
    
    // Pour stocker les messages affichés
    std::vector<std::string> messages;
};
