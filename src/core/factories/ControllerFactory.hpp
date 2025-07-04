#pragma once

#include <memory>

#include "app/di/DependencyContainer.hpp"
#include "core/utils/Result.hpp"

class InputController;

/**
 * @brief Factory pour créer et configurer les contrôleurs
 * 
 * Cette factory centralise la logique de création des contrôleurs,
 * respectant le principe de responsabilité unique et le pattern Factory.
 */
class ControllerFactory {
public:
    /**
     * @brief Configuration pour la factory
     */
    struct FactoryConfig {
        bool validateDependencies;
        bool autoRegisterInContainer;
        
        FactoryConfig() : validateDependencies(true), autoRegisterInContainer(true) {}
    };

    /**
     * @brief Constructeur avec conteneur de dépendances
     * @param container Conteneur de dépendances
     * @param config Configuration de la factory
     */
    explicit ControllerFactory(std::shared_ptr<DependencyContainer> container,
                              const FactoryConfig& config = FactoryConfig());

    /**
     * @brief Destructeur par défaut
     */
    ~ControllerFactory() = default;

    /**
     * @brief Crée un InputController avec dépendances résolues
     * @return Result<std::shared_ptr<InputController>> Contrôleur créé ou erreur
     */
    Result<std::shared_ptr<InputController>> createInputController();

    /**
     * @brief Valide que toutes les dépendances requises sont disponibles
     * @return Result<bool> Succès ou erreur avec détails
     */
    Result<bool> validateDependencies() const;

    /**
     * @brief Vérifie si la factory est opérationnelle
     * @return true si le conteneur est disponible et configuré
     */
    bool isOperational() const;

private:
    std::shared_ptr<DependencyContainer> container_;
    FactoryConfig config_;

    /**
     * @brief Enregistre un contrôleur dans le conteneur si configuré
     * @tparam T Type du contrôleur
     * @param controller Contrôleur à enregistrer
     */
    template<typename T>
    void registerControllerIfConfigured(std::shared_ptr<T> controller);

    /**
     * @brief Valide qu'un contrôleur créé est valide
     * @tparam T Type du contrôleur
     * @param controller Contrôleur à valider
     * @return Result<bool> Succès ou erreur
     */
    template<typename T>
    Result<bool> validateController(std::shared_ptr<T> controller) const;
};