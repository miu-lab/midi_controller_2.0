#pragma once

#include <memory>
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"

/**
 * @brief Gestionnaire d'enregistrement des configurations dans le conteneur DI
 * 
 * Responsable de l'enregistrement des objets de configuration dans le conteneur
 * de dépendances avec les custom deleters appropriés.
 */
class ConfigurationRegistry {
public:
    /**
     * @brief Constructeur avec conteneur de dépendances
     * @param container Conteneur pour l'enregistrement des dépendances
     */
    explicit ConfigurationRegistry(std::shared_ptr<DependencyContainer> container);
    
    /**
     * @brief Enregistre une implémentation de ConfigurationSubsystem
     * @param configSubsystem Instance à enregistrer
     * @return true si l'enregistrement a réussi
     */
    bool registerConfigurationSubsystem(std::shared_ptr<void> configSubsystem);
    
    /**
     * @brief Enregistre une configuration unifiée
     * @param appConfig Configuration application contenant la configuration unifiée
     * @return true si l'enregistrement a réussi
     */
    bool registerUnifiedConfiguration(std::shared_ptr<ApplicationConfiguration> appConfig);

private:
    std::shared_ptr<DependencyContainer> container_;
    
    /**
     * @brief Custom deleter pour ConfigurationSubsystem (ne fait rien)
     */
    template<typename T>
    static void noOpDeleter(T*) {
        // Ne rien faire lors de la destruction (le conteneur ne possède pas cet objet)
    }
};