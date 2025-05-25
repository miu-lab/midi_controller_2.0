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

Result<bool, std::string> InputSubsystem::configureInputs(const std::vector<ControlDefinition>& controlDefinitions) {
    if (!initialized_) {
        return Result<bool, std::string>::error("InputSubsystem not initialized");
    }

    Serial.println(F("InputSubsystem: Configuring inputs with ControlDefinition interface"));

    // Extraire les configurations par type
    auto encoderConfigs = extractEncoderConfigs(controlDefinitions);
    auto buttonConfigs = extractButtonConfigs(controlDefinitions);

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
    Serial.print(controlDefinitions.size());
    Serial.println(F(" controls"));

    return Result<bool, std::string>::success(true);
}

std::vector<ControlDefinition> InputSubsystem::getAllActiveControlDefinitions() const {
    if (!configuration_) {
        return {};
    }
    
    const auto& allControls = configuration_->getAllControlDefinitions();
    std::vector<ControlDefinition> activeControls;
    
    // Filtrer seulement les contrôles actifs
    for (const auto& control : allControls) {
        if (control.enabled) {
            activeControls.push_back(control);
        }
    }
    
    return activeControls;
}

std::optional<ControlDefinition> InputSubsystem::getControlDefinitionById(InputId id) const {
    if (!configuration_) {
        return std::nullopt;
    }
    
    return configuration_->getControlDefinitionById(id);
}

size_t InputSubsystem::getActiveInputCountByType(InputType type) const {
    const auto activeControls = getAllActiveControlDefinitions();
    return std::count_if(activeControls.begin(), activeControls.end(),
                        [type](const ControlDefinition& control) {
                            return control.hardware.type == type;
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

Result<bool, std::string> InputSubsystem::loadUnifiedConfigurations() {
    Serial.println(F("InputSubsystem: Loading unified control definitions"));

    // Obtenir toutes les définitions de contrôles depuis la nouvelle interface
    const auto& allControlDefinitions = configuration_->getAllControlDefinitions();
    
    if (allControlDefinitions.empty()) {
        return Result<bool, std::string>::error("No control definitions found");
    }

    // Valider toutes les définitions
    if (!configuration_->validateAllConfigurations()) {
        return Result<bool, std::string>::error("Some control definitions are invalid");
    }

    // Extraire les configurations par type
    auto encoderConfigs = extractEncoderConfigs(allControlDefinitions);
    auto buttonConfigs = extractButtonConfigs(allControlDefinitions);

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

    Serial.println(F("InputSubsystem: Unified control definitions loaded successfully"));
    return Result<bool, std::string>::success(true);
}

std::vector<EncoderConfig> InputSubsystem::extractEncoderConfigs(const std::vector<ControlDefinition>& controlDefinitions) const {
    std::vector<EncoderConfig> encoderConfigs;
    
    for (const auto& controlDef : controlDefinitions) {
        if (controlDef.hardware.type == InputType::ENCODER && controlDef.enabled) {
            // Extraire la configuration d'encodeur depuis ControlDefinition
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
                
                // Ajouter le bouton si présent
                if (controlDef.hardware.encoderButtonPin) {
                    ButtonConfig btnConfig;
                    btnConfig.id = controlDef.getEncoderButtonId();
                    btnConfig.gpio = *controlDef.hardware.encoderButtonPin;
                    btnConfig.activeLow = true;
                    btnConfig.mode = ButtonMode::MOMENTARY;
                    btnConfig.debounceMs = controlDef.hardware.encoderButtonDebounceMs.value_or(30);
                    btnConfig.enableLongPress = false;
                    btnConfig.longPressMs = 800;
                    hwConfig.buttonConfig = btnConfig;
                }
                
                encoderConfigs.push_back(hwConfig);
            }
        }
    }
    
    return encoderConfigs;
}

std::vector<ButtonConfig> InputSubsystem::extractButtonConfigs(const std::vector<ControlDefinition>& controlDefinitions) const {
    std::vector<ButtonConfig> buttonConfigs;
    
    for (const auto& controlDef : controlDefinitions) {
        if (controlDef.hardware.type == InputType::BUTTON && controlDef.enabled) {
            // Extraire la configuration de bouton depuis ControlDefinition
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
