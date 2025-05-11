#pragma once

#include <functional>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <any>

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
        dependencies_[typeid(T)] = instance;
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
        dependencies_[typeid(TInterface)] = std::static_pointer_cast<void>(instance);
    }

    /**
     * @brief Récupère une instance partagée
     * 
     * @tparam T Type de l'instance à récupérer
     * @return std::shared_ptr<T> Instance partagée, ou nullptr si non trouvée
     */
    template<typename T>
    std::shared_ptr<T> resolve() {
        auto it = dependencies_.find(typeid(T));
        if (it != dependencies_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        // Vérifier les factories
        auto factoryIt = factories_.find(typeid(T));
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
     * @brief Enregistre une fabrique pour créer l'instance à la demande
     * 
     * @tparam T Type de l'instance à créer
     * @param factory Fonction qui crée l'instance
     */
    template<typename T>
    void registerFactory(std::function<std::shared_ptr<T>()> factory) {
        factories_[typeid(T)] = factory;
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
        return dependencies_.find(typeid(T)) != dependencies_.end() || 
               factories_.find(typeid(T)) != factories_.end();
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
        
        auto depIt = dependencies_.find(typeid(T));
        if (depIt != dependencies_.end()) {
            dependencies_.erase(depIt);
            removed = true;
        }
        
        auto factIt = factories_.find(typeid(T));
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
    std::unordered_map<std::type_index, std::shared_ptr<void>> dependencies_;
    std::unordered_map<std::type_index, std::any> factories_;
};
