// app/MidiControllerApp.hpp
#pragma once

#include <Arduino.h>

#include "app/services/ConfigurationService.hpp"
#include "app/services/InputSystem.hpp"
#include "app/services/MidiSystem.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/services/ServiceLocator.hpp"
#include "app/services/UiEventService.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "app/services/EventInputSystem.hpp"
#include "core/listeners/UIEventListener.hpp"
#include "core/listeners/UIControllerEventListener.hpp"
#include <memory> // Pour std::unique_ptr et std::make_unique

/**
 * @brief Application principale du contrôleur MIDI
 */
class MidiControllerApp {
public:
    /**
     * @brief Constructeur
     * @param appConfig Configuration de l'application
     */
    explicit MidiControllerApp(const ApplicationConfiguration& appConfig);
    
    /**
     * @brief Destructeur - libère les ressources et se désabonne du bus d'événements
     */
    virtual ~MidiControllerApp();

    /**
     * @brief Définit si un contrôle est utilisé pour la navigation
     * @param id Identifiant du contrôle
     * @param isNavigation true si le contrôle est utilisé pour la navigation, false sinon
     */
    void setControlForNavigation(ControlId id, bool isNavigation);

    /**
     * @brief Vérifie si un contrôle est utilisé pour la navigation
     * @param id Identifiant du contrôle
     * @return true si le contrôle est utilisé pour la navigation, false sinon
     */
    bool isNavigationControl(ControlId id) const;

    /**
     * @brief Initialise l'application
     */
    void init();

    /**
     * @brief Met à jour l'application
     */
    void update();

private:
    // Services standard
    ConfigurationService configService_;
    NavigationConfigService navigationConfig_;
    ProfileManager profileManager_;
    MidiSystem midiSystem_;
    
    // Système d'événements
    EventInputSystem eventInputSystem_;
    UiEventService uiEventService_;
    
    // Écouteurs d'événements
    std::unique_ptr<UIControllerEventListener> uiControllerEventListener_;
    std::unique_ptr<UIEventListener> uiEventListener_;
    
    // IDs d'abonnements
    SubscriptionId uiControllerEventListenerSubId_ = 0;
    SubscriptionId uiEventListenerSubId_ = 0;
};