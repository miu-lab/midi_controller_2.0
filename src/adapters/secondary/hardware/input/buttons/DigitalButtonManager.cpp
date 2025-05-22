// adapters/secondary/hardware/input/buttons/DigitalButtonManager.cpp
#include "adapters/secondary/hardware/input/buttons/DigitalButtonManager.hpp"

#include <Arduino.h>

DigitalButtonManager::DigitalButtonManager(const std::vector<ButtonConfig>& configs) {
    ownedButtons_.reserve(configs.size());
    buttons_.reserve(configs.size());

    for (const auto& cfg : configs) {
        // Crée un Button à partir de ButtonConfig
        ownedButtons_.emplace_back(std::make_unique<DigitalButton>(cfg));
        buttons_.push_back(ownedButtons_.back().get());
    }
}

DigitalButtonManager::~DigitalButtonManager() = default;

void DigitalButtonManager::updateAll() {
    for (auto& btn : ownedButtons_) {
        btn->update();
    }
}

const std::vector<ButtonPort*>& DigitalButtonManager::getButtons() const {
    return buttons_;
}

void DigitalButtonManager::resetAllToggleStates() {
    for (auto& btn : ownedButtons_) {
        btn->resetState();
    }
}

void DigitalButtonManager::resetToggleState(ButtonId buttonId) {
    for (auto& btn : ownedButtons_) {
        if (btn->getId() == buttonId) {
            btn->resetState();
            break;
        }
    }
}
