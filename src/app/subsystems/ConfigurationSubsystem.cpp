#include "ConfigurationSubsystem.hpp"

#include <Arduino.h>

#include "config/HardwareConfiguration.hpp"

ConfigurationSubsystem::ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    navService_ = std::make_shared<NavigationConfigService>();
    hardwareConfig_ = std::make_shared<HardwareConfiguration>();
}

Result<bool, std::string> ConfigurationSubsystem::init() {
    // Récupérer la configuration de l'application depuis le conteneur
    config_ = container_->resolve<ApplicationConfiguration>();
    if (!config_) {
        // Si aucune configuration n'est trouvée, créer une configuration par défaut
        config_ = std::make_shared<ApplicationConfiguration>();
        container_->registerDependency<ApplicationConfiguration>(config_);
    }

    // Charger les configurations d'encodeurs et de boutons
    auto encoderResult = loadEncoderConfigs();
    if (encoderResult.isError()) {
        return encoderResult;
    }

    auto buttonResult = loadButtonConfigs();
    if (buttonResult.isError()) {
        return buttonResult;
    }

    // Enregistrer ce sous-système comme implémentation de IConfiguration
    container_->registerImplementation<IConfiguration, ConfigurationSubsystem>(
        std::shared_ptr<ConfigurationSubsystem>(this, [](ConfigurationSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        }));

    return Result<bool, std::string>::success(true);
}

bool ConfigurationSubsystem::isNavigationControl(InputId id) const {
    return navService_->isNavigationControl(id);
}

void ConfigurationSubsystem::setControlForNavigation(InputId id, bool isNavigation) {
    navService_->setControlForNavigation(id, isNavigation);
}

const std::vector<EncoderConfig>& ConfigurationSubsystem::encoderConfigs() const {
    return encoderConfigs_;
}

const std::vector<ButtonConfig>& ConfigurationSubsystem::buttonConfigs() const {
    return buttonConfigs_;
}

bool ConfigurationSubsystem::isDebugEnabled() const {
    // TODO: Accéder à la configuration de debug via la structure appropriée
    // Pour l'instant, renvoyer une valeur par défaut
    return true;
}

int ConfigurationSubsystem::midiChannel() const {
    // TODO: Accéder au canal MIDI via la structure appropriée
    // Pour l'instant, renvoyer le canal 1 par défaut
    return 1;
}

bool ConfigurationSubsystem::isHardwareInitEnabled() const {
    // TODO: Vérifier si l'initialisation matérielle est activée
    // Pour l'instant, renvoyer true par défaut
    return true;
}

Result<bool, std::string> ConfigurationSubsystem::loadEncoderConfigs() {
    // Charger les configurations des encodeurs depuis la configuration matérielle
    encoderConfigs_.clear();

    Serial.println(F("ConfigurationSubsystem: Loading physical encoder configurations"));

    // Utiliser HardwareConfiguration pour charger les encodeurs
    const std::vector<EncoderConfig>& configuredEncoders =
        hardwareConfig_->getEncoderConfigurations();
    // Copier tous les encodeurs configurés
    for (const auto& encoder : configuredEncoders) {
        encoderConfigs_.push_back(encoder);
    }

    Serial.print(encoderConfigs_.size());
    Serial.println(F(" Encoders loaded"));

    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> ConfigurationSubsystem::loadButtonConfigs() {
    // Charger les configurations des boutons depuis la configuration matérielle
    buttonConfigs_.clear();

    Serial.println(F("ConfigurationSubsystem: Loading button configurations"));

    // Utiliser HardwareConfiguration pour charger les boutons
    const std::vector<ButtonConfig>& configuredButtons =
        hardwareConfig_->getControlButtonConfigurations();

    // Copier tous les boutons configurés
    for (const auto& button : configuredButtons) {
        buttonConfigs_.push_back(button);
    }

    Serial.print(buttonConfigs_.size());
    Serial.println(F(" Buttons loaded"));

    return Result<bool, std::string>::success(true);
}