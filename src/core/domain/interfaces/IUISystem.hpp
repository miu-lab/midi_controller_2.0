#pragma once

#include <string>
#include "core/utils/Result.hpp"

/**
 * @brief Interface pour le système d'interface utilisateur
 * 
 * Cette interface définit les méthodes que tous les systèmes UI doivent implémenter.
 * Elle permet de gérer l'affichage et l'interaction avec l'utilisateur.
 */
class IUISystem {
public:
    virtual ~IUISystem() = default;
    
    /**
     * @brief Initialise le système d'interface utilisateur
     * 
     * @param enableFullUI Indique si l'interface complète doit être activée
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> init(bool enableFullUI = false) = 0;
    
    /**
     * @brief Met à jour l'état du système d'interface utilisateur
     * 
     * Cette méthode doit être appelée régulièrement pour mettre à jour l'affichage.
     */
    virtual void update() = 0;
    
    /**
     * @brief Affiche un message sur l'écran
     * 
     * @param message Le message à afficher
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> showMessage(const std::string& message) = 0;
    
    /**
     * @brief Efface l'écran
     * 
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> clearDisplay() = 0;
};
