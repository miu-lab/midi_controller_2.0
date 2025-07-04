#include "InputManager.hpp"

#include "adapters/secondary/hardware/input/buttons/DigitalButtonManager.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderManager.hpp"
#include "core/controllers/InputController.hpp"
#include "core/use_cases/ProcessButtons.hpp"
#include "core/use_cases/ProcessEncoders.hpp"
#include "core/utils/Error.hpp"

InputManager::InputManager(const ManagerConfig& config)
    : config_(config)
    , initialized_(false) {
}

InputManager::~InputManager() = default;

Result<bool> InputManager::initialize(const std::vector<ControlDefinition>& controlDefinitions,
                                     std::shared_ptr<InputController> inputController) {
    if (initialized_) {
        return Result<bool>::success(true);
    }

    // Stocker le contrôleur d'entrée
    inputController_ = inputController;

    // Extraire les configurations par type
    auto encoderConfigs = extractEncoderConfigs(controlDefinitions);
    auto buttonConfigs = extractButtonConfigs(controlDefinitions);

    // Créer les gestionnaires de matériel
    auto managerResult = createManagers(encoderConfigs, buttonConfigs);
    if (!managerResult.isSuccess()) {
        return managerResult;
    }

    // Initialiser les processeurs d'événements si activés
    if (config_.enableEventProcessing) {
        auto processorResult = initializeProcessors();
        if (!processorResult.isSuccess()) {
            return processorResult;
        }

        // Connecter les processeurs au contrôleur
        connectProcessors();
    }

    initialized_ = true;
    return Result<bool>::success(true);
}

void InputManager::update() {
    if (!isOperational()) {
        return;
    }

    // Mettre à jour les gestionnaires de matériel
    if (config_.enableEncoders && encoderManager_) {
        encoderManager_->updateAll();
    }

    if (config_.enableButtons && buttonManager_) {
        buttonManager_->updateAll();
    }

    // Mettre à jour les processeurs d'événements
    if (config_.enableEventProcessing) {
        if (processEncoders_) {
            processEncoders_->update();
        }

        if (processButtons_) {
            processButtons_->update();
        }
    }
}

Result<bool> InputManager::reconfigure(const std::vector<ControlDefinition>& controlDefinitions) {
    if (!initialized_) {
        return Result<bool>::error(Error(ErrorCode::OperationFailed, "InputManager not initialized"));
    }

    // Réinitialiser complètement avec les nouvelles configurations
    initialized_ = false;
    
    // Nettoyer les anciens composants
    processEncoders_.reset();
    processButtons_.reset();
    encoderManager_.reset();
    buttonManager_.reset();

    // Réinitialiser avec les nouvelles définitions
    return initialize(controlDefinitions, inputController_);
}

bool InputManager::isOperational() const {
    if (!initialized_) {
        return false;
    }

    // Vérifier que les gestionnaires requis sont créés
    if (config_.enableEncoders && !encoderManager_) {
        return false;
    }

    if (config_.enableButtons && !buttonManager_) {
        return false;
    }

    // Vérifier que les processeurs requis sont créés
    if (config_.enableEventProcessing) {
        if (config_.enableEncoders && !processEncoders_) {
            return false;
        }

        if (config_.enableButtons && !processButtons_) {
            return false;
        }
    }

    return true;
}

std::shared_ptr<EncoderManager> InputManager::getEncoderManager() const {
    return encoderManager_;
}

std::shared_ptr<DigitalButtonManager> InputManager::getButtonManager() const {
    return buttonManager_;
}

