#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IUISystem.hpp"
#include "core/utils/Result.hpp"
#include "adapters/primary/ui/ViewManagerEventListener.hpp"
#include "core/domain/events/EventBatcher.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "core/ui/DisplayManager.hpp"
#include "core/ui/ViewFactory.hpp"
#include "core/ui/UISystemCore.hpp"

class ViewManager;

/**
 * @brief Sous-système d'interface utilisateur
 *
 * Cette classe implémente l'interface IUISystem et gère tous
 * les aspects de l'interface utilisateur.
 */
class UISubsystem : public IUISystem {
public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param container Conteneur de dépendances
     */
    explicit UISubsystem(std::shared_ptr<DependencyContainer> container);

    /**
     * @brief Destructeur par défaut
     */
    ~UISubsystem() override = default;

    /**
     * @brief Initialise le sous-système d'interface utilisateur
     *
     * @param enableFullUI Indique si l'interface complète doit être activée
     * @return Result<bool> Succès ou message d'erreur
     */
    Result<bool> init(bool enableFullUI = false);

    /**
     * @brief Met à jour l'état du sous-système d'interface utilisateur
     */
    void update() override;

    /**
     * @brief Affiche un message sur l'écran
     *
     * @param message Le message à afficher
     * @return Result<bool> Succès ou message d'erreur
     */
    Result<bool> showMessage(const std::string& message);

    /**
     * @brief Efface l'écran
     *
     * @return Result<bool> Succès ou message d'erreur
     */
    Result<bool> clearDisplay();

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<IConfiguration> configuration_;
    std::shared_ptr<Ili9341LvglBridge> m_lvglBridge;
    std::unique_ptr<ViewFactory> viewFactory_;
    std::unique_ptr<UISystemCore> uiCore_;

    bool fullUIEnabled_ = false;
    bool initialized_ = false;
};