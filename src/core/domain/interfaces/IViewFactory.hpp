#pragma once

#include <memory>
#include "core/utils/Result.hpp"

// Forward declarations
class ViewManager;

/**
 * @brief Interface pour la création de ViewManager
 * 
 * Cette interface définit les opérations pour créer des gestionnaires
 * de vues dans le système UI.
 */
class IViewFactory {
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
     * @brief Destructeur virtuel
     */
    virtual ~IViewFactory() = default;

    /**
     * @brief Crée un ViewManager avec la configuration spécifiée
     * @param config Configuration pour la création
     * @return Result contenant le ViewManager créé ou une erreur
     */
    virtual Result<std::shared_ptr<ViewManager>> createViewManager(const ViewManagerConfig& config = ViewManagerConfig()) = 0;

    /**
     * @brief Vérifie si toutes les dépendances nécessaires sont disponibles
     * @return true si toutes les dépendances sont présentes
     */
    virtual bool validateDependencies() const = 0;
};