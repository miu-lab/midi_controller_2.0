// app/ControllerApp.hpp
#pragma once

#include <Arduino.h>
#include <vector>

#include "storage/ProfileManager.hpp"
#include "app/NavigationConfigService.hpp"
#include "app/UiEventService.hpp"
#include "app/MidiSystem.hpp"
#include "app/InputSystem.hpp"
#include "app/ConfigurationService.hpp"


/**
 * @brief Application principale du contrôleur MIDI
 */
class ControllerApp {
public:
    ControllerApp();

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


