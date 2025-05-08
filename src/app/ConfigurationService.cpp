#include "app/ConfigurationService.hpp"
#include "config/ControlEncodersConfig.hpp"
#include "config/ControlButtonsConfig.hpp"
#include "config/MidiMappingConfig.hpp"
#include "config/NavigationMappingConfig.hpp"

ConfigurationService::ConfigurationService() {
    // Initialisation avec des vecteurs vides
}

void ConfigurationService::loadDefaultConfigurations() {
    // Charger toutes les configurations par défaut
    loadEncoderConfigs();
    loadButtonConfigs();
    loadMidiMappings();
    loadNavigationControls();
}

void ConfigurationService::applyConfigurations(ProfileManager& profileManager, 
                                              NavigationConfigService& navService) {
    // Appliquer les mappings MIDI
    for (const auto& mapping : midiMappings_) {
        profileManager.setBinding(mapping.controlId, mapping.midiControl);
    }
    
    // Appliquer les contrôles de navigation
    for (const auto& navControl : navigationControls_) {
        navService.setControlForNavigation(navControl.id, true);
    }
}

const std::vector<EncoderConfig>& ConfigurationService::getEncoderConfigs() const {
    return encoderConfigs_;
}

const std::vector<ButtonConfig>& ConfigurationService::getButtonConfigs() const {
    return buttonConfigs_;
}

void ConfigurationService::loadEncoderConfigs() {
    // Copier les configurations depuis le fichier de configuration externe
    encoderConfigs_ = controlEncoderConfigs;
}

void ConfigurationService::loadButtonConfigs() {
    // Copier les configurations depuis le fichier de configuration externe
    buttonConfigs_ = controlButtonConfigs;
}

void ConfigurationService::loadMidiMappings() {
    // Copier les mappings depuis le fichier de configuration externe
    midiMappings_.clear();
    for (std::size_t i = 0; i < defaultMidiMappingCount; i++) {
        midiMappings_.push_back(defaultMidiMappings[i]);
    }
}

void ConfigurationService::loadNavigationControls() {
    // Copier les contrôles de navigation depuis le fichier de configuration externe
    navigationControls_.clear();
    for (std::size_t i = 0; i < navigationControlCount; i++) {
        navigationControls_.push_back(navigationControls[i]);
    }
}
