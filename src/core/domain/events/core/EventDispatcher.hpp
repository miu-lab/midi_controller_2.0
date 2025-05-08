#pragma once

#include "core/domain/events/core/Event.hpp"

/**
 * @brief Dispatcheur d'événements simplifié
 */
class EventDispatcher {
public:
    /**
     * @brief Constructeur
     * @param event Événement à dispatcher
     */
    EventDispatcher(Event& event) : event_(event) {}
    
    /**
     * @brief Dispatch un événement à un gestionnaire spécifique
     * 
     * @tparam T Type d'événement dérivé
     * @param handler Fonction de gestion pour le type spécifique
     * @return true si l'événement a été traité, false sinon
     */
    template <typename T>
    bool dispatch(bool (*handler)(const T&)) {
        // Si l'événement est déjà traité, on peut éviter la conversion
        if (event_.isHandled()) {
            return false;
        }
        
        // Si les types correspondent, appeler le gestionnaire
        if (event_.getType() == T::staticType()) {
            // Conversion dynamique sécurisée - ceci est sûr car nous avons vérifié le type
            const T& derivedEvent = static_cast<const T&>(event_);
            
            // Appeler le gestionnaire
            bool handled = handler(derivedEvent);
            
            // Marquer l'événement comme traité si le gestionnaire l'a traité
            if (handled) {
                event_.setHandled();
            }
            
            return handled;
        }
        
        return false;
    }
    
private:
    Event& event_;  // Référence à l'événement à dispatcher
};
