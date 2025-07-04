#pragma once

#include <memory>
#include "core/utils/Result.hpp"

// Forward declarations
class ViewManager;
class DependencyContainer;
class DefaultViewManager;
class Ili9341LvglBridge;
class UnifiedConfiguration;
class EventBus;

/**
 * @brief Factory pour la création de ViewManager
 * 
 * Cette classe centralise la logique de création des gestionnaires de vues
 * en utilisant l'injection de dépendances et en respectant les configurations.
 */
class ViewFactory {
public:
    /**
     * @brief Configuration pour la création de ViewManager
     */
    struct ViewManagerConfig {
        bool enableFullUI;
        bool enableEventListener;
        bool registerInContainer;
        
        ViewManagerConfig() 
            : enableFullUI(false)
            , enableEventListener(true)
            , registerInContainer(true) {}
    };

    /**
     * @brief Constructeur avec conteneur de dépendances
     * @param container Conteneur pour résoudre les dépendances
     */
    explicit ViewFactory(std::shared_ptr<DependencyContainer> container);

    /**
     * @brief Destructeur
     */
    ~ViewFactory() = default;

    /**
     * @brief Crée un ViewManager avec la configuration spécifiée
     * @param config Configuration pour la création
     * @return Result contenant le ViewManager créé ou une erreur
     */
    Result<std::shared_ptr<ViewManager>> createViewManager(const ViewManagerConfig& config = ViewManagerConfig());

    /**
     * @brief Crée un DefaultViewManager avec les dépendances LVGL
     * @return Result contenant le DefaultViewManager créé ou une erreur
     */
    Result<std::shared_ptr<DefaultViewManager>> createDefaultViewManager();

    /**
     * @brief Vérifie si toutes les dépendances nécessaires sont disponibles
     * @return true si toutes les dépendances sont présentes
     */
    bool validateDependencies() const;

private:
    std::shared_ptr<DependencyContainer> container_;

    /**
     * @brief Résout les dépendances LVGL nécessaires
     * @param lvglBridge [out] Bridge LVGL résolu
     * @param unifiedConfig [out] Configuration unifiée résolue
     * @param eventBus [out] Bus d'événements résolu
     * @return true si toutes les dépendances ont été résolues
     */
    bool resolveLvglDependencies(
        std::shared_ptr<Ili9341LvglBridge>& lvglBridge,
        std::shared_ptr<UnifiedConfiguration>& unifiedConfig,
        std::shared_ptr<EventBus>& eventBus) const;
};