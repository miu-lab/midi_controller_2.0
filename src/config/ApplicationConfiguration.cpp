#include "config/ApplicationConfiguration.hpp"

#include <Arduino.h>

#include "config/unified/ConfigurationFactory.hpp"  // Pour la factory STL
#include "config/unified/UnifiedConfiguration.hpp"

ApplicationConfiguration::ApplicationConfiguration() {
    // === INITIALISATION DU SYSTÈME UNIFIÉ MODERNE ===

    unifiedConfig_ = ConfigurationFactory::createDefaultConfiguration();
    if (!unifiedConfig_) {
        // En cas d'échec critique, impossible de continuer
        // TODO DEBUG MSG
        return;
    }

    // Affichage des statistiques (Serial peut ne pas être initialisé ici)
    // // TODO DEBUG MSG
    // // TODO DEBUG MSG
    // Initialisation des paramètres de performance
    performanceSettings = {SystemConstants::Performance::PERFORMANCE_MODE_DEFAULT,
                           SystemConstants::Performance::MAX_UPDATE_TIME_US,
                           SystemConstants::Performance::INPUT_TIME_INTERVAL,
                           SystemConstants::Performance::MIDI_TIME_INTERVAL,
                           SystemConstants::Performance::UI_TIME_INTERVAL};

    // Initialisation des paramètres d'interface utilisateur
    uiSettings = {SystemConstants::Performance::DISPLAY_REFRESH_RATE_HZ,
                  SystemConstants::Performance::DISPLAY_REFRESH_PERIOD_MS,
                  SystemConstants::UI::SHOW_DEBUG_INFO,
                  SystemConstants::UI::ENABLE_FULL_UI};

    // Initialisation des paramètres de contrôle
    controlSettings = {SystemConstants::Input::DEFAULT_ENCODER_SENSITIVITY,
                       SystemConstants::Performance::ENCODER_RATE_LIMIT_MS};

    // Initialisation des paramètres MIDI
    midiSettings = {SystemConstants::Audio::DEFAULT_CHANNEL,
                    SystemConstants::Audio::CC_VALUE_MIN,
                    SystemConstants::Audio::CC_VALUE_MAX};

    // Initialisation des paramètres système
    systemSettings = {SystemConstants::Hardware::SERIAL_BAUD_RATE, SystemConstants::Hardware::MAX_COMMAND_HISTORY};
}

const ApplicationConfiguration::PerformanceSettings&
ApplicationConfiguration::getPerformanceSettings() const {
    return performanceSettings;
}

void ApplicationConfiguration::setPerformanceSettings(const PerformanceSettings& settings) {
    performanceSettings = settings;
    notifyChange("performanceSettings");
}

const ApplicationConfiguration::UISettings& ApplicationConfiguration::getUISettings() const {
    return uiSettings;
}

void ApplicationConfiguration::setUISettings(const UISettings& settings) {
    uiSettings = settings;
    notifyChange("uiSettings");
}

const ApplicationConfiguration::ControlSettings& ApplicationConfiguration::getControlSettings()
    const {
    return controlSettings;
}

void ApplicationConfiguration::setControlSettings(const ControlSettings& settings) {
    controlSettings = settings;
    notifyChange("controlSettings");
}

const ApplicationConfiguration::MidiSettings& ApplicationConfiguration::getMidiSettings() const {
    return midiSettings;
}

void ApplicationConfiguration::setMidiSettings(const MidiSettings& settings) {
    midiSettings = settings;
    notifyChange("midiSettings");
}

const ApplicationConfiguration::SystemSettings& ApplicationConfiguration::getSystemSettings()
    const {
    return systemSettings;
}

void ApplicationConfiguration::setSystemSettings(const SystemSettings& settings) {
    systemSettings = settings;
    notifyChange("systemSettings");
}

void ApplicationConfiguration::setEncoderSensitivity(float value) {
    controlSettings.encoderSensitivity = value;
    notifyChange("encoderSensitivity");
}

