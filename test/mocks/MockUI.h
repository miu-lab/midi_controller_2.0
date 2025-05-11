#pragma once
#include "core/domain/interfaces/IUISystem.hpp"
#include <vector>

/**
 * @brief Mock du système UI pour les tests
 */
class MockUI : public IUISystem {
public:
    void init(bool enableFullUI = false) override { 
        initialized = true; 
        fullUIEnabled = enableFullUI;
    }
    
    void update() override { updateCount++; }
    
    void showMessage(const std::string& message) override {
        messages.push_back(message);
    }
    
    void clearDisplay() override {
        displayCleared = true;
    }
    
    // Variables pour vérifier l'état
    bool initialized = false;
    bool fullUIEnabled = false;
    int updateCount = 0;
    bool displayCleared = false;
    
    // Pour stocker les messages affichés
    std::vector<std::string> messages;
};
