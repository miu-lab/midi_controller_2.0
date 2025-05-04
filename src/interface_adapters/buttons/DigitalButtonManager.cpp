// interface_adapters/buttons/DigitalButtonManager.cpp
#ifdef UNIT_TEST_NATIVE
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include "interface_adapters/buttons/DigitalButtonManager.hpp"

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

const std::vector<IButton*>& DigitalButtonManager::getButtons() const {
    return buttons_;
}