float ApplicationConfiguration::getEncoderSensitivity() const {
    return controlSettings.encoderSensitivity;
}

void ApplicationConfiguration::setOperationMode(OperationMode mode) {
    switch (mode) {
    case OperationMode::NORMAL:
        performanceSettings.performanceMode = false;
        uiSettings.showDebugInfo = false;
        uiSettings.enableFullUI = true;
        break;

    case OperationMode::PERFORMANCE:
        performanceSettings.performanceMode = true;
        uiSettings.showDebugInfo = false;
        uiSettings.enableFullUI = true;
        break;

    case OperationMode::DEBUG_MODE:  // Mise à jour du nom ici aussi
        performanceSettings.performanceMode = false;
        uiSettings.showDebugInfo = true;
        uiSettings.enableFullUI = true;
        break;
    }

    notifyChange("operationMode");
}

bool ApplicationConfiguration::loadFromStorage() {
    // À implémenter pour charger depuis EEPROM ou SD
    // TODO: Implémenter la persistence
    return false;
}

bool ApplicationConfiguration::saveToStorage() const {
    // À implémenter pour sauvegarder dans EEPROM ou SD
    // TODO: Implémenter la persistence
    return false;
}

void ApplicationConfiguration::resetToDefaults() {
    // Réinitialiser performance
    performanceSettings = {SystemConstants::Performance::PERFORMANCE_MODE_DEFAULT,
                           SystemConstants::Performance::MAX_UPDATE_TIME_US,
                           SystemConstants::Performance::INPUT_TIME_INTERVAL,
                           SystemConstants::Performance::MIDI_TIME_INTERVAL,
                           SystemConstants::Performance::UI_TIME_INTERVAL};

    // Réinitialiser UI
    uiSettings = {SystemConstants::Performance::DISPLAY_REFRESH_RATE_HZ,
                  SystemConstants::Performance::DISPLAY_REFRESH_PERIOD_MS,
                  SystemConstants::UI::SHOW_DEBUG_INFO,
                  SystemConstants::UI::ENABLE_FULL_UI};

    // Réinitialiser contrôles
    controlSettings = {SystemConstants::Input::DEFAULT_ENCODER_SENSITIVITY,
                       SystemConstants::Performance::ENCODER_RATE_LIMIT_MS};

    // Réinitialiser MIDI
    midiSettings = {SystemConstants::Audio::DEFAULT_CHANNEL,
                    SystemConstants::Audio::CC_VALUE_MIN,
                    SystemConstants::Audio::CC_VALUE_MAX};

    // Réinitialiser système
    systemSettings = {SystemConstants::Hardware::SERIAL_BAUD_RATE, SystemConstants::Hardware::MAX_COMMAND_HISTORY};

    notifyChange("allSettings");
}

void ApplicationConfiguration::registerChangeCallback(ChangeCallback callback) {
    changeCallbacks.push_back(callback);
}

void ApplicationConfiguration::unregisterChangeCallback(const ChangeCallback& callback) {
    // Note: Cette implémentation est simplifiée car on ne peut pas directement comparer des
    // std::function En pratique, il faudrait utiliser un identifiant ou un système d'abonnement
    // plus sophistiqué
    if (!changeCallbacks.empty()) {
        changeCallbacks.pop_back();
    }
}

void ApplicationConfiguration::notifyChange(const std::string& paramName) {
    for (const auto& callback : changeCallbacks) {
        if (callback) {
            callback(paramName);
        }
    }
}

// === IMPLÉMENTATION DES NOUVELLES MÉTHODES POUR LE SYSTÈME UNIFIÉ ===

const UnifiedConfiguration& ApplicationConfiguration::getUnifiedConfiguration() const {
    if (unifiedConfig_) {
        return *unifiedConfig_;
    }
    // Fallback - ne devrait jamais arriver mais pour sécurité
    static UnifiedConfiguration emptyConfig;
    return emptyConfig;
}

void ApplicationConfiguration::enableUnifiedSystem() {
    // TODO DEBUG MSG
    notifyChange("configurationSystem");
}