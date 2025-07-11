#pragma once

#include "core/domain/events/core/Event.hpp"
#include "NavigationAction.hpp"
#include "AppState.hpp"

/**
 * @brief Types d'événements de navigation
 * 
 * Plage réservée : 4000-4999
 */
namespace NavigationEventTypes {
    constexpr EventType NAVIGATION_REQUESTED = 4000;
    constexpr EventType STATE_CHANGE_REQUESTED = 4001;
    constexpr EventType BACK_REQUESTED = 4002;
    constexpr EventType HOME_REQUESTED = 4003;
    constexpr EventType MENU_ROOT_REQUESTED = 4004;
}

/**
 * @brief Événement de navigation générique
 * 
 * Contient une action de navigation avec ses paramètres.
 */
class NavigationEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param action Action de navigation à exécuter
     * @param parameter Paramètre optionnel pour l'action
     */
    NavigationEvent(NavigationAction action, int parameter = 0)
        : Event(NavigationEventTypes::NAVIGATION_REQUESTED, EventCategory::UI),
          action_(action), parameter_(parameter) {}
    
    /**
     * @brief Obtient l'action de navigation
     */
    NavigationAction getAction() const { return action_; }
    
    /**
     * @brief Obtient le paramètre
     */
    int getParameter() const { return parameter_; }
    
    /**
     * @brief Nom de l'événement pour debug
     */
    const char* getEventName() const override { return "NavigationEvent"; }
    
private:
    NavigationAction action_;
    int parameter_;
};

/**
 * @brief Événement de changement d'état direct
 * 
 * Permet de forcer un changement d'état spécifique.
 */
class StateChangeEvent : public Event {
public:
    /**
     * @brief Constructeur
     * @param newState Nouvel état à définir
     * @param parameter Paramètre optionnel pour l'état
     * @param subState Sous-état optionnel
     */
    StateChangeEvent(AppState newState, uint8_t parameter = 0, uint8_t subState = 0)
        : Event(NavigationEventTypes::STATE_CHANGE_REQUESTED, EventCategory::UI),
          newState_(newState), parameter_(parameter), subState_(subState) {}
    
    /**
     * @brief Obtient le nouvel état
     */
    AppState getNewState() const { return newState_; }
    
    /**
     * @brief Obtient le paramètre
     */
    uint8_t getParameter() const { return parameter_; }
    
    /**
     * @brief Obtient le sous-état
     */
    uint8_t getSubState() const { return subState_; }
    
    /**
     * @brief Nom de l'événement pour debug
     */
    const char* getEventName() const override { return "StateChangeEvent"; }
    
private:
    AppState newState_;
    uint8_t parameter_;
    uint8_t subState_;
};

/**
 * @brief Événement "retour" rapide
 * 
 * Événement optimisé pour l'action "back" fréquente.
 */
class BackRequestedEvent : public Event {
public:
    /**
     * @brief Constructeur
     */
    BackRequestedEvent()
        : Event(NavigationEventTypes::BACK_REQUESTED, EventCategory::UI) {}
    
    /**
     * @brief Nom de l'événement pour debug
     */
    const char* getEventName() const override { return "BackRequestedEvent"; }
};

/**
 * @brief Événement "accueil" rapide
 * 
 * Événement optimisé pour l'action "home" fréquente.
 */
class HomeRequestedEvent : public Event {
public:
    /**
     * @brief Constructeur
     */
    HomeRequestedEvent()
        : Event(NavigationEventTypes::HOME_REQUESTED, EventCategory::UI) {}
    
    /**
     * @brief Nom de l'événement pour debug
     */
    const char* getEventName() const override { return "HomeRequestedEvent"; }
};

/**
 * @brief Événement retour à la page racine du menu
 * 
 * Demande au ViewManager de retourner à la page racine du menu LVGL.
 */
class MenuRootRequestedEvent : public Event {
public:
    /**
     * @brief Constructeur
     */
    MenuRootRequestedEvent()
        : Event(NavigationEventTypes::MENU_ROOT_REQUESTED, EventCategory::UI) {}
    
    /**
     * @brief Nom de l'événement pour debug
     */
    const char* getEventName() const override { return "MenuRootRequestedEvent"; }
};