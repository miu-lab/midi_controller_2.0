#include "app/services/ConfigurationService.hpp"

ConfigurationService::ConfigurationService(const ApplicationConfiguration& appConfig)
    : appConfig_(appConfig) {
}

void ConfigurationService::applyConfigurations(ProfileManager& profileManager, 
                                              NavigationConfigService& navService) {
    // Appliquer les mappings MIDI
    const auto& midiMappings = appConfig_.getMappingConfiguration().getAllMidiMappings();
    for (const auto& mapping : midiMappings) {
        profileManager.setBinding(mapping.controlId, mapping.midiControl);
    }
    
    // Appliquer les contrôles de navigation
    const auto& navigationControls = appConfig_.getMappingConfiguration().getNavigationControls();
    for (const auto& navControl : navigationControls) {
        navService.setControlForNavigation(navControl.id, true);
    }
}

const std::vector<EncoderConfig>& ConfigurationService::getEncoderConfigs() const {
    return appConfig_.getHardwareConfiguration().getEncoderConfigurations();
}

const std::vector<ButtonConfig>& ConfigurationService::getButtonConfigs() const {
    return appConfig_.getHardwareConfiguration().getControlButtonConfigurations();
}
