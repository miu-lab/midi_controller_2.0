#include "InputSubsystem.hpp"

#include <algorithm>

// #include "NavigationSubsystem.hpp" // TODO: Réintégrer après refactoring interface
#include "core/utils/Error.hpp"
#include "core/controllers/InputController.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"

InputSubsystem::InputSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container), initialized_(false) {
    // Créer les composants délégués
    if (container_) {
        InputManager::ManagerConfig managerConfig;
        inputManager_ = std::make_unique<InputManager>(managerConfig);
        
        ControllerFactory::FactoryConfig factoryConfig;
        controllerFactory_ = std::make_unique<ControllerFactory>(container_, factoryConfig);
    }
}

Result<bool> InputSubsystem::init() {
    if (initialized_) {
        return Result<bool>::success(true);
    }

    // Récupérer la configuration
    configuration_ = container_->resolve<IConfiguration>();
    if (!configuration_) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Failed to resolve IConfiguration"});
    }

    // REFACTOR: Récupérer le service de navigation
    navigationService_ = container_->resolve<INavigationService>();
    if (!navigationService_) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "Failed to resolve INavigationService"});
    }

    // Initialiser les composants délégués
    auto delegateResult = initializeDelegatedComponents();
    if (!delegateResult.isSuccess()) {
        return delegateResult;
    }

    // Configurer avec les définitions de contrôles
    auto controlDefinitions = configuration_->getAllControlDefinitions();
    auto setupResult = setupInputManager(controlDefinitions);
    if (!setupResult.isSuccess()) {
        return setupResult;
    }

    // REFACTOR: Configurer les contrôles de navigation
    auto navigationResult = configureNavigationControls(controlDefinitions);
    if (!navigationResult.isSuccess()) {
        return navigationResult;
    }

    // Enregistrer ce sous-système comme implémentation de IInputSystem
    container_->registerImplementation<IInputSystem, InputSubsystem>(
        std::shared_ptr<InputSubsystem>(this, [](InputSubsystem*) {
            // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
        }));

    initialized_ = true;
    return Result<bool>::success(true);
}

void InputSubsystem::update() {
    if (!initialized_ || !inputManager_) {
        return;
    }

    // Déléguer la mise à jour à InputManager
    inputManager_->update();
}

Result<bool> InputSubsystem::configureInputs(const std::vector<ControlDefinition>& controlDefinitions) {
    if (!initialized_ || !inputManager_) {
        return Result<bool>::error({ErrorCode::OperationFailed, "InputSubsystem not initialized"});
    }

    // Déléguer la reconfiguration à InputManager
    return inputManager_->reconfigure(controlDefinitions);
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

Result<void> InputSubsystem::validateInputsStatus() const {
    if (!initialized_) {
        return Result<void>::error({ErrorCode::OperationFailed, "InputSubsystem not initialized"});
    }
    
    // Vérifier que InputManager est opérationnel
    if (!inputManager_) {
        return Result<void>::error({ErrorCode::DependencyMissing, "InputManager not available"});
    }
    
    if (!inputManager_->isOperational()) {
        return Result<void>::error({ErrorCode::OperationFailed, "InputManager not operational"});
    }
    
    // Vérifier que le contrôleur est créé
    if (!inputController_) {
        return Result<void>::error({ErrorCode::DependencyMissing, "InputController not created"});
    }
    
    return Result<void>::success();
}

Result<bool> InputSubsystem::initializeDelegatedComponents() {
    if (!controllerFactory_) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "ControllerFactory not available"});
    }

    // Créer InputController via ControllerFactory
    auto controllerResult = controllerFactory_->createInputController();
    if (!controllerResult.isSuccess()) {
        return Result<bool>::error(controllerResult.error().value());
    }

    inputController_ = controllerResult.value().value();
    return Result<bool>::success(true);
}

Result<bool> InputSubsystem::setupInputManager(const std::vector<ControlDefinition>& controlDefinitions) {
    if (!inputManager_ || !inputController_) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "InputManager or InputController not available"});
    }

    // Valider les configurations
    if (controlDefinitions.empty()) {
        return Result<bool>::error({ErrorCode::ConfigError, "No control definitions found"});
    }

    if (!configuration_->validateAllConfigurations()) {
        return Result<bool>::error({ErrorCode::ConfigError, "Some control definitions are invalid"});
    }

    // Initialiser InputManager avec les définitions et le contrôleur
    auto initResult = inputManager_->initialize(controlDefinitions, inputController_);
    if (!initResult.isSuccess()) {
        return initResult;
    }

    return Result<bool>::success(true);
}

Result<bool> InputSubsystem::configureNavigationControls(const std::vector<ControlDefinition>& controlDefinitions) {
    if (!navigationService_) {
        return Result<bool>::error({ErrorCode::DependencyMissing, "NavigationService not available"});
    }

    // REFACTOR: La configuration détaillée est maintenant gérée par NavigationSubsystem
    // InputSubsystem ne fait plus que déléguer
    
    // TODO: Récupérer le NavigationSubsystem depuis le container si disponible
    // auto navigationSubsystem = container_->resolve<NavigationSubsystem>();
    // if (navigationSubsystem) {
    //     // Utiliser le nouveau NavigationSubsystem pour configuration avancée
    //     auto result = navigationSubsystem->configureNavigationControls(controlDefinitions);
    //     if (!result.isSuccess()) {
    //         return result;
    //     }
    // }
    
    // Identifier les contrôles de navigation pour l'ancien service (compatibilité)
    std::set<InputId> navigationControlIds;
    
    for (const auto& controlDef : controlDefinitions) {
        if (!controlDef.enabled) continue;
        
        // Vérifier si le contrôle a des mappings de navigation
        for (const auto& mapping : controlDef.mappings) {
            if (mapping.role == MappingRole::NAVIGATION) {
                navigationControlIds.insert(controlDef.id);
                break;
            }
        }
    }
    
    // Configurer l'ancien service de navigation (compatibilité)
    navigationService_->setNavigationControls(navigationControlIds);
    
    return Result<bool>::success(true);
}