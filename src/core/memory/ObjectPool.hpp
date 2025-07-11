#pragma once

#include <bitset>
#include <memory>
#include <type_traits>
#include <cstddef>
#include <new>
#include "config/SystemConstants.hpp"

/**
 * @brief Pool d'objets à taille fixe pour systèmes embarqués
 * 
 * Cette classe implémente un pool d'objets avec allocation statique
 * pour éviter la fragmentation mémoire et améliorer les performances.
 * 
 * @tparam T Type d'objet à gérer dans le pool
 * @tparam N Nombre maximum d'objets dans le pool
 */
template<typename T, size_t N>
class ObjectPool {
public:
    static_assert(N > 0, "Pool size must be greater than 0");
    static_assert(std::is_destructible_v<T>, "Type must be destructible");
    
    /**
     * @brief Constructeur par défaut
     */
    ObjectPool() : used_mask_{}, allocated_count_(0) {
        // Le storage est déjà initialisé (mémoire brute)
    }
    
    /**
     * @brief Destructeur - détruit tous les objets actifs
     */
    ~ObjectPool() {
        // Détruire tous les objets encore alloués
        for (size_t i = 0; i < N; ++i) {
            if (used_mask_[i]) {
                reinterpret_cast<T*>(&storage_[i])->~T();
            }
        }
    }
    
    /**
     * @brief Acquiert un objet du pool
     * 
     * @tparam Args Types des arguments du constructeur
     * @param args Arguments à passer au constructeur de T
     * @return T* Pointeur vers l'objet alloué, ou nullptr si le pool est plein
     */
    template<typename... Args>
    T* acquire(Args&&... args) {
        // Chercher le premier slot libre
        for (size_t i = 0; i < N; ++i) {
            if (!used_mask_[i]) {
                // Marquer comme utilisé
                used_mask_[i] = true;
                allocated_count_++;
                
                // Construire l'objet in-place
                T* obj = reinterpret_cast<T*>(&storage_[i]);
                new (obj) T(std::forward<Args>(args)...);
                
                return obj;
            }
        }
        
        // Pool plein
        return nullptr;
    }
    
    /**
     * @brief Libère un objet et le retourne au pool
     * 
     * @param obj Pointeur vers l'objet à libérer
     * @return true si l'objet a été libéré avec succès
     * @return false si l'objet n'appartient pas à ce pool
     */
    bool release(T* obj) {
        if (!obj) {
            return false;
        }
        
        // Vérifier que l'objet appartient à ce pool
        const char* obj_ptr = reinterpret_cast<const char*>(obj);
        const char* pool_start = reinterpret_cast<const char*>(&storage_[0]);
        const char* pool_end = pool_start + (N * sizeof(ObjectStorage));
        
        if (obj_ptr < pool_start || obj_ptr >= pool_end) {
            return false; // Objet n'appartient pas à ce pool
        }
        
        // Calculer l'index
        size_t index = (obj_ptr - pool_start) / sizeof(ObjectStorage);
        
        if (index >= N || !used_mask_[index]) {
            return false; // Index invalide ou objet déjà libéré
        }
        
        // Détruire l'objet
        obj->~T();
        
        // Marquer comme libre
        used_mask_[index] = false;
        allocated_count_--;
        
        return true;
    }
    
    /**
     * @brief Obtient le nombre d'objets actuellement alloués
     * 
     * @return size_t Nombre d'objets alloués
     */
    size_t allocated_count() const {
        return allocated_count_;
    }
    
    /**
     * @brief Obtient le nombre d'objets disponibles
     * 
     * @return size_t Nombre d'objets disponibles
     */
    size_t available_count() const {
        return N - allocated_count_;
    }
    
    /**
     * @brief Obtient la taille totale du pool
     * 
     * @return size_t Taille totale du pool
     */
    constexpr size_t capacity() const {
        return N;
    }
    
