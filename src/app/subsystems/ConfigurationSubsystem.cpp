#include "ConfigurationSubsystem.hpp"

ConfigurationSubsystem::ConfigurationSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
    navService_ = std::make_shared<NavigationConfigService>();
}

void ConfigurationSubsystem::init() {
    // Récupérer la configuration de l'application depuis le conteneur
    config_ = container_->resolve<ApplicationConfiguration>();
    if (!config_) {
        // Si aucune configuration n'est trouvée, créer une configuration par défaut
        config_ = std::make_shared<ApplicationConfiguration>();
        container_->registerDependency<ApplicationConfiguration>(config_);
    }
    
    // Charger les configurations d'encodeurs et de boutons
    loadEncoderConfigs();
    loadButtonConfigs();
    
    // Enregistrer ce sous-système comme implémentation de IConfiguration
    container_->registerImplementation<IConfiguration, ConfigurationSubsystem>(
        std::shared_ptr<ConfigurationSubsystem>(this, [](ConfigurationSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        })
    );
}

bool ConfigurationSubsystem::isNavigationControl(ControlId id) const {
    return navService_->isNavigationControl(id);
}

void ConfigurationSubsystem::setControlForNavigation(ControlId id, bool isNavigation) {
    navService_->setControlForNavigation(id, isNavigation);
}

const std::vector<EncoderConfig>& ConfigurationSubsystem::getEncoderConfigs() const {
    return encoderConfigs_;
}

const std::vector<ButtonConfig>& ConfigurationSubsystem::getButtonConfigs() const {
    return buttonConfigs_;
}

bool ConfigurationSubsystem::isDebugEnabled() const {
    // TODO: Accéder à la configuration de debug via la structure appropriée
    // Pour l'instant, renvoyer une valeur par défaut
    return true;
}

int ConfigurationSubsystem::getMidiChannel() const {
    // TODO: Accéder au canal MIDI via la structure appropriée
    // Pour l'instant, renvoyer le canal 1 par défaut
    return 1;
}

bool ConfigurationSubsystem::isHardwareInitEnabled() const {
    // TODO: Vérifier si l'initialisation matérielle est activée
    // Pour l'instant, renvoyer true par défaut
    return true;
}

void ConfigurationSubsystem::loadEncoderConfigs() {
    // Charger les configurations des encodeurs depuis la configuration matérielle
    // Pour l'instant, utiliser des configurations par défaut
    encoderConfigs_.clear();
    encoderConfigs_.push_back(EncoderConfig{1, 2, 3, 1});  // Pins A, B, Bouton, ID
    encoderConfigs_.push_back(EncoderConfig{4, 5, 6, 2});
}

void ConfigurationSubsystem::loadButtonConfigs() {
    // Charger les configurations des boutons depuis la configuration matérielle
    // Pour l'instant, utiliser des configurations par défaut
    buttonConfigs_.clear();
    buttonConfigs_.push_back(ButtonConfig{10, 3});  // Pin, ID
    buttonConfigs_.push_back(ButtonConfig{11, 4});
}