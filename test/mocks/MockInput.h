#pragma once
#include "core/domain/interfaces/IInputSystem.hpp"

/**
 * @brief Mock du système d'entrée pour les tests
 */
class MockInput : public IInputSystem {
public:
    void init() override { initialized = true; }
    void update() override { updateCount++; }
    
    // Variables pour vérifier l'état
    bool initialized = false;
    int updateCount = 0;
};
