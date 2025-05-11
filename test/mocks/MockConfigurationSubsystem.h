#pragma once

#include "core/domain/interfaces/IConfiguration.hpp"
#include <vector>

class MockConfigurationSubsystem : public IConfiguration {
public:
    bool initCalled = false;
    std::vector<EncoderConfig> testEncoderConfigs;
    std::vector<ButtonConfig> testButtonConfigs;
    bool debugEnabled = false;
    int testMidiChannel = 1;
    bool hardwareInitEnabled = true;
    
    // Map pour stocker les contrôles de navigation
    std::unordered_map<ControlId, bool> navigationControls;

    // Implémentation de IConfiguration
    Result<bool, std::string> init() override {
        initCalled = true;
        return Result<bool, std::string>::success(true);
    }
    // Implémentation de IConfiguration
    bool isNavigationControl(ControlId id) const override {
        auto it = navigationControls.find(id);
        if (it != navigationControls.end()) {
            return it->second;
        }
        return false;
    }

    void setControlForNavigation(ControlId id, bool isNavigation = true) override {
        navigationControls[id] = isNavigation;
    }

    const std::vector<EncoderConfig>& encoderConfigs() const override {
        return testEncoderConfigs;
    }

    const std::vector<ButtonConfig>& buttonConfigs() const override {
        return testButtonConfigs;
    }

    bool isDebugEnabled() const override {
        return debugEnabled;
    }

    int midiChannel() const override {
        return testMidiChannel;
    }

    bool isHardwareInitEnabled() const override {
        return hardwareInitEnabled;
    }
};
