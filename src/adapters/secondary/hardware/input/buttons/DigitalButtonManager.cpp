// adapters/secondary/hardware/input/buttons/DigitalButtonManager.cpp
#include "adapters/secondary/hardware/input/buttons/DigitalButtonManager.hpp"
#include "adapters/secondary/hardware/input/buttons/ButtonFactory.hpp"

#include <Arduino.h>

DigitalButtonManager::DigitalButtonManager(const std::vector<ButtonConfig>& configs) {
    ownedButtons_.reserve(configs.size());
    buttons_.reserve(configs.size());

    for (const auto& cfg : configs) {
        // Utiliser la factory pour créer le bouton unifié avec la bonne stratégie
        auto button = ButtonFactory::createButton(cfg);

        if (button) {
            buttons_.push_back(button.get());
            ownedButtons_.push_back(std::move(button));
        } else {
            Serial.print("[DigitalButtonManager] ERROR: Failed to create button ");
            Serial.println(cfg.id);
        }
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
        btn->resetState();  // Méthode directe sur UnifiedButton
    }
}

void DigitalButtonManager::resetToggleState(ButtonId buttonId) {
    for (auto& btn : ownedButtons_) {
        if (btn->getId() == buttonId) {
            btn->resetState();  // Méthode directe sur UnifiedButton
            break;
        }
    }
}
