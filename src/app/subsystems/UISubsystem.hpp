#pragma once

#include <memory>
#include <string>

#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IUISystem.hpp"
#include "core/utils/Result.hpp"
#include "adapters/ui/views/ViewManagerEventListener.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/secondary/hardware/display/DisplayManagerAdapter.hpp"
#include "app/factories/ViewFactory.hpp"
#include "adapters/ui/UISystemAdapter.hpp"
#include "core/domain/interfaces/IUIManager.hpp"
#include "core/domain/interfaces/IViewFactory.hpp"
#include "core/domain/interfaces/IDisplayManager.hpp"

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
    std::shared_ptr<ViewFactory> viewFactory_;
    std::shared_ptr<UISystemAdapter> uiAdapter_;

    bool fullUIEnabled_ = false;
    bool initialized_ = false;
};