#pragma once
#include "domain/IButton.hpp"
#include <vector>

class ProcessButtons {
public:
    explicit ProcessButtons(const std::vector<IButton*>& buttons);
    void update();

private:
    std::vector<IButton*> buttons_;
    std::vector<bool> lastPressed_;
};
