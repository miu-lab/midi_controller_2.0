#include "InputSubsystem.hpp"

#include "adapters/secondary/hardware/input/buttons/DigitalButtonManager.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderManager.hpp"
#include "adapters/secondary/midi/MidiMapper.hpp"
#include "core/controllers/InputController.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/use_cases/ProcessButtons.hpp"
#include "core/use_cases/ProcessEncoders.hpp"

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

    // Créer les gestionnaires de périphériques avec les configurations
    encoderManager_ = std::make_shared<EncoderManager>(encoderConfigs);
    if (!encoderManager_) {
        return Result<bool, std::string>::error("Failed to create EncoderManager");
    }

    buttonManager_ = std::make_shared<DigitalButtonManager>(buttonConfigs);
    if (!buttonManager_) {
        return Result<bool, std::string>::error("Failed to create DigitalButtonManager");
    }

    // Créer les processeurs d'événements
    processEncoders_ = std::make_unique<ProcessEncoders>(encoderManager_->getEncoders());
    if (!processEncoders_) {
        return Result<bool, std::string>::error("Failed to create ProcessEncoders");
    }

    processButtons_ = std::make_unique<ProcessButtons>(buttonManager_->getButtons());
    if (!processButtons_) {
        return Result<bool, std::string>::error("Failed to create ProcessButtons");
    }

    // Récupérer InputController depuis le conteneur
    inputController_ = container_->resolve<InputController>();
    if (!inputController_) {
        return Result<bool, std::string>::error("Failed to resolve InputController");
    } else {
        // Connecter les processeurs au contrôleur d'entrée
        processEncoders_->setInputController(inputController_.get());
        processButtons_->setInputController(inputController_.get());
    }

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

    // Traiter les événements d'encodeurs
    if (processEncoders_) {
        processEncoders_->update();
    }

    // Traiter les événements de boutons
    if (processButtons_) {
        processButtons_->update();
    }
}

Result<bool, std::string> InputSubsystem::configureEncoders(
    const std::vector<EncoderConfig>& encoderConfigs) {
    if (!encoderManager_) {
        return Result<bool, std::string>::error("EncoderManager is not initialized");
    }

    // Note: cette méthode n'est plus utilisée car les encodeurs sont configurés dans le
    // constructeur Mais nous la conservons pour l'API publique
    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> InputSubsystem::configureButtons(
    const std::vector<ButtonConfig>& buttonConfigs) {
    if (!buttonManager_) {
        return Result<bool, std::string>::error("ButtonManager is not initialized");
    }

    // Note: cette méthode n'est plus utilisée car les boutons sont configurés dans le constructeur
    // Mais nous la conservons pour l'API publique
    return Result<bool, std::string>::success(true);
}