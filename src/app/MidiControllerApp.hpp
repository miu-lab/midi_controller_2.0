#include "core/domain/commands/CommandManager.hpp"// app/MidiControllerApp.hpp
#pragma once

#include <Arduino.h>

#include <vector>

#include "adapters/secondary/storage/ProfileManager.hpp"
#include "app/services/ConfigurationService.hpp"
#include "app/services/InputSystem.hpp"
#include "app/services/MidiSystem.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/services/ServiceLocator.hpp"
#include "app/services/UiEventService.hpp"
#include "app/services/ControllerService.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/UIController.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "adapters/primary/ui/ViewManager.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "core/controllers/MenuController.hpp"
#include "config/ApplicationConfiguration.hpp"

/**
 * @brief Application principale du contrôleur MIDI
 *
 * Cette classe est le point d'entrée de l'application. Elle initialise et coordonne
 * tous les systèmes, services et contrôleurs nécessaires au fonctionnement du
 * contrôleur MIDI. L'architecture suit un modèle hexagonal avec une séparation
 * claire des responsabilités et une injection de dépendances.
 */
class MidiControllerApp {
public:
    /**
     * @brief Construit l'application avec la configuration spécifiée
     * @param appConfig Configuration de l'application
     */
    MidiControllerApp(const ApplicationConfiguration& appConfig);

    /**
     * @brief Initialise l'application
     */
    void init();

    /**
     * @brief Met à jour l'application à chaque cycle
     */
    void update();

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
private:
    // Service de configuration centralisée
    ConfigurationService configService_;  // Gestion de toutes les configurations

    // Gestion des profils et navigation
    ProfileManager profileManager_;             // Stockage des mappings MIDI
    NavigationConfigService navigationConfig_;  // Configuration des contrôles de navigation

    // Systèmes
    InputSystem inputSystem_;   // Système d'entrée
    MidiSystem midiSystem_;     // Système MIDI
    UiEventService uiService_;  // Gestion des événements d'UI

    // Note: Les contrôleurs sont gérés par le ServiceLocator - pas besoin de les stocker ici
};