std::vector<EncoderConfig> InputManager::extractEncoderConfigs(const std::vector<ControlDefinition>& controlDefinitions) const {
    std::vector<EncoderConfig> encoderConfigs;
    
    for (const auto& controlDef : controlDefinitions) {
        if (controlDef.hardware.type == InputType::ENCODER && controlDef.enabled) {
            if (auto encConfig = std::get_if<ControlDefinition::EncoderConfig>(&controlDef.hardware.config)) {
                EncoderConfig hwConfig;
                hwConfig.id = controlDef.id;
                hwConfig.pinA = encConfig->pinA;
                hwConfig.pinB = encConfig->pinB;
                hwConfig.ppr = encConfig->ppr;
                hwConfig.sensitivity = encConfig->sensitivity;
                hwConfig.enableAcceleration = encConfig->enableAcceleration;
                hwConfig.stepsPerDetent = encConfig->stepsPerDetent;
                hwConfig.invertDirection = false;      // Valeur par défaut
                hwConfig.accelerationThreshold = 100;  // Valeur par défaut
                hwConfig.maxAcceleration = 5.0f;       // Valeur par défaut
                
                encoderConfigs.push_back(hwConfig);
            }
        }
    }
    
    return encoderConfigs;
}

std::vector<ButtonConfig> InputManager::extractButtonConfigs(const std::vector<ControlDefinition>& controlDefinitions) const {
    std::vector<ButtonConfig> buttonConfigs;
    
    for (const auto& controlDef : controlDefinitions) {
        if (controlDef.hardware.type == InputType::BUTTON && controlDef.enabled) {
            if (auto btnConfig = std::get_if<ControlDefinition::ButtonConfig>(&controlDef.hardware.config)) {
                ButtonConfig hwConfig;
                hwConfig.id = controlDef.id;
                hwConfig.gpio = btnConfig->pin;
                hwConfig.activeLow = btnConfig->activeLow;
                hwConfig.mode = btnConfig->mode;
                hwConfig.debounceMs = btnConfig->debounceMs;
                if (btnConfig->longPressMs.has_value()) {
                    hwConfig.longPressMs = *btnConfig->longPressMs;
                    hwConfig.enableLongPress = true;
                } else {
                    hwConfig.longPressMs = 800;  // Valeur par défaut
                    hwConfig.enableLongPress = false;
                }
                
                buttonConfigs.push_back(hwConfig);
            }
        }
    }
    
    return buttonConfigs;
}

Result<bool> InputManager::createManagers(const std::vector<EncoderConfig>& encoderConfigs,
                                         const std::vector<ButtonConfig>& buttonConfigs) {
    // Créer le gestionnaire d'encodeurs si activé et qu'il y a des configurations
    if (config_.enableEncoders && !encoderConfigs.empty()) {
        encoderManager_ = std::make_shared<EncoderManager>(encoderConfigs);
        if (!encoderManager_) {
            return Result<bool>::error(Error(ErrorCode::InitializationFailed, "Failed to create EncoderManager"));
        }
    }

    // Créer le gestionnaire de boutons si activé et qu'il y a des configurations
    if (config_.enableButtons && !buttonConfigs.empty()) {
        buttonManager_ = std::make_shared<DigitalButtonManager>(buttonConfigs);
        if (!buttonManager_) {
            return Result<bool>::error(Error(ErrorCode::InitializationFailed, "Failed to create DigitalButtonManager"));
        }
    }

    return Result<bool>::success(true);
}

Result<bool> InputManager::initializeProcessors() {
    // Créer le processeur d'encodeurs si activé et gestionnaire disponible
    if (config_.enableEncoders && encoderManager_) {
        processEncoders_ = std::make_unique<ProcessEncoders>(encoderManager_->getEncoders());
        if (!processEncoders_) {
            return Result<bool>::error(Error(ErrorCode::InitializationFailed, "Failed to create ProcessEncoders"));
        }
    }

    // Créer le processeur de boutons si activé et gestionnaire disponible
    if (config_.enableButtons && buttonManager_) {
        processButtons_ = std::make_unique<ProcessButtons>(buttonManager_->getButtons());
        if (!processButtons_) {
            return Result<bool>::error(Error(ErrorCode::InitializationFailed, "Failed to create ProcessButtons"));
        }
    }

    return Result<bool>::success(true);
}

void InputManager::connectProcessors() {
    if (!inputController_) {
        return;
    }

    // Connecter les processeurs au contrôleur d'entrée
    if (processEncoders_) {
        processEncoders_->setInputController(inputController_.get());
    }
    
    if (processButtons_) {
        processButtons_->setInputController(inputController_.get());
    }
}