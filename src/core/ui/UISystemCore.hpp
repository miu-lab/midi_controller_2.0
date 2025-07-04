#pragma once

#include <memory>
#include <string>
#include "core/utils/Result.hpp"

// Forward declarations
class ViewManager;
class DisplayManager;
class EventManager;
class ViewManagerEventListener;

/**
 * @brief Noyau centralisé pour la logique UI
 * 
 * Cette classe centralise la logique métier du système UI qui était
 * précédemment dispersée dans UISubsystem, en respectant le principe SRP.
 */
class UISystemCore {
public:
    /**
     * @brief Configuration pour UISystemCore
     */
    struct CoreConfig {
        bool enableFullUI;
        bool enableEventProcessing;
        bool enableDisplayRefresh;
        
        CoreConfig() 
            : enableFullUI(false)
            , enableEventProcessing(true) 
            , enableDisplayRefresh(true) {}
    };

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration du noyau UI
     */
    explicit UISystemCore(const CoreConfig& config = CoreConfig());

    /**
     * @brief Destructeur
     */
    ~UISystemCore() = default;

    /**
     * @brief Initialise le noyau UI avec les composants nécessaires
     * @param viewManager Gestionnaire de vues
     * @param displayManager Gestionnaire d'affichage
     * @param eventManager Gestionnaire d'événements unifié
     * @return Result indiquant le succès ou l'erreur
     */
    Result<bool> initialize(
        std::shared_ptr<ViewManager> viewManager,
        std::unique_ptr<DisplayManager> displayManager,
        std::unique_ptr<EventManager> eventManager);

    /**
     * @brief Met à jour tous les composants UI dans le bon ordre
     */
    void update();

    /**
     * @brief Affiche un message modal
     * @param message Message à afficher
     * @return Result indiquant le succès ou l'erreur
     */
    Result<bool> showMessage(const std::string& message);

    /**
     * @brief Efface l'affichage et ferme les modals
     * @return Result indiquant le succès ou l'erreur
     */
    Result<bool> clearDisplay();

    /**
     * @brief Vérifie si le noyau est initialisé
     * @return true si initialisé
     */
    bool isInitialized() const;

    /**
     * @brief Vérifie si le noyau est opérationnel
     * @return true si tous les composants sont présents et l'UI activée
     */
    bool isOperational() const;

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

private:
    CoreConfig config_;
    bool initialized_;

    // Composants UI centralisés
    std::shared_ptr<ViewManager> viewManager_;
    std::unique_ptr<DisplayManager> displayManager_;
    std::unique_ptr<EventManager> eventManager_;
    std::unique_ptr<ViewManagerEventListener> eventListener_;

    /**
     * @brief Valide que tous les composants requis sont présents
     * @return true si la validation réussit
     */
    bool validateComponents() const;

    /**
     * @brief Traite les événements UI via EventManager
     */
    void processEvents();

    /**
     * @brief Met à jour le gestionnaire de vues
     */
    void updateViewManager();

    /**
     * @brief Rafraîchit l'affichage
     */
    void refreshDisplay();
};