#pragma once

#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/utils/Result.hpp"

class MockInputSubsystem : public IInputSystem {
public:
    bool initCalled = false;
    bool updateCalled = false;
    bool encodersConfigured = false;
    bool buttonsConfigured = false;
    std::vector<EncoderConfig> lastEncoderConfigs;
    std::vector<ButtonConfig> lastButtonConfigs;

    // Implémentation de IInputSystem
    Result<bool, std::string> init() override {
        initCalled = true;
        return Result<bool, std::string>::success(true);
    }

    void update() override {
        updateCalled = true;
    }

    Result<bool, std::string> configureEncoders(const std::vector<EncoderConfig>& encoderConfigs) override {
        encodersConfigured = true;
        lastEncoderConfigs = encoderConfigs;
        return Result<bool, std::string>::success(true);
    }

    Result<bool, std::string> configureButtons(const std::vector<ButtonConfig>& buttonConfigs) override {
        buttonsConfigured = true;
        lastButtonConfigs = buttonConfigs;
        return Result<bool, std::string>::success(true);
    }
};
