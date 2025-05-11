#pragma once
#include <set>
#include <vector>

#include "adapters/secondary/hardware/buttons/DigitalButtonConfig.hpp"
#include "adapters/secondary/hardware/encoders/EncoderConfig.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"

/**
 * @brief Mock de IConfiguration pour les tests
 */
class MockConfiguration : public IConfiguration {
public:
    // Implémentation des méthodes avec valeurs de test par défaut
    bool isNavigationControl(ControlId id) const override {
        return navigationControls.count(id) > 0;
    }

    void setControlForNavigation(ControlId id, bool isNavigation = true) override {
        if (isNavigation)
            navigationControls.insert(id);
        else
            navigationControls.erase(id);
    }

    const std::vector<EncoderConfig>& encoderConfigs() const override {
        return encoderConfigs_;
    }

    const std::vector<ButtonConfig>& buttonConfigs() const override {
        return buttonConfigs_;
    }

    bool isDebugEnabled() const override {
        return debugEnabled;
    }

    int midiChannel() const override {
        return midiChannel_;
    }

    bool isHardwareInitEnabled() const override {
        return hardwareInitEnabled;
    }

    // Variables d'état pour les tests
    std::set<ControlId> navigationControls;
    std::vector<EncoderConfig> encoderConfigs_;
    std::vector<ButtonConfig> buttonConfigs_;
    bool debugEnabled = true;
    int midiChannel_ = 1;
    bool hardwareInitEnabled = true;
};