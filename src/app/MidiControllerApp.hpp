// app/MidiControllerApp.hpp
#pragma once

#include <Arduino.h>
#include <vector>

#include "adapters/secondary/storage/ProfileManager.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/services/UiEventService.hpp"
#include "app/services/MidiSystem.hpp"
#include "app/services/InputSystem.hpp"
#include "app/services/ConfigurationService.hpp"
#include "app/services/ServiceLocator.hpp"
#include "config/ApplicationConfiguration.hpp"

/**
 * @brief Application principale du contrôleur MIDI
 */
class MidiControllerApp {
public:
    /**
     * @brief Construit l'application avec la configuration spécifiée
     * @param appConfig Configuration de l'application
     */
    MidiControllerApp(const ApplicationConfiguration& appConfig);

    /// alias pour setup()
    void init()   { begin(); }
    /// alias pour loop()
    void update() { tick(); }

    /**
     * @brief Définit un contrôle comme étant dédié à la navigation
     * @param id Identifiant du contrôle
     * @param isNavigation true pour dédier à la navigation, false pour MIDI normal
     */
    void setControlForNavigation(ControlId id, bool isNavigation = true);

    /**
     * @brief Vérifie si un contrôle est dédié à la navigation
     * @param id Identifiant du contrôle à vérifier
     * @return true si le contrôle est dédié à la navigation, false sinon
     */
    bool isNavigationControl(ControlId id) const;

    void begin();
    void tick();

private:
    // Service de configuration centralisée
    ConfigurationService    configService_;    // Gestion de toutes les configurations
    
    // Gestion des profils et navigation
    ProfileManager          profileManager_;   // Stockage des mappings MIDI
    NavigationConfigService navigationConfig_; // Configuration des contrôles de navigation
    
    // Systèmes
    InputSystem             inputSystem_;      // Système d'entrée
    MidiSystem              midiSystem_;       // Système MIDI
    UiEventService          uiService_;        // Gestion des événements d'UI
};
