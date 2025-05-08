#pragma once
#include "core/domain/IButton.hpp"
#include <vector>

class ProcessButtons {
public:
    explicit ProcessButtons(const std::vector<IButton*>& buttons);
    void update();
    
    // Initialise les états sans déclencher d'événements
    void initStates();

private:
    std::vector<IButton*> buttons_;
    std::vector<bool> lastPressed_;
    bool initialized_;
};
