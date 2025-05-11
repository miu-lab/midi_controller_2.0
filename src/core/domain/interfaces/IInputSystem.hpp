#pragma once

/**
 * @brief Interface pour le système d'entrée
 * 
 * Cette interface définit les méthodes que tous les systèmes d'entrée doivent implémenter.
 */
class IInputSystem {
public:
    virtual ~IInputSystem() = default;
    
    /**
     * @brief Initialise le système d'entrée
     */
    virtual void init() = 0;
    
    /**
     * @brief Met à jour l'état du système d'entrée
     */
    virtual void update() = 0;
};
