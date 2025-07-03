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
    performanceSettings = {ConfigDefaults::PERFORMANCE_MODE,
                           ConfigDefaults::MAX_UPDATE_TIME_US,
                           ConfigDefaults::MAX_INPUT_TIME_US,
                           ConfigDefaults::MAX_MIDI_TIME_US,
                           ConfigDefaults::MAX_UI_TIME_US};

    // Initialisation des paramètres d'interface utilisateur
    uiSettings = {ConfigDefaults::DISPLAY_REFRESH_RATE_HZ,
                  ConfigDefaults::DISPLAY_REFRESH_PERIOD_MS,
                  ConfigDefaults::SHOW_DEBUG_INFO,
                  ConfigDefaults::ENABLE_FULL_UI};

    // Initialisation des paramètres de contrôle
    controlSettings = {ConfigDefaults::DEFAULT_ENCODER_SENSITIVITY,
                       ConfigDefaults::ENCODER_RATE_LIMIT_MS};

    // Initialisation des paramètres MIDI
    midiSettings = {ConfigDefaults::DEFAULT_MIDI_CHANNEL,
                    ConfigDefaults::CC_VALUE_MIN,
                    ConfigDefaults::CC_VALUE_MAX};

    // Initialisation des paramètres système
    systemSettings = {ConfigDefaults::SERIAL_BAUD_RATE, ConfigDefaults::MAX_COMMAND_HISTORY};
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
    performanceSettings = {ConfigDefaults::PERFORMANCE_MODE,
                           ConfigDefaults::MAX_UPDATE_TIME_US,
                           ConfigDefaults::MAX_INPUT_TIME_US,
                           ConfigDefaults::MAX_MIDI_TIME_US,
                           ConfigDefaults::MAX_UI_TIME_US};

    // Réinitialiser UI
    uiSettings = {ConfigDefaults::DISPLAY_REFRESH_RATE_HZ,
                  ConfigDefaults::DISPLAY_REFRESH_PERIOD_MS,
                  ConfigDefaults::SHOW_DEBUG_INFO,
                  ConfigDefaults::ENABLE_FULL_UI};

    // Réinitialiser contrôles
    controlSettings = {ConfigDefaults::DEFAULT_ENCODER_SENSITIVITY,
                       ConfigDefaults::ENCODER_RATE_LIMIT_MS};

    // Réinitialiser MIDI
    midiSettings = {ConfigDefaults::DEFAULT_MIDI_CHANNEL,
                    ConfigDefaults::CC_VALUE_MIN,
                    ConfigDefaults::CC_VALUE_MAX};

    // Réinitialiser système
    systemSettings = {ConfigDefaults::SERIAL_BAUD_RATE, ConfigDefaults::MAX_COMMAND_HISTORY};

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