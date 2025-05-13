#pragma once

#include <memory>
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Interface de base pour les vues
 * 
 * Classe abstraite définissant l'interface commune pour toutes les vues
 * de l'interface utilisateur.
 */
class View {
public:
    /**
     * @brief Constructeur prenant un pointeur vers l'affichage
     * @param display Pointeur vers l'adaptateur d'affichage
     */
    explicit View(std::shared_ptr<DisplayPort> display) : display_(display) {}
    
    /**
     * @brief Destructeur virtuel
     */
    virtual ~View() = default;
    
    /**
     * @brief Initialise la vue
     * @return true si l'initialisation a réussi, false sinon
     */
    virtual bool init() = 0;
    
    /**
     * @brief Met à jour la vue
     */
    virtual void update() = 0;
    
    /**
     * @brief Rend la vue sur l'affichage
     */
    virtual void render() = 0;
    
    /**
     * @brief Gère les événements d'entrée pour cette vue
     * @param eventType Type d'événement
     * @param data Données associées à l'événement
     * @return true si l'événement a été traité, false sinon
     */
    virtual bool handleEvent(uint8_t eventType, int32_t data) = 0;
    
    /**
     * @brief Vérifie si la vue est active
     * @return true si la vue est active, false sinon
     */
    virtual bool isActive() const = 0;
    
    /**
     * @brief Active ou désactive la vue
     * @param active Nouvel état d'activation
     */
    virtual void setActive(bool active) = 0;

protected:
    std::shared_ptr<DisplayPort> display_; ///< Référence à l'affichage
    bool active_ = false; ///< Indique si la vue est actuellement active
};