    /**
     * @brief Vérifie si le pool est plein
     * 
     * @return true si le pool est plein
     * @return false sinon
     */
    bool is_full() const {
        return allocated_count_ >= N;
    }
    
    /**
     * @brief Vérifie si le pool est vide
     * 
     * @return true si le pool est vide
     * @return false sinon
     */
    bool is_empty() const {
        return allocated_count_ == 0;
    }
    
    /**
     * @brief Obtient les statistiques d'utilisation
     * 
     * @return float Pourcentage d'utilisation (0.0 - 1.0)
     */
    float usage_ratio() const {
        return static_cast<float>(allocated_count_) / static_cast<float>(N);
    }

private:
    // Empêcher la copie et l'assignation
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    ObjectPool(ObjectPool&&) = delete;
    ObjectPool& operator=(ObjectPool&&) = delete;
    
    /**
     * @brief Storage aligné pour les objets
     */
    struct alignas(T) ObjectStorage {
        char data[sizeof(T)];
    };
    
    ObjectStorage storage_[N];              ///< Storage statique pour les objets
    std::bitset<N> used_mask_;              ///< Masque des slots utilisés
    size_t allocated_count_;                ///< Nombre d'objets actuellement alloués
};

/**
 * @brief RAII Guard pour libération automatique d'objets du pool
 * 
 * @tparam T Type d'objet géré
 * @tparam N Taille du pool
 */
template<typename T, size_t N>
class PoolGuard {
public:
    /**
     * @brief Constructeur avec objet à gérer
     * 
     * @param pool Référence vers le pool
     * @param obj Pointeur vers l'objet à gérer
     */
    PoolGuard(ObjectPool<T, N>& pool, T* obj) 
        : pool_(pool), obj_(obj) {}
    
    /**
     * @brief Destructeur - libère automatiquement l'objet
     */
    ~PoolGuard() {
        if (obj_) {
            pool_.release(obj_);
        }
    }
    
    /**
     * @brief Déréférencement
     */
    T& operator*() const { return *obj_; }
    
    /**
     * @brief Accès aux membres
     */
    T* operator->() const { return obj_; }
    
    /**
     * @brief Accès au pointeur brut
     */
    T* get() const { return obj_; }
    
    /**
     * @brief Vérifie si l'objet est valide
     */
    explicit operator bool() const { return obj_ != nullptr; }
    
    /**
     * @brief Libère la propriété de l'objet
     */
    T* release() {
        T* temp = obj_;
        obj_ = nullptr;
        return temp;
    }

private:
    // Empêcher la copie
    PoolGuard(const PoolGuard&) = delete;
    PoolGuard& operator=(const PoolGuard&) = delete;
    
    ObjectPool<T, N>& pool_;
    T* obj_;
};

// Alias pour les pools spécialisés avec tailles configurées
namespace Pools {
    /**
     * @brief Pool pour les événements généraux
     */
    template<typename T>
    using EventPool = ObjectPool<T, SystemConstants::Performance::EVENT_POOL_SIZE>;
    
    /**
     * @brief Pool pour les événements MIDI
     */
    template<typename T>
    using MidiEventPool = ObjectPool<T, SystemConstants::Performance::MIDI_EVENT_POOL_SIZE>;
    
    /**
     * @brief Pool pour les événements UI
     */
    template<typename T>
    using UIEventPool = ObjectPool<T, SystemConstants::Performance::UI_EVENT_POOL_SIZE>;
    
    /**
     * @brief Guards correspondants
     */
    template<typename T>
    using EventGuard = PoolGuard<T, SystemConstants::Performance::EVENT_POOL_SIZE>;
    
    template<typename T>
    using MidiEventGuard = PoolGuard<T, SystemConstants::Performance::MIDI_EVENT_POOL_SIZE>;
    
    template<typename T>
    using UIEventGuard = PoolGuard<T, SystemConstants::Performance::UI_EVENT_POOL_SIZE>;
    
} // namespace Pools