#include "InputSubsystem.hpp"

#include <Arduino.h>
#include <algorithm>

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

    // Charger les configurations unifiées
    auto loadResult = loadUnifiedConfigurations();
    if (loadResult.isError()) {
        return loadResult;
    }

    // Connecter le contrôleur d'entrée
    auto controllerResult = connectInputController();
    if (controllerResult.isError()) {
        return controllerResult;
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

// === INTERFACE UNIFIÉE ===

Result<bool, std::string> InputSubsystem::configureInputs(const std::vector<InputConfig>& inputConfigs) {
    if (!initialized_) {
        return Result<bool, std::string>::error("InputSubsystem not initialized");
    }

    Serial.println(F("InputSubsystem: Configuring inputs with unified interface"));

    // Extraire les configurations par type
    auto encoderConfigs = extractEncoderConfigs(inputConfigs);
    auto buttonConfigs = extractButtonConfigs(inputConfigs);

    // Recréer les managers avec les nouvelles configurations
    auto managerResult = createManagers(encoderConfigs, buttonConfigs);
    if (managerResult.isError()) {
        return managerResult;
    }

    // Réinitialiser les processeurs
    auto processorResult = initializeProcessors();
    if (processorResult.isError()) {
        return processorResult;
    }

    Serial.print(F("InputSubsystem: Successfully configured "));
    Serial.print(inputConfigs.size());
    Serial.println(F(" inputs"));

    return Result<bool, std::string>::success(true);
}

std::vector<InputConfig> InputSubsystem::getAllActiveInputConfigurations() const {
    if (!configuration_) {
        return {};
    }
    
    const auto& allConfigs = configuration_->getAllInputConfigurations();
    std::vector<InputConfig> activeConfigs;
    
    // Filtrer seulement les configurations actives
    for (const auto& config : allConfigs) {
        if (config.enabled) {
            activeConfigs.push_back(config);
        }
    }
    
    return activeConfigs;
}

std::optional<InputConfig> InputSubsystem::getInputConfigurationById(InputId id) const {
    if (!configuration_) {
        return std::nullopt;
    }
    
    return configuration_->getInputConfigurationById(id);
}

size_t InputSubsystem::getActiveInputCountByType(InputType type) const {
    const auto activeConfigs = getAllActiveInputConfigurations();
    return std::count_if(activeConfigs.begin(), activeConfigs.end(),
                        [type](const InputConfig& config) {
                            return config.type == type;
                        });
}

bool InputSubsystem::validateInputsStatus() const {
    if (!initialized_) {
        return false;
    }
    
    // Vérifier que les managers sont créés
    if (!encoderManager_ || !buttonManager_) {
        return false;
    }
    
    // Vérifier que les processeurs sont créés
    if (!processEncoders_ || !processButtons_) {
        return false;
    }
    
    // Vérifier que le contrôleur est connecté
    if (!inputController_) {
        return false;
    }
    
    return true;
}

// === MÉTHODES PRIVÉES ===

Result<bool, std::string> InputSubsystem::loadUnifiedConfigurations() {
    Serial.println(F("InputSubsystem: Loading unified input configurations"));

    // Obtenir toutes les configurations d'entrée depuis la nouvelle interface
    const auto& allInputConfigs = configuration_->getAllInputConfigurations();
    
    if (allInputConfigs.empty()) {
        return Result<bool, std::string>::error("No input configurations found");
    }

    // Valider toutes les configurations
    if (!configuration_->validateAllConfigurations()) {
        return Result<bool, std::string>::error("Some input configurations are invalid");
    }

    // Extraire les configurations par type
    auto encoderConfigs = extractEncoderConfigs(allInputConfigs);
    auto buttonConfigs = extractButtonConfigs(allInputConfigs);

    Serial.print(F("InputSubsystem: Found "));
    Serial.print(encoderConfigs.size());
    Serial.print(F(" encoders and "));
    Serial.print(buttonConfigs.size());
    Serial.println(F(" buttons"));

    // Créer les managers avec les configurations extraites
    auto managerResult = createManagers(encoderConfigs, buttonConfigs);
    if (managerResult.isError()) {
        return managerResult;
    }

    // Initialiser les processeurs
    auto processorResult = initializeProcessors();
    if (processorResult.isError()) {
        return processorResult;
    }

    Serial.println(F("InputSubsystem: Unified configurations loaded successfully"));
    return Result<bool, std::string>::success(true);
}

std::vector<EncoderConfig> InputSubsystem::extractEncoderConfigs(const std::vector<InputConfig>& inputConfigs) const {
    std::vector<EncoderConfig> encoderConfigs;
    
    for (const auto& inputConfig : inputConfigs) {
        if (inputConfig.type == InputType::ENCODER && inputConfig.enabled) {
            if (auto encConfig = inputConfig.getConfig<EncoderConfig>()) {
                encoderConfigs.push_back(*encConfig);
            }
        }
    }
    
    return encoderConfigs;
}

std::vector<ButtonConfig> InputSubsystem::extractButtonConfigs(const std::vector<InputConfig>& inputConfigs) const {
    std::vector<ButtonConfig> buttonConfigs;
    
    for (const auto& inputConfig : inputConfigs) {
        if (inputConfig.type == InputType::BUTTON && inputConfig.enabled) {
            if (auto btnConfig = inputConfig.getConfig<ButtonConfig>()) {
                buttonConfigs.push_back(*btnConfig);
            }
        }
    }
    
    return buttonConfigs;
}

Result<bool, std::string> InputSubsystem::createManagers(const std::vector<EncoderConfig>& encoderConfigs,
                                                        const std::vector<ButtonConfig>& buttonConfigs) {
    // Créer le gestionnaire d'encodeurs
    encoderManager_ = std::make_shared<EncoderManager>(encoderConfigs);
    if (!encoderManager_) {
        return Result<bool, std::string>::error("Failed to create EncoderManager");
    }

    // Créer le gestionnaire de boutons
    buttonManager_ = std::make_shared<DigitalButtonManager>(buttonConfigs);
    if (!buttonManager_) {
        return Result<bool, std::string>::error("Failed to create DigitalButtonManager");
    }

    Serial.println(F("InputSubsystem: Hardware managers created successfully"));
    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> InputSubsystem::initializeProcessors() {
    // Créer les processeurs d'événements
    processEncoders_ = std::make_unique<ProcessEncoders>(encoderManager_->getEncoders());
    if (!processEncoders_) {
        return Result<bool, std::string>::error("Failed to create ProcessEncoders");
    }

    processButtons_ = std::make_unique<ProcessButtons>(buttonManager_->getButtons());
    if (!processButtons_) {
        return Result<bool, std::string>::error("Failed to create ProcessButtons");
    }

    // Connecter les processeurs au contrôleur d'entrée si disponible
    if (inputController_) {
        processEncoders_->setInputController(inputController_.get());
        processButtons_->setInputController(inputController_.get());
    }

    Serial.println(F("InputSubsystem: Event processors initialized successfully"));
    return Result<bool, std::string>::success(true);
}

Result<bool, std::string> InputSubsystem::connectInputController() {
    // Récupérer InputController depuis le conteneur
    inputController_ = container_->resolve<InputController>();
    if (!inputController_) {
        return Result<bool, std::string>::error("Failed to resolve InputController");
    }

    // Connecter les processeurs si ils existent déjà
    if (processEncoders_) {
        processEncoders_->setInputController(inputController_.get());
    }
    
    if (processButtons_) {
        processButtons_->setInputController(inputController_.get());
    }

    Serial.println(F("InputSubsystem: InputController connected successfully"));
    return Result<bool, std::string>::success(true);
}
