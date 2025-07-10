#include "ControllerFactory.hpp"

#include "core/controllers/InputController.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "config/unified/UnifiedConfiguration.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/utils/Error.hpp"

ControllerFactory::ControllerFactory(std::shared_ptr<DependencyContainer> container,
                                   const FactoryConfig& config)
    : container_(container)
    , config_(config) {
}

Result<std::shared_ptr<InputController>> ControllerFactory::createInputController() {
    if (!isOperational()) {
        return Result<std::shared_ptr<InputController>>::error(
            Error(ErrorCode::DependencyMissing, "ControllerFactory not operational")
        );
    }

    // Valider les dépendances si configuré
    if (config_.validateDependencies) {
        auto validationResult = validateDependencies();
        if (!validationResult.isSuccess()) {
            return Result<std::shared_ptr<InputController>>::error(validationResult.error().value());
        }
    }

    // Résoudre NavigationConfigService depuis le conteneur
    auto navigationConfig = container_->resolve<NavigationConfigService>();
    if (!navigationConfig) {
        return Result<std::shared_ptr<InputController>>::error(
            Error(ErrorCode::DependencyMissing, "Failed to resolve NavigationConfigService for InputController")
        );
    }

    // Résoudre UnifiedConfiguration
    auto unifiedConfig = container_->resolve<UnifiedConfiguration>();
    if (!unifiedConfig) {
        return Result<std::shared_ptr<InputController>>::error(
            Error(ErrorCode::DependencyMissing, "Failed to resolve UnifiedConfiguration for InputController")
        );
    }

    // Résoudre EventBus (optionnel)
    auto eventBus = container_->resolve<EventBus>();

    // Créer InputController avec les dépendances résolues
    auto inputController = std::make_shared<InputController>(navigationConfig, unifiedConfig, eventBus);
    if (!inputController) {
        return Result<std::shared_ptr<InputController>>::error(
            Error(ErrorCode::InitializationFailed, "Failed to create InputController")
        );
    }

    // Valider le contrôleur créé
    auto validationResult = validateController(inputController);
    if (!validationResult.isSuccess()) {
        return Result<std::shared_ptr<InputController>>::error(validationResult.error().value());
    }

    // Enregistrer dans le conteneur si configuré
    registerControllerIfConfigured(inputController);

    return Result<std::shared_ptr<InputController>>::success(inputController);
}

Result<bool> ControllerFactory::validateDependencies() const {
    if (!container_) {
        return Result<bool>::error(
            Error(ErrorCode::DependencyMissing, "DependencyContainer not available")
        );
    }

    // Vérifier que NavigationConfigService est disponible
    auto navigationConfig = container_->resolve<NavigationConfigService>();
    if (!navigationConfig) {
        return Result<bool>::error(
            Error(ErrorCode::DependencyMissing, "NavigationConfigService dependency missing")
        );
    }

    return Result<bool>::success(true);
}

bool ControllerFactory::isOperational() const {
    return container_ != nullptr;
}

template<typename T>
void ControllerFactory::registerControllerIfConfigured(std::shared_ptr<T> controller) {
    if (!config_.autoRegisterInContainer || !container_ || !controller) {
        return;
    }

    // Enregistrer le contrôleur dans le conteneur pour une utilisation future
    container_->registerDependency<T>(controller);
}

template<typename T>
Result<bool> ControllerFactory::validateController(std::shared_ptr<T> controller) const {
    if (!controller) {
        return Result<bool>::error(
            Error(ErrorCode::OperationFailed, "Controller is null")
        );
    }

    // Validation spécifique pour InputController
    if constexpr (std::is_same_v<T, InputController>) {
        // Vérifier que le contrôleur a été correctement initialisé
        // (ici on pourrait ajouter des vérifications spécifiques si nécessaire)
    }

    return Result<bool>::success(true);
}

// Instanciation explicite des templates pour InputController
template void ControllerFactory::registerControllerIfConfigured<InputController>(std::shared_ptr<InputController>);
template Result<bool> ControllerFactory::validateController<InputController>(std::shared_ptr<InputController>) const;