#pragma once

#include <memory>
#include <string>

#include "core/domain/interfaces/IUIManager.hpp"
#include "core/utils/Result.hpp"
#include "UIProcessorManager.hpp"
#include "config/SystemConstants.hpp"

// Forward declarations
class ViewManager;
class IDisplayManager;
class ViewManagerEventListener;

namespace MidiController::Events {
    class IEventBus;
}

/**
 * @brief Adapter pour la gestion centralisée du système UI
 * 
 * Cette classe implémente IUIManager et centralise la logique UI
 * qui était précédemment dans UISystemCore, respectant l'architecture hexagonale.
 */
class UISystemAdapter : public IUIManager {
public:
    /**
     * @brief Constructeur avec configuration
     * @param config Configuration du système UI
     */
    explicit UISystemAdapter(const UIConfig& config = UIConfig());

    /**
     * @brief Destructeur
     */
    ~UISystemAdapter() override = default;

    /**
     * @brief Initialise le système UI avec un bus d'événements
     * @param eventBus Bus d'événements unifié
     * @return Result indiquant le succès ou l'erreur
     */
    Result<bool> initialize(std::shared_ptr<MidiController::Events::IEventBus> eventBus) override;

    /**
     * @brief Met à jour tous les composants UI dans le bon ordre
     */
    void update() override;

    /**
     * @brief Affiche un message modal
     * @param message Message à afficher
     * @return Result indiquant le succès ou l'erreur
     */
    Result<bool> showMessage(const std::string& message) override;

    /**
     * @brief Efface l'affichage et ferme les modals
     * @return Result indiquant le succès ou l'erreur
     */
    Result<bool> clearDisplay() override;

    /**
     * @brief Vérifie si le système est initialisé
     * @return true si initialisé
     */
    bool isInitialized() const override;

    /**
     * @brief Vérifie si le système est opérationnel
     * @return true si tous les composants sont présents et l'UI activée
     */
    bool isOperational() const override;

    /**
     * @brief Configure l'écouteur d'événements UI
     * @param eventListener Écouteur d'événements à configurer
     * @return Result indiquant le succès ou l'erreur
     */
    Result<bool> configureEventListener(std::unique_ptr<ViewManagerEventListener> eventListener);

    /**
     * @brief Obtient le gestionnaire de vues
     * @return Pointeur vers ViewManager ou nullptr si non initialisé
     */
    std::shared_ptr<ViewManager> getViewManager() const;

    /**
     * @brief Initialise avec tous les composants nécessaires
     * @param viewManager Gestionnaire de vues
     * @param displayManager Gestionnaire d'affichage
     * @param eventBus Bus d'événements unifié
     * @return Result indiquant le succès ou l'erreur
     */
    Result<bool> initializeWithComponents(
        std::shared_ptr<ViewManager> viewManager,
        std::unique_ptr<IDisplayManager> displayManager,
        std::shared_ptr<MidiController::Events::IEventBus> eventBus);

private:
    UIConfig config_;
    bool initialized_;

    // === SYSTÈME DE PROCESSORS ===
    std::unique_ptr<UIProcessorManager> processorManager_;
    std::unique_ptr<ViewManagerEventListener> eventListener_;

    /**
     * @brief Valide que tous les composants requis sont présents
     * @return true si la validation réussit
     */
    bool validateComponents() const;

    /**
     * @brief Vérifie si la configuration système est valide
     */
    bool isConfigurationValid() const;
};