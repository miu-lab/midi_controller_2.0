#pragma once

#include <string>

/**
 * @brief Interface pour le système d'interface utilisateur
 * 
 * Cette interface définit les méthodes que tous les systèmes UI doivent implémenter.
 */
class IUISystem {
public:
    virtual ~IUISystem() = default;
    
    /**
     * @brief Initialise le système d'interface utilisateur
     * 
     * @param enableFullUI Indique si l'interface complète doit être activée
     */
    virtual void init(bool enableFullUI = false) = 0;
    
    /**
     * @brief Met à jour l'état du système d'interface utilisateur
     */
    virtual void update() = 0;
    
    /**
     * @brief Affiche un message sur l'écran
     * 
     * @param message Le message à afficher
     */
    virtual void showMessage(const std::string& message) = 0;
    
    /**
     * @brief Efface l'écran
     */
    virtual void clearDisplay() = 0;
};
