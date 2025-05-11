#pragma once
#include "core/domain/interfaces/IInputSystem.hpp"

/**
 * @brief Mock du système d'entrée pour les tests
 */
class MockInput : public IInputSystem {
public:
    // Méthodes implémentant l'interface IInputSystem avec Result
    Result<bool, std::string> init() override { 
        initialized = true; 
        return Result<bool, std::string>::success(true);
    }
    
    void update() override { 
        updateCount++; 
    }
    
    Result<bool, std::string> configureEncoders(const std::vector<EncoderConfig>& encoderConfigs) override {
        return Result<bool, std::string>::success(true);
    }
    
    Result<bool, std::string> configureButtons(const std::vector<ButtonConfig>& buttonConfigs) override {
        return Result<bool, std::string>::success(true);
    }
    
    // Variables pour vérifier l'état
    bool initialized = false;
    int updateCount = 0;
};
