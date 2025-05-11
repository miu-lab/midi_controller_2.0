#pragma once
#include <set>
#include <vector>
#include "core/domain/interfaces/IConfiguration.hpp"
#include "adapters/secondary/hardware/encoders/EncoderConfig.hpp"
#include "adapters/secondary/hardware/buttons/DigitalButtonConfig.hpp"

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
        if (isNavigation) navigationControls.insert(id);
        else navigationControls.erase(id);
    }
    
    const std::vector<EncoderConfig>& getEncoderConfigs() const override { 
        return encoderConfigs; 
    }
    
    const std::vector<ButtonConfig>& getButtonConfigs() const override { 
        return buttonConfigs; 
    }
    
    bool isDebugEnabled() const override { 
        return debugEnabled; 
    }
    
    int getMidiChannel() const override { 
        return midiChannel; 
    }
    
    bool isHardwareInitEnabled() const override { 
        return hardwareInitEnabled; 
    }
    
    // Variables d'état pour les tests
    std::set<ControlId> navigationControls;
    std::vector<EncoderConfig> encoderConfigs;
    std::vector<ButtonConfig> buttonConfigs;
    bool debugEnabled = true;
    int midiChannel = 1;
    bool hardwareInitEnabled = true;
};