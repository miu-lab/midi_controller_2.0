#pragma once

#include <functional>
#include <memory>
#include <string>
#include <any>
#include "core/utils/Result.hpp"
#include "config/ETLConfig.hpp"

// Helper pour générer un ID de type sans utiliser typeid
class TypeIdGenerator {
public:
    template <typename T>
    static const void* getTypeId() {
        static char typeId;
        return &typeId;
    }
};

/**
 * @brief Conteneur pour l'injection de dépendances
 * 
 * Cette classe centralise la gestion des dépendances entre composants.
 * Elle permet d'enregistrer des instances et des fabriques, puis de les
 * récupérer au besoin.
 */
class DependencyContainer {
public:
    DependencyContainer() = default;
    ~DependencyContainer() = default;

    /**
     * @brief Enregistre une instance partagée
     * 
     * @tparam T Type de l'instance
     * @param instance Instance à enregistrer
     */
    template<typename T>
    void registerDependency(std::shared_ptr<T> instance) {
        dependencies_[TypeIdGenerator::getTypeId<T>()] = instance;
    }

    /**
     * @brief Enregistre une instance comme implémentation d'une interface
     * 
     * @tparam TInterface Type de l'interface
     * @tparam TImplementation Type de l'implémentation
     * @param instance Instance à enregistrer
     */
    template<typename TInterface, typename TImplementation>
    void registerImplementation(std::shared_ptr<TImplementation> instance) {
        static_assert(std::is_base_of<TInterface, TImplementation>::value, 
                    "TImplementation must derive from TInterface");
        dependencies_[TypeIdGenerator::getTypeId<TInterface>()] = std::static_pointer_cast<void>(instance);
    }

    /**
     * @brief Récupère une instance partagée
     * 
     * @tparam T Type de l'instance à récupérer
     * @return std::shared_ptr<T> Instance partagée, ou nullptr si non trouvée
     */
    template<typename T>
    std::shared_ptr<T> resolve() {
        auto it = dependencies_.find(TypeIdGenerator::getTypeId<T>());
        if (it != dependencies_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        // Vérifier les factories
        auto factoryIt = factories_.find(TypeIdGenerator::getTypeId<T>());
        if (factoryIt != factories_.end()) {
            auto factory = std::any_cast<std::function<std::shared_ptr<T>()>>(&factoryIt->second);
            if (factory) {
                auto instance = (*factory)();
                registerDependency<T>(instance);
                return instance;
            }
        }

        return nullptr;
    }

    /**
     * @brief Récupère une instance partagée avec gestion d'erreurs explicite
     * 
     * @tparam T Type de l'instance à récupérer
     * @return Result<std::shared_ptr<T>> Instance partagée ou erreur détaillée
     */
    template<typename T>
    Result<std::shared_ptr<T>> resolveResult() {
        auto it = dependencies_.find(TypeIdGenerator::getTypeId<T>());
        if (it != dependencies_.end()) {
            auto instance = std::static_pointer_cast<T>(it->second);
            if (instance) {
                return Result<std::shared_ptr<T>>::success(instance);
            } else {
                return Result<std::shared_ptr<T>>::error({ErrorCode::DependencyMissing, "Failed to cast dependency to requested type"});
            }
        }

        // Vérifier les factories
        auto factoryIt = factories_.find(TypeIdGenerator::getTypeId<T>());
        if (factoryIt != factories_.end()) {
            auto factory = std::any_cast<std::function<std::shared_ptr<T>()>>(&factoryIt->second);
            if (factory) {
                auto instance = (*factory)();
                if (instance) {
                    registerDependency<T>(instance);
                    return Result<std::shared_ptr<T>>::success(instance);
                } else {
                    return Result<std::shared_ptr<T>>::error({ErrorCode::InitializationFailed, "Factory returned null instance"});
                }
            } else {
                return Result<std::shared_ptr<T>>::error({ErrorCode::InitializationFailed, "Failed to cast factory function"});
            }
        }

        return Result<std::shared_ptr<T>>::error({ErrorCode::DependencyMissing, "Dependency not registered"});
    }

    /**
     * @brief Enregistre une fabrique pour créer l'instance à la demande
     * 
     * @tparam T Type de l'instance à créer
     * @param factory Fonction qui crée l'instance
     */
    template<typename T>
    void registerFactory(std::function<std::shared_ptr<T>()> factory) {
        factories_[TypeIdGenerator::getTypeId<T>()] = factory;
    }

    /**
     * @brief Vérifie si une dépendance est enregistrée
     * 
     * @tparam T Type de la dépendance
     * @return true si la dépendance est enregistrée
     * @return false sinon
     */
    template<typename T>
    bool has() const {
        return dependencies_.find(TypeIdGenerator::getTypeId<T>()) != dependencies_.end() || 
               factories_.find(TypeIdGenerator::getTypeId<T>()) != factories_.end();
    }

    /**
     * @brief Supprime une dépendance enregistrée
     * 
     * @tparam T Type de la dépendance à supprimer
     * @return true si la dépendance a été supprimée
     * @return false si la dépendance n'était pas enregistrée
     */
    template<typename T>
    bool remove() {
        bool removed = false;
        
        auto depIt = dependencies_.find(TypeIdGenerator::getTypeId<T>());
        if (depIt != dependencies_.end()) {
            dependencies_.erase(depIt);
            removed = true;
        }
        
        auto factIt = factories_.find(TypeIdGenerator::getTypeId<T>());
        if (factIt != factories_.end()) {
            factories_.erase(factIt);
            removed = true;
        }
        
        return removed;
    }

    /**
     * @brief Efface toutes les dépendances enregistrées
     */
    void clear() {
        dependencies_.clear();
        factories_.clear();
    }

private:
    ETLConfig::DependencyMap<const void*, std::shared_ptr<void>> dependencies_;
    ETLConfig::DependencyMap<const void*, std::any> factories_;
};
