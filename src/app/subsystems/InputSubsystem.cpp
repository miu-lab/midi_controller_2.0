#include "InputSubsystem.hpp"

#include "adapters/secondary/hardware/buttons/DigitalButtonManager.hpp"
#include "adapters/secondary/hardware/encoders/EncoderManager.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"

InputSubsystem::InputSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container), initialized_(false) {}

Result<bool, std::string> InputSubsystem::init() {
    if (initialized_) {
        return Result<bool, std::string>::success(true);
    }

    // Récupérer la configuration
    configuration_ = container_->resolve<IConfiguration>();
    if (!configuration_) {
        return Result<bool, std::string>::error("Failed to resolve IConfiguration");
    }

    // Obtenir les configurations des périphériques
    auto encoderConfigs = configuration_->encoderConfigs();
    auto buttonConfigs = configuration_->buttonConfigs();

    // Dans un environnement sans exceptions, nous vérifions simplement les erreurs directement
    // Créer les gestionnaires de périphériques avec les configurations
    encoderManager_ = std::make_shared<EncoderManager>(encoderConfigs);
    if (!encoderManager_) {
        return Result<bool, std::string>::error("Failed to create EncoderManager");
    }
    
    buttonManager_ = std::make_shared<DigitalButtonManager>(buttonConfigs);
    if (!buttonManager_) {
        return Result<bool, std::string>::error("Failed to create DigitalButtonManager");
    }

    // Pas besoin d'appeler configureEncoders et configureButtons séparément
    // car nous avons déjà fourni les configurations aux constructeurs

    // Les périphériques sont déjà configurés dans les constructeurs

    // Enregistrer ce sous-système comme implémentation de IInputSystem
    container_->registerImplementation<IInputSystem, InputSubsystem>(
        std::shared_ptr<InputSubsystem>(this, [](InputSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        }));

    initialized_ = true;
    return Result<bool, std::string>::success(true);
}

void InputSubsystem::update() {
    if (!initialized_) {
        return;
    }

    // Mettre à jour l'état des encodeurs
    if (encoderManager_) {
        encoderManager_->updateAll();
    }

    // Mettre à jour l'état des boutons
    if (buttonManager_) {
        buttonManager_->updateAll();
    }
}

Result<bool, std::string> InputSubsystem::configureEncoders(const std::vector<EncoderConfig>& encoderConfigs) {
    if (!encoderManager_) {
        return Result<bool, std::string>::error("EncoderManager is not initialized");
    }

    // Note: cette méthode n'est plus utilisée car les encodeurs sont configurés dans le
    // constructeur Mais nous la conservons pour l'API publique
    // TODO: Implémenter la reconfiguration des encodeurs si nécessaire
    
    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> InputSubsystem::configureButtons(const std::vector<ButtonConfig>& buttonConfigs) {
    if (!buttonManager_) {
        return Result<bool, std::string>::error("ButtonManager is not initialized");
    }

    // Note: cette méthode n'est plus utilisée car les boutons sont configurés dans le constructeur
    // Mais nous la conservons pour l'API publique
    // TODO: Implémenter la reconfiguration des boutons si nécessaire
    
    return Result<bool, std::string>::success(true);
}